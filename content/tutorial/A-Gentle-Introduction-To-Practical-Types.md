---
date: 2017-09-29
title: A Gentle Introduction to Practical Types
slug: a-gentle-introduction-to-practical-types
categories:
- Programming
- Rust
- Math
description: Some programmers talk a lot about types, but what is a "type", anyway? This tutorial explores the definition and utility of a ML-like type system.
---

Programmers talk a lot about types, but what is a "type", anyway? It is, in essence, _the set of all possible values_ for some variable. Defining such a set gives us some information about _what we can do_ with the value of that variable, in general.

For example, when speaking about numbers, we might say, "let x be any integer" or "let y be any real number not equal to zero". These statements tell us what we can do with these values; we know that we can add, subtract, or multiply x and y. We cannot necessarily divide by x (because x might be equal to zero), though we can divide by y. 

In programming, most languages have many built in types, and not just numbers; strings (of characters; that is, text) and more exotic things like pointers are all types of values that can be found in most programs, and the types of those values essentially tell us what we can do with them.

Types also tell us _how to represent data_. For instance, a variable of type `int` is not "any integer"; it is any integer between -2^31 and 2^31 - 1, because it is represented in memory as 32 bits, the first of which is the sign (negative or positive) and the rest of which are the absolute value of the number.

But before we can really talk about integers, decimal numbers, or text, we need to understand a little more about types themselves.

## The Naming of Things
The first type I'm going to make is something that mathematicians call a Σ type or sum type. What it really is is a list of options; an _enumeration_ of possible values, or, in many programming languages, an `enum`. Each option has its own type, and the enumeration of these options creates a new type. 

Here's a type that may be familiar to you: a Boolean value. It can be either True, or False. Nothing else. Here's how we would write that down in a programming language (Rust, in this case):

<pre><code class="rs">
enum Boolean {
    True,
    False
}
</code></pre>

This is saying, "Create a new enumeration type. It's called the Boolean type, and every value of this type must be one of the following: True, or False."

We can figure out a few things about this. For instance, we can check if two values of type Boolean are equal or not; we can also define the Boolean functions like `and`, `or`, and `xor`, if we wish.

We need not be restricted to two options, however. For instance, here's a type that you might use when giving someone directions:

<pre><code class="rs">
enum StreetDirection {
    Left,
    Right,
    Straight,
}
</code></pre>

We can use these two types to apply some type theory to a conversation. Let's say Alice is giving Bob directions. Bob might ask, "Which way do I go at Laurel Street?" He is requesting a value of type StreetDirection; it would make no sense if Alice were to say "Blue" or "False".

We can extend this construct to create very useful types like an unsigned integer (the always-positive variant of the bounded `int` discussed above). It can hold values from 0 to 2^64 - 1. Were we to define it using this syntax, it would look something like this:

<pre><code class="rs">
enum PositiveBoundedInteger {
    0,
    1,
    2,
    3,
    (skip a few)
    18446744073709551615,
    18446744073709551616
}
</code></pre>

