---
date: 2017-08-01
title: A Gentle Introduction to Practical Types
slug: a-gentle-introduction-to-practical-types
categories:
- Programming
- Math
tags:
- programming
- programming languages
- type theory
---

## Types

Programmers talk a lot about types, but what is a "type", anyway? It is, in essence, _the set of all possible values_ for, well, some _type_ of value.

In a logical sense, it tells us _what we can do with a value_. For example, when speaking about numbers, we might say, "let x be any integer" or "let y be any real number not equal to zero". These statements tell us what we can do with these values; we know that we can add, subtract, or multiply x and y. We cannot necessarily divide by x (because x might be equal to zero), though we can divide by y. 

In programming, most languages have many built in types, and not just numbers; strings (of characters; that is, text) and more exotic things like pointers are all types of values that can be found in most programs, and the types of those values essentially tell us what we can do with them.

Types also tell us _how to represent data_. For instance, in Python, a variable of type `int` is not "any integer"; it is any integer between -2^63 and 2^63 - 1, because it is represented in memory as 64 bits, the first of which is the sign (negative or positive) and the rest of which are the absolute value of the number.

But before we can really talk about integers, decimal numbers, or text, we need to understand a little more about types themselves.

## The Naming of Things
The first type I'm going to make is something that mathematicians call a Σ type or sum type. What it really is is a list of options; an _enumeration_ of possible values, or, in many programming languages, an `enum`. Each option has its own type, and the enumeration of thes options creates a new type. 

Here's a type that may be familiar to you: a Boolean value. It can be either True, or False. Nothing else. Here's how we would write that down in a programming language (Rust, in this case):

{{< highlight rust >}}
enum Boolean {
    True,
    False
}
{{< /highlight >}}

This is saying, "Create a new enumeration type. It's called the Boolean type, and every value of this type must be one of the following: True, or False."

We can figure out a few things about this. For instance, we can check if two values of type Boolean are equal or not; we can also define the Boolean functions like `and`, `or`, and `xor`, if we wish.

We need not be restricted to two options, however. For instance, here's a type that you might use when giving someone directions:

{{< highlight rust >}}
enum StreetDirection {
    Left,
    Right,
    Straight,
}
{{< /highlight >}}

We can use these two types to apply some type theory to a conversation. Let's say Alice is giving Bob directions. Bob might ask, "Which way do I go at Laurel Street?" He is requesting a value of type StreetDirection; it would make no sense if Alice were to say "Blue" or "False".

We can extend this construct to create very useful types like and unsigned integer (the always-positive variant of the bounded `int` discussed above). It can hold values from 0 to 2^64 - 1. Were we to define it using this syntax, it would look something like this:

{{< highlight rust >}}
enum PositiveBoundedInteger {
    0,
    1,
    2,
    3,
    (skip a few)
    18446744073709551615,
    18446744073709551616
}
{{< /highlight >}}

Of course, nobody would ever actually write out all the integers from 0 to 18446744073709551616, but we _could_ define the type that way. We could also begin to write functions like addition, subtraction, and multiplication.

## Doing More with Enumerations

Consider the power of these sum or enumeration types; we can write the type of anything that is only one of a number of options, whether those options are ordered (like the PositiveBoundedInteger type above) or unordered (like Boolean values and directions). 

These types are called sum types because the number of possible values is the _sum_ of the number of options; for now, that's just 1 each - there are 2 possible values of type Boolean, for instance - but that won't always be true. We can define types that are the combinations of these types. 

For example, we could define a type that is the combination of itself and a StreetDirection. We could imagine a controller giving a robot directions on how to move around with this type:

{{< highlight rust >}}
enum MovementInstruction {
    Go(StreetDirection),
    NoGo,
}
{{< /highlight >}}

The MovementInstruction type has 2 options, but one of those options has a StreetDirection in it! Therefore, we could write little sets of instructions for our hypothetical robot to solve a maze:

{{< highlight text >}}
Go(Straight)
Go(Right)
Go(Straight)
NoGo
{{< /highlight >}}

It's pretty easy to figure out how many options there are: Go(Straight), Go(Right), Go(Left), and NoGo. That is, the sum of the first option's possible values (3) and the second option's possible values (1). Thus, the number of possibilities for values is the _sum_ of the number of possiblities for values of the types of the enumerated options. Enumeration types are sum types.

## Getting Productive

Here's a challenge for you, as a sort of perverse reward for making it through sum/enum types. How would you use an `eunm` type to represent values that record how many cats and dogs someone has?


