---
title: "What Is Rust's unsafe?"
date: 2019-07-12T10:00:00-07:00
categories:
    - Rust
    - Programming
tags: ["rust", "programming", "unsafe", "safety"]
description: An in-depth look at why Rust has the unsafe keyword, what it does, and how it's used to make the Rust ecosystem as a whole safer.
---

I've seen a lot of misconceptions around what the `unsafe` keyword means for the utility
and validity of Rust and its marketing as a "safe systems language". The truth is a lot
more complicated than a single pithy tweet can possibly sum up, unfortunately; here it is
as I see it.

Basically, **the unsafe keyword does not turn off the advanced type system
that keeps Rust code honest**. It only allows a few select "superpowers", like dereferencing
raw pointers. It is used to implement safe abstractions over a fundamentally unsafe world
so that the majority of Rust code can use those abstractions and avoid memory unsafety.

## The Promise of Safety

Rust promises safety as one of its core tenets; it is, in some ways, the _raison d'être_
of the language. It does not, however, go about providing that safety in the traditional
way, using a runtime and a garbage collector; rather, Rust uses a very advanced type
system to keep track of which values are safe to access when, and the compiler then
statically analyzes each Rust program to ensure that certain invariants are upheld.

### Safety in Python

Let's take, as an example, the Python programming language. Pure Python code cannot
corrupt its memory. List accesses have bounds checking, references returned by functions
are reference counted to prevent dangling pointers, and there's no way to perform
arbitrary pointer arithmetic.

This has two consequences: first, a lot of types have to be "special". For example, it's
not possible to implement an efficient Python list or dict in pure Python; instead, the
CPython interpreter implements lists and dicts internally. Second, access to external
(non-Python-managed) functions, called "foreign function interface", requires the use of
the special `ctypes` module and breaks the language's safety guarantees.

In a certain sense, this means that everything written in Python is memory unsafe.

### Safety in Rust

Rust also provides safety, but instead of implementing unsafe structures in C, it provides
a so-called "escape hatch": the `unsafe` keyword.
This means that the foundational data structures in Rust like `Vec`, `VecDeque`, `BTreeMap`,
and `String` are all implemented _using Rust_.

"But Nora," I hear you asking, "if Rust provides an escape hatch from its guarantees, and
the standard library is implemented using that escape hatch, isn't everything written in
Rust unsafe?"

In a word, dear reader, **yes** - in exactly the same way that everything in Python is.
Let's dig into that.

## What Is Prohibited in Safe Rust?

Safety, in Rust, is very well-defined; we think about it a lot. In essence, safe Rust
programs cannot:

- **Dereference a pointer that does not point to the type the compiler thinks it points to.**
This means no null pointers (because they point to _nothing_), no memory-out-of-bounds
and/or segmentation faults, and no buffer overflows, but it also means no use-after-free
or double-free (because freeing memory counts as dereferencing a pointer), and no [type
punning](https://en.wikipedia.org/wiki/Type_punning).
- **Cause there to be either multiple mutable references or both mutable and immutable
references to the same data at the same time.** This is, if you have a mutable reference
to some data, _only you_ have that reference, and if you have an immutable reference to
that data, _it will not change_ while you hold that reference. This means it is impossible
to cause a data race in safe Rust, which is a guarantee most other safe languages do not
provide.

Rust encodes this information in the type system, either through the use of **algebraic
data types** like `Option<T>` to encode presence/absence and `Result<T, E>` to encode
failure/success, or **references and lifetimes** like `&T` vs `&mut T` to encode the
difference between shared (immutable) and exclusive (mutable) references and `&'a T`
versus `&'b T` to denote references that are valid in different contexts/frames. (These
are usually elided; that is, the compiler is generally smart enough to figure them out.)

### Examples

For example, the following code does not compile because it would cause a dangling
reference; specifically, `my_struct does not live long enough`. In other words, the
function would return a reference to something that no longer exists, and therefore the
compiler won't (and really, doesn't know _how_ to) compile it.

```rust
fn dangling_reference(v: &u64) -> &MyStruct {
    // Create a new value of type MyStruct with the value field set to v,
    // the function's one parameter.
    let my_struct = MyStruct { value: v };
    // Return a reference to the local variable my_struct.
    return &my_struct;
    // my_struct is deallocated (popped off the stack, really).
}
```

