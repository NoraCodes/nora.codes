---
title: "Building Readable Tests with Fluent Testing APIs"
date: 2018-07-23T09:56:24-05:00
tags: ['rust', 'programming', 'testing']
description: A discussion of some issues my former employer ran into with extremely verbose and fragile Rust unit tests, which we solved with a fluent testing API.
---

One of the biggest challenges in software testing is defining the input for code under
test in a way that is expressive and powerful enough to test complex situations but
doesn’t distract from the intent of the test or clutter the test code to a degree that
makes it difficult to read. 

Many dynamic languages have testing APIs which take advantage of their looser and later
type checking to provide easy mocking and stubbing, but strict, statically typed
languages can make it difficult to build up suitable instances of the types needed in the
test. This is especially true with nested data.

Motivation
----------

At [CancerIQ](https://canceriq.com), we have a large body of tests to ensure we correctly
implement our risk models. These tests are thorough and give us a high
confidence that new features or refactors aren’t breaking old
functionality.

We do have one problem, however: fragility and verbosity. Our tests are
very complete but, until the changes described here, they didn’t
express what they were testing well, and they were very difficult to
change.

In this post, you’ll see how we got from fragile and hard-to-understand
tests to short, expressive, and functionally equivalent tests
with fluent-style testing APIs.

The Literal Approach
--------------------

Originally, these tests were written using literals.
For instance, consider a `Person` that looks something like this:

```rust
struct Person {
    pub id: String,
    pub age: Option<u8>,
    pub mother_id: Option<String>,
    pub father_id: Option<String>,
    pub children: Option<Vec<String>>,
    pub medical_history: Option<MedicalHistory>,
    pub reproductive_history: Option<ReproductiveHistory>,
    // ... and many more fields
}
```

Of course, the `MedicalHistory` and `ReproductiveHistory` structs are
themselves nontrivial, and because we are dealing with data from
inconsistent sources (patient reporting), almost every field is
`Option`al. Our tests ended up looking something like this:

```rust
#[test]
fn patient_with_field_that_matters_value_is_flagged() {
    let patient = Patient {
        id: "1".into(),
        age: None,
        mother_id: None,
        father_id: None,
        children: None,
        medical_history: Some(MedicalHistory{
            diagnoses: vec![Diagnosis {
                type: DiagnosisType::SomeIllnessOrOther,
                field_a: None,
                field_b: None,
                field_that_matters: Some(31337),
                field_d: None,
            }]
        }),
        reproductive_history: None,
        // Followed by many more Nones
    };
    assert!(function_under_test(patient),
           "Function did not return True for patient who should be at risk!");
}
```

Is it clear what we’re doing here? Not really. It turns out, we only
care about one field here: `field_that_matters`, on the `Patient`’s
`MedicalHistory`. In addition, this test is 21 lines long - far too long
for a test which is only evaluating a single function call.

More importantly, though, this test is totally inflexible. Whenever we
had to add a field to the `Diagnosis` struct, our tests stopped
compiling, even though the field was `Option`al and not important here.
With hundreds of such fragile tests in the codebase, making even basic
changes became very slow.

First Steps
-----------

The most obvious way to solve both fragility and verbosity was to
encapsulate this initialization into a function, which we did. These
functions took some basic info and then tests with more complex
requirements would add the properties they needed. For instance:

```rust
fn patient_with_field_that_matters_is_flagged() {
    let mut patient = make_basic_patient("1", None, None, None, None);
    let diagnosis = make_diagnosis(
        Diagnosis_Type::SomeIllnessOrOther,
        None,
        None,
        Some(31337),
        None);
    patient.medical_history = MedicalHistory{
        diagnoses: Some(MedicalHistory{
            diagnoses: vec![diagnosis]
        })
    }
    assert!(function_under_test(patient),
           "Function did not return True for patient who should be at risk!");
}
```

This approach does make the test more concise, but it actually reduces
readability due to the large number of unnamed function arguments.
Without looking up the signature of `make_diagnosis`, a reader has no
way to know that the value given is going into `field_that_matters`.

This can also be done with Rust’s struct update syntax, either by
implementing `Default` or with an explicit `::base()` associated
function:

```rust
fn patient_with_field_that_matters_is_flagged() {
    let patient = Patient {
        id: "1".into(),
        medical_history: Some(MedicalHistory{
            diagnoses: Some(vec![ Diagnosis {
                field_that_matters: Some(31337),
                .. Diagnosis::base(DiagnosisType::SomeIllnessOrOther)
            }])
        }),
        .. Default::default()
    }
    assert!(function_under_test(patient),
           "Function did not return True for patient who should be at risk!");
}
```

Now, only the `::base()` method or the implementation of `Default` has
to be modified when a struct field is added or modified. This is a good
first step, but it doesn’t fully solve the problem of clarity.

Taking Inspiration 
------------------

I don’t have a lot of occasion to work with dynamic languages, but I was pairing with
another engineer on some Ruby code and noticed how nice the testing API was with RSpec.
This lead me down a rabbit hole which eventually brought me to [fluent
programming](https://en.wikipedia.org/wiki/Fluent_interface).

In short, the idea of a fluent API is that it can be read almost
like a sentence in a natural language. We built an API with a similar
idea, which is what is currently used in the risk model tests. They look
something like this:

```rust
fn patient_with_field_that_matters_is_flagged() {
    let patient = Patient::base("1").with_medical_history(
        MedicalHistory::base().and_diagnosis(
            Diagnosis::base(DiagnosisType::SomeIllnessOrOther)
                .with_field_that_matters(31337)
        )
    );
    assert(function_under_test(patient),
           "Function did not return True for patient who should be at risk!");
}
```

The advantages of this approach become even clearer when working with
tests that require many associated structs. For example, consider this
code:

```rust
let patient = Person::base("1", Gender::Male)
    .with_father("2")
    .with_mother("3")
    .with_children(&["4", "5"])
    .with_age(53);
let father = Person::base("2", Gender::Male).deceased_at(84);
let mother = Person::base("3", Gender::Female).with_medical_history(
    MedicalHistory::base()
        .and_diagnosis(Diagnosis::base(DiagnosisType::CommonCold).at_age(32))
        .and_diagnosis(Diagnosis::base(DiagnosisType::AvianFlu)
            .with_treatment(Treatment::base(Treatment::SomeFluTreatment))));
let child1 = Person::base("4", Gender::Female);
let child2 = Person::base("5", Gender::Female)
    .with_mutation(GeneticMutation::BlueHairAllele_pg5s77);
```

This is a *lot* of information! We’ve effectively defined a DAG (a
family tree) in fewer lines than our original test, and it’s trivial to
read through this.

> “Who’s the father?” “Oh, he’s a male who died at 84.”
>
> “Who’s the mother?” “She’s a woman who got the cold at 32 and was
> treated for avian flu.”
>
> “Who’s the patient?” “He’s a 53 year old man with two children.”

The critical idea here is that the API doesn’t require specifying that
you *don’t* have some information; rather, you use methods to specify
which pieces of information you *do* have.

Implementation
--------------

Implementation of these methods is efficient and straightforward in
Rust, using the aforementioned update syntax. Consider the following
struct:

```rust
struct ShippingItem {
    pub volume: f64,
    pub condition: Option<Condition>,
    pub sender: Option<String>,
    pub recipient: Option<String>,
    pub certified: Option<bool>,
    pub contents: Option<Vec<InventoryObject>>,
    // et cetera
}
```

This struct has 1 required field and several optional fields. Because of
the required field, just implementing `Default` isn’t an option, so the
`::base()` method is the way to go:

```rust
impl ShippingItem {
    pub fn base(volume: f64) -> ShippingItem {
        ShippingItem {
            volume,
            condition: None,
            sender: None,
            recipient: None,
            certified: None,
            contents: None,
        }
    }
}
```

Then, for each basic `Option`al field, we’ll add a method. This includes
`condition`, `sender`, and `recipient`.

```rust
impl ShippingItem {
    pub fn with_condition(self, cond: Condition) -> Self {
        ShippingItem {
            condition: cond,
            ..self
        }
    }

    pub fn with_sender(self, sender: &str) -> Self {
        ShippingItem {
            sender: sender.into(),
            ..self
        }
    }

    pub fn with_recipient(self, recipient: &str) -> Self {
        ShippingItem {
            recipient: recipient.into(),
            ..self
        }
    }
}
```

Each of these takes ownership of `self` and returns a new
`ShippingItem`, allowing the creation of long sentence-like
descriptions.

For boolean values, two methods are preferrable.

```rust
impl ShippingItem {
    pub fn certified(self) -> Self {
        ShippingItem {
            certified: Some(true),
            ..self
        }
    }
    
    pub fn not_certified(self) -> Self {
        ShippingItem {
            certified: Some(false),
            ..self
        }
    }
}
```

`Option`al lists provide something more of a challenge. We settled on a
two-method API, providing one method for replacing the contents with a
slice and one for adding a single item.

```rust
impl ShippingItem {
    pub fn with_contents(self, contents: &[InventoryObject]) -> Self {
        ShippingItem {
            contents: Some(contents.into()),
            ..self
        }
    }

    pub fn and_contents(self, object: InventoryObject) -> Self {
        let contents = self.contents.unwrap_or(Some(Vec::new()));
        contents.push(object);
        ShippingItem {
            contents,
            ..self
        }
    }
}
```

Now we can write our tests with this API! For example, let’s say we
wanted to test a shipping validation function. We can specify only the required
information, rather than all the possible fields.

```rust
#[test]
fn same_sender_and_receiver_not_shippable() {
    let item = ShippingItem::base(1.0)
        .with_sender("Somebody T. Something")
        .with_receiver("Somebody T. Something"); 
    assert!(!item.validate());
}
```

Conclusion and Use Cases
------------------------

This technique isn’t for every project. The main drawback is that it
requires a fair amount of boilerplate for each struct, and that the
`::base()` method needs an argument for every required item in the
struct (as opposed to a builder-style pattern).

However, for any system in which you need to test a lot of computations
on a struct with a lot of optional fields, you should consider this
fluent-style testing pattern. It’s made our tests more readable, easier
to write, and easier to modify, and it can probably do the same for you.

> Note: This post was written while I was working at CancerIQ as a software engineering
> intern, and was also posted on their engineering blog. You can find it [here](http://engineering.canceriq.com/posts/building_readable_tests_with_fluent_testing_apis/).