Of course, ([almost](https://en.wikipedia.org/wiki/Roman_Opa%C5%82ka)) nobody would ever actually write out all the integers from 0 to 18446744073709551616, but we _could_ define the type that way. We could also begin to write functions like addition, subtraction, and multiplication.

## Doing More with Enumerations

Consider the power of these sum or enumeration types; we can write the type of anything that is only one of a number of options, whether those options are ordered (like the PositiveBoundedInteger type above) or unordered (like Boolean values and directions). 

These types are called sum types because the number of possible values is the _sum_ of the number of options; for now, that's just 1 each - there are 2 possible values of type Boolean, for instance - but that won't always be true. We can define types that are the combinations of these types. 

For example, we could define a type that is the combination of itself and a StreetDirection. We could imagine a controller giving a robot directions on how to move around with this type:

<pre><code class="rs">
enum MovementInstruction {
    Go(StreetDirection),
    NoGo,
}
</code></pre>

The MovementInstruction type has 2 options, but one of those options has a StreetDirection in it! Therefore, we could write little sets of instructions for our hypothetical robot to solve a maze:

<pre><code class="rs">
Go(Straight)
Go(Right)
Go(Straight)
NoGo
</code></pre>

It's pretty easy to figure out how many options there are: Go(Straight), Go(Right), Go(Left), and NoGo. That is, the sum of the first option's possible values (3) and the second option's possible values (1). Thus, the number of possibilities for values is the _sum_ of the number of possiblities for values of the types of the enumerated options. Enumeration types are sum types.

## Getting Productive

Here's a challenge for you, as a sort of perverse reward for making it through sum/enum types. How would you use an `eunm` type to represent values that record how many cats and dogs someone has?


I'd start like this:

<pre><code class="rs">
enum Pets{
    NoCatsNoDogs,
    NoCatsSomeDogs(BoundedPositiveInteger),
    SomeCatsNoDogs(BoundedPositiveInteger),
}
</code></pre>

This is a good start. We can talk about people with no pets, or only one type of of pet. We do have a problem, though; what about people who have dogs _and_ cats? This is where product types or `struct`ured types come in. We can write something like this:

<pre><code class="rs">
struct Pets {
    dogs: BoundedPositiveInteger,
    cats: BoundedPositiveInteger,
}
</code></pre>

This definition means, "Create a new structured type. Its name is Pets, and it consists of one value of type BoundedPositiveInteger, called dogs, followed by another value of type BoundedPositiveInteger, called cats." We call it a structured type because we define it not by a list of possible values but by the structure of any possible value; in this case, two arbitrary BoundedPositiveIntegers. 

We could denote the state of owning no pets as:

<pre><code class="rs">
Pets {
    dogs: 0,
    cats: 0,
}
</code></pre>

That is, no dogs and no cats. A person with, say, 4 dogs and 3 cats would be:

<pre><code class="rs">
Pets {
    dogs: 4,
    cats: 3,
}
</code></pre>

Consider the number of possible values of the Pets type. There are 2^64 possible values of BoundedPositiveInteger, and here there are two BoundedPositiveIntegers, neither dependent on the other. Simple combinatorics tells us that there are 2^64 * 2^64 possible permutations; the _product_ of the number of values of the two consituent types. 

## Combining the Combinations

Product and sum types can be combined to create any type. For instance, we could create a signed integer type, based on our existing BoundedPositiveInteger:

<pre><code class="rs">
struct BoundedInteger {
    isNegative: Boolean,
    absoluteValue: BoundedPositiveInteger,
}
</code></pre>

This would have as many values as BoundedPositiveInteger (2^64) times as many values as Boolean (2); in other words, 2^65 values.

## More Exotic Types

So far, we've talked about types with sets of values with cardinality between 2 (Boolean) and 2^65 (BoundedInteger). Most _useful_ types will have sizes like this; however, there are uses for types with both infinite cardinality and only one possible value.

### Infinitely Large Types

We can use the combination of sum types and product types in another way: to define lists. Let's say we wanted a list of numbers; perhaps winning lottery numbers. For this, we need two types:

<pre><code class="rs">
struct IntegerListNode {
    value: BoundedInteger,
    link: IntegerListLink,
}

enum IntegerListLink {
    HasNextNode(IntegerListNode),
    NoNextNode,
}
</code></pre>

Each node holds a link which points to either another node (the next in the list) or to nothing (signifying the end of the list). We could construct a list and then look at each of its elements in sequence. Writing such a list out is a bit daunting; The list 1, 2, 3 would read, in full:

<pre><code class="rs">
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
</code></pre>

We do run into a bit of a problem, though, when looking at the size of this type. Clearly, the IntegerListNode type has as many possible values as a BoundedInteger times as many possible values as an IntegerListLink. We know that the BoundedInteger type has 2^65 values, but how many does IntegerListLink have? Well, it's the sum of 1 (for NoNextNode) plus... however many possible values IntegerListNode has! This list type is a type with infinitely many possible values, because the nesting of nodes and links can go on for arbitrarily long.

> **Sidebar: Representing Infinite Types in Real Computers**

> The property just described actually causes the Rust compiler (which sadly must exist in the real world) to reject this type as invalid, because it cannot generate a representation that would fit into a computer's memory. Because each node contains all subsequent nodes, and because one of the things that makes Rust programs fast is that values are, by default, allocated all at once, the compiler would have to ask the computer to allocate the _maximum number of nodes that could ever exist_. Since we didn't introduce any limit to the number of nodes, that would require an infinite amount of memory. This is where _indirection_ via pointers comes in; see the Rust Book[^rustbook] for more info. 
>
> Any programming language that puts values on the stack by default will have this problem; for instance, C and Rust will require explicit indirection, while e.g. Python and Java do it for you, at a bit of a speed cost.
> 
> The correct types look like this. Note the added `Box<...>` in the first variant of IntegerListLink; if you are unfamiliar with Rust, it is sufficient to know that this annotation tells the compiler to allocate some memory for the next IntegerListNode and store only the address of that memory inside the IntegerListLink.
> <pre><code class="rs">
struct IntegerListNode {
    value: BoundedInteger,
    link: IntegerListLink,
}
enum IntegerListLink {
    HasNextNode(Box&lt;IntegerListNode&gt;),
    NoNextNode,
}
></code></pre>
> It's important to remember that while the current Rust _implementation_ doesn't like this type, there's no reason that Rust as a _language_ can't handle it. It's a very useful type, and a good logical description of how linked lists work, even though we have to add some incantations to get them to work in the real world. 

### Zero Sized or Single Value Types

One thing that's interesting to note is that we have a bit of a discrepency between two kinds of enumeration type options. Some of them have other types associated with them, and some don't. Recall our type MovementInstruction:

<pre><code class="rs">
enum MovementInstruction {
    Go(StreetDirection),
    NoGo,
}
</code></pre>

This is actually a simplification of the "real" types here. NoGo technically "contains" a value of what we call the "unit type", which we denote () or ○. Values of this type can only have one value; the unit value, also denoted () or ○. So, we can re-write this type:

<pre><code class="rs">
enum MovementInstruction {
    Go(StreetDirection),
    NoGo(()),
}
</code></pre>

We can see that this has a size of 0: there is only one possible value, so whenever a value with this type appears, we know what the value is. We also can't do anything at all with the unit value, meaning that we can't do anything with variables of the unit type.

However, the unit type _can_ be useful for conveying information to the compiler, or to help the compiler figure out when the programmer has made a mistake. For instance, in Rust, we say that things which return nothing (for instance, `if something { end_the_world(); }`) actually return the unit value. This gives us errors like this:

<pre><code class="rs">
error[E0308]: mismatched types
 --> test.rs:2:24
  |
2 |   fn something() -> bool {
  |  ________________________^
3 | |     println!("This doesn't return anything!");
4 | | }
  | |_^ expected bool, found ()
  |
  = note: expected type `bool`
             found type `()`
</code></pre>

## Recap & Further Reading

It's been a long journey. We've gone from nothing, through finite lists of possible values (enumerations/sum types), to combinations of those lists (structured types/product types) and, finally, types with only one possible value and those with infinitely many possible values. Go forth and apply this knowledge!

To help you in actually understanding how to use the type system of Rust specifically, I suggest the excellent Rust Book[^rustbook] and Sean Griffin's talk on the type system[^seangriffin] from RustConf 2017. Traits and lifetimes are the next topics I'd look into in order to develop a better understanding of how Rust leverages the type system to make guarantees about your programs. Most of these ideas are applicable to other languages, like TypeScript, Java, and of course OCaml-like and Haskell-like languages.

NOTE: The examples here will not compile on their own; I have elided `use` statements and other such language-specific constructs to make a point. Refer to the Rust Book for information on how to actually use the types.

[^rustbook]: [The Rust Programming Language](https://doc.rust-lang.org/stable/book/), 2nd Edition, by S. Klabnik and C. Nichols. 2018. O'Reilly.
[^seangriffin]: [Type System Tips for the Real World](https://www.youtube.com/watch?v=wxPehGkoNOw), by Sean Griffin at RustConf 2017.