I'd start like this:

{{< highlight rust >}}
enum Pets{
    NoCatsNoDogs,
    NoCatsSomeDogs(BoundedPositiveInteger),
    SomeCatsNoDogs(BoundedPositiveInteger),
}
{{< /highlight >}}

This is a good start. We can talk about people with no pets, or only one type of of pet. We do have a problem, though; what about people who have dogs _and_ cats? This is where product types or `struct`ured types come in. We can write something like this:

{{< highlight rust >}}
struct Pets {
    dogs: BoundedPositiveInteger,
    cats: BoundedPositiveInteger,
}
{{< /highlight >}}

This definition means, "Create a new structured type. It's name is Pets, and it consists of one value of type BoundedPositiveInteger, called dogs, followed by another value of type BoundedPositiveInteger, called cats." We call it a structured type because we define it not by a list of possible values but by the structure of any possible value; in this case, two arbitrary BoundedPositiveIntegers. 

We could denote the state of owning no pets as:

{{< highlight rust >}}
Pets {
    dogs: 0,
    cats: 0,
}
{{< /highlight >}}

That is, no dogs and no cats. A person with, say, 4 dogs and 3 cats would be:

{{< highlight rust >}}
Pets {
    dogs: 4,
    cats: 3,
}
{{< /highlight >}}

Consider the number of possible values of the Pets type. There are 2^64 possible values of BoundedPositiveInteger, and here there are two BoundedPositiveIntegers, neither dependent on the other. Simple combinatorics tells us that there are 2^64 * 2^64 possible permutations; the _product_ of the number of values of the two consituent types. 

## Combining the Combinations

Product and sum types can be combined to create any type. For instance, we could create a signed integer type, based on our existing BoundedPositiveInteger:

{{< highlight rust >}}
struct BoundedInteger {
    isNegative: Boolean,
    absoluteValue: BoundedPositiveInteger,
}
{{< /highlight >}}

This would have as many values as BoundedPositiveInteger (2^64) times as many values as Boolean (2); in other words, 2^65 values.

## More Exotic Types

We can use the combination of sum types and product types in another way: to define lists. Let's say we wanted a list of numbers; perhaps winning lottery numbers. For this, we need two types:

{{< highlight rust >}}
struct IntegerListNode {
    value: BoundedInteger,
    link: IntegerListLink,
}

enum IntegerListLink {
    HasNextNode(IntegerListNode),
    NoNextNode,
}
{{< /highlight >}}

Each node holds a link which points to either another node (the next in the list) or to nothing (signifying the end of the list). We could construct a list and then look at each of its elements in sequence. Writing such a list out is a bit daunting; The list 1, 2, 3 would read, in full:

{{< highlight rust >}}
IntegerListNode {
    value: 1,
    link: HasNextNode(
        IntegerListNode {
            value: 2,
            link: HasNextNode(
                IntegerListNode {
                    value: 3,
                    link: NoNextNode,
                }
            ),
        }
    ),
}
{{< /highlight >}}

We do run into a bit of a problem, though, when looking at the size of this type. Clearly, the IntegerListNode type has as many possible values as a BoundedInteger times as many possible values as an IntegerListLink. We know that the BoundedInteger type has 2^65 values, but how many does IntegerListLink have? Well, it's the sum of 1 (for NoNextNode) plus... however many possible values IntegerListNode has! This list type is a type with infinitely many possible values, because the nesting of nodes and links can go on for arbitrarily long.

Interestingly, this causes the Rust compiler (which sadly must exist in the real world) to reject this type as invalid, because it cannot generate a representation that would fit into a computer's memory. See [the Rust Book](https://doc.rust-lang.org/book/second-edition/ch15-01-box.html) for more info.

## Recap & Further Reading

It's been a long journey. We've gone from nothing, through finite lists of possible values, to combinations of those lists and, finally, types with infinitely many possible values. Go forth and apply this knowledge!

To help you in actually understanding how to use the type system of Rust specifically, I suggest the excellent [Rust Book](https://doc.rust-lang.org/book/second-edition) and [this talk on the type system](https://www.youtube.com/watch?v=wxPehGkoNOw&index=6&list=PL85XCvVPmGQhUSX_QBkxb4g1-o56cCqI9) from RustConf 2017. Traits and lifetimes are the next topics I'd look into in order to develop a better understanding of how Rust leverages the type system to make guarantees about your programs. Most of these ideas are applicable to other languages, like TypeScript, Java, and of course OCaml-like and Haskell-like languages.