This code does the same thing, but tries to get around the problem by placing the value
on the heap (`Box` is Rust's name for a basic smart pointer with no wacky behavior.)

```rust
fn dangling_heap_reference(v: &u64) -> &Box<MyStruct> {
    let my_struct = MyStruct { value: v };
    // Put the struct in a Box, allocating space for it on the heap and moving it there.
    let my_box = Box::new(my_struct);
    // Return a reference to the local variable my_box.
    return &my_box;
    // my_box is popped off the stack. It "owns" my_struct and thus is responsible
    // deallocating it, so the MyStruct is deallocated.
}
```

The correct code returns the `Box<MyStruct>` itself, rather than a reference to it. This
encodes the transfer of ownership - responsibility for deallocation - in the type
signature of the function. Just by looking at the signature, it's clear the the caller
is responsible for what happens to the `Box<MyStruct>`, and indeed, the compiler handles
this automatically.

```rust
fn no_dangling_reference(v: &u64) -> Box<MyStruct> {
    let my_struct = MyStruct { value: v };
    let my_box = Box::new(my_struct);
    // Return local variable my_box by value.
    return my_box;
    // Nothing is deallocated. The caller is now responsible for managing the heap memory
    // allocated in this function; it will almost certainly be deallocated automatically
    // when the `Box<MyStruct>` goes out of scope in the caller, barring a double-panic.
}
```

> Some bad things are not prohibited in safe Rust. For example, it is absolutely
permissible, from the point of view of the compiler, to:

> - deadlock a program
> - leak an arbitrarily large amount of memory
> - fail to close file handles, database connections, or missile silo covers

> A strength of the Rust ecosystem is that many projects take the approach of using
the type system to enforce correctness to heart, but the compiler does not require such
enforcement except for the case of memory safety.

## What is Permitted in Unsafe Rust?

Unsafe Rust is Rust code annotated with the `unsafe` keyword. `unsafe` can be applied to
a function or a block of code. When applied to a function, it means, "this function
requires that its caller manually uphold variants otherwise upheld by the compiler"; when
applied to a block of code, it means, "this block of code manually upholds variants required to
prevent causing memory unsafety, and should therefore be permitted to do `unsafe` things".

**In other words, on a function, `unsafe` means "you need to check", and on a block of
code, it means "I checked".**

As mentioned in [TRPL](https://doc.rust-lang.org/book/ch19-01-unsafe-rust.html), code in a block annotated with the `unsafe` keyword can:

- **Dereference a raw pointer**. This is the key "superpower" that lets us implement
doubly-linked lists, hashmaps, and other fundamental data structures.
- **Call an unsafe function or method**. More discussion on this below.
- **Access or modify a mutable static variable**. Static variables whose scope is not
controlled cannot be statically checked, so their use is inherently unsafe.
- **Implement an unsafe trait**. Unsafe traits are used to mark whether or not specific
types guarantee certain invariants; for instance, the `Send` and `Sync` traits determine
whether or not a type can be sent across a thread boundry or used in multiple threads
at once.

Those dangling pointer examples I gave above? Annotate the functions with `unsafe` and
you'll just get the compiler complaining twice as much, because it doesn't like the use
of `unsafe` on code that doesn't need it.

Instead, the `unsafe` keyword is used to implement safe abstractions over otherwise
arbitrary manipulations of pointers. For example, the `Vec` type is implemented using
`unsafe`, but it's safe to use, since it checks accesses and doesn't allow overflow, and
while it does provide operations like `set_len` which _could_ cause memory unsafety,
those operations are all marked `unsafe`.

For example, we could do the same thing as the `no_dangling_reference` example with the
gratuitous use of `unsafe`:

```rust
fn manual_heap_reference(v: u64) -> *mut MyStruct {
    let my_struct = MyStruct { value: v };
    let my_box = Box::new(my_struct);
    // Convert the Box into a regular old pointer.
    let struct_pointer = Box::into_raw(my_box);
    return struct_pointer;
    // Nothing is dereferenced; this functions just returns a pointer.
    // The MyStruct remains where it is on the heap.
}
```

Note the lack of unsafe; _creating_ raw pointers is totally safe. As written, this is a
memory leak risk, but nothing more, and leaking memory is safe. Calling the function
is safe too; its only when something wants to _dereference_ the pointer that `unsafe` is
needed. As an added benefit, it will deallocate the memory automatically.

```rust
fn main() {
    let my_pointer = manual_heap_reference(1337);
    let my_boxed_struct = unsafe { Box::from_raw(my_pointer) };
    // Prints "Value: 1337".
    println!("Value: {}", my_boxed_struct.value);
    // my_boxed_struct goes out of scope. It now owns the memory on the heap, so it
    // deallocates the MyStruct.
}
```

With optimizations, that code is exactly equivalent to just returning the `Box` in the
first place. `Box` is a safe abstraction over the use of pointers, because it prevents
spreading that raw pointer all over the place. For instance, the following version of
`main` **will** result in a double-free.

```rust
fn main() {
    let my_pointer = manual_heap_reference(1337);
    let my_boxed_struct_1 = unsafe { Box::from_raw(my_pointer) };
    // DOUBLE FREE BUG!
    let my_boxed_struct_2 = unsafe { Box::from_raw(my_pointer) };
    // Prints "Value: 1337", twice.
    println!("Value: {}", my_boxed_struct_1.value);
    println!("Value: {}", my_boxed_struct_2.value);
    // my_boxed_struct_2 goes out of scope. It owns the memory on the heap, so it
    // deallocates the MyStruct.
    // my_boxed_struct_1 then goes out of scope. It also owns the memory on the heap,
    // so it also deallocates the MyStruct. This is a double-free bug.
}
```

### Just What is a Safe Abstraction?

A safe abstraction is one that uses the type system to provide an API that cannot be used
to violate the safety guarantees I mentioned above. `Box` is safer than `*mut T` because
it _cannot_ be used to produce the double-free just illustrated.

Another great example of this is Rust's `Rc` type. It's a reference counted pointer - a
read-only reference to some data stored on the heap. Because it allows multiple
simultaneous access to a single memory location, it _must_ prevent mutation to be
considered safe.

In addition, it's marked as not thread-safe; if you want thread-safety, you have to use
the `Arc` (Atomic Reference Counting) type, which takes a performance hit from using
atomic values for its reference count to prevent the possibility of data races in multi-
threaded environments.

The compiler will stop you from using `Rc` where you should use `Arc`, because the people
who implemented `Rc` did not mark it as thread safe. If they had, that would be "unsound":
a false promise of safety.

### When is Unsafe Rust Necessary?

Unsafe Rust is needed whenever performing an operation that might cause one of those two
rules mentioned above to be violated. For example, in a doubly-linked list, not having
two mutable references to the same data (from the node before and after a given node)
defeats the whole purpose. With `unsafe`, the implementor of a doubly-linked list can
write that code using `*mut Node<T>` pointers and then encapsulate it in a safe
abstraction.

Another example is when working in the embedded space. Often, microcontrollers use a set
of registers whose values are determined by the actual physical state of that device.
The world won't just pause for you when you take an `&mut u8` of that register, so the
device support crates need `unsafe` in order to work with them - but they generally strive
to encapsulate that state in transparent safe wrappers that copy data when possible, or
use other techniques to uphold the guarantees of the compiler.

It's sometimes unavoidable to do an operation that might lead to simultaneous reading
and writing, or memory unsafety, and that's when `unsafe` is needed. But, so long as there
is a way to ensure that the Rust safety invariants are checked before the user of safe
(that is, non-`unsafe`-marked) code touches anything, that's completely fine.

## On Whose Shoulders Does It Fall?

So, we come back to the point I made above - **yes**, the Rust language's utility is built
on a foundation of `unsafe` code. Though it's done in a slightly different way than the
unsafe implementations of core data structures in Python, it's just as true that the
implemenation of `Vec`, `HashMap`, et cetera _must_ use raw pointer manipulations at some
level.

We say that safe Rust is safe with the fundamental assumption that `unsafe` code we use
through dependencies, whether on the standard library or other library code, has been
correctly written and encapsulated. The fundamental advance of Rust is that the code
is corralled into `unsafe` blocks, which are supposed to be verified by their authors.

In Python, the burden of ensuring that raw memory manipulation is safe falls only on the
interpreter maintainers and the users of the foreign function interface. In C, that burden
is on every single programmer. 

In Rust, the burden falls on the user of the `unsafe` keyword. It's clear, within the
code, where variants need to be manually upheld, and it is common to strive for zero
unsafe code anywhere in a library or application's codebase. The unsafety is identified,
segregated, and clearly marked, so if your Rust code segfaults, you have
discovered either a bug in the compiler or a bug in your few lines of `unsafe` code.

It's not a perfect system, but if what you need is the trifecta of speed, safety, and
concurrency, it's the only option out there.

