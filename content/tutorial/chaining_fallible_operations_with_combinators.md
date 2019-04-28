---
title: "Chaining Fallible Operations with Combinators"
date: 2018-06-26T12:54:53-05:00
categories:
- Programming
- Rust
description: A demonstration of some techniques for chaining fallible operations, such as parsing or network requests, using Rust's iterator combinators, without introducing overhead.
---

Rust's [Iterator](https://doc.rust-lang.org/std/iter/trait.Iterator.html) trait is one of its most useful features. It allows lazy processing of item-by-item streams of anything from [the bytes of a file](https://doc.rust-lang.org/std/io/trait.Read.html#method.bytes) to [threads](https://doc.rust-lang.org/beta/rust-by-example/std_misc/threads.html) to complex and exotic data structures.

Most of the useful functionality, though, is provided by combinators, functions that allow us to combine iterators and process them in useful ways. These include `map`, `fold`, `filter`, and many other useful functions (including those from the excellent [itertools](https://docs.rs/itertools) crate).

## Concision and Clarity

Consider that we have the following functions which we'd like to apply to some list of numbers:

<pre><code class="rust">
fn operation_one(a: i32) -> i32 { a + 1 }
fn operation_two(a: i32, b: i32) -> i32 { a + b }
fn check(a: i32) -> bool { a > 3 }
</code></pre>

That is, we want to apply `operation_one` to every item, apply `operation_two` to every pair of those results, and finally return only the results which satisfy the condition in `check`.

Procedurally, that looks a bit like this:

<pre><code class="rust">
fn procedural(values: &[i32]) -> Vec<i32> {
    let mut temp = Vec::with_capacity(values.len());
    for value in values {
        temp.push(operation_one(*value));
    }
    
    let mut temp2 = Vec::with_capacity(values.len());
    for index in 1..temp.len() {
        temp2.push(operation_two(temp[index - 1], temp[index]));
    }
    
    let mut temp3 = Vec::with_capacity(values.len());
    for value in temp2 {
        if check(value) {
            temp3.push(value)
        }
    }
    
    return temp3;
}
</code></pre>

A function with that signature must allocate at least once (to have a `Vec` to return), but this function has _three_ allocations, three mutable bindings, and is 19 lines long! Surely we can do better with functional programming:

<pre><code class="rust">
extern crate itertools; // 0.7.8
use itertools::Itertools;
fn functional(values: &[i32]) -> Vec<i32> {
    values.iter()
        .map(|x| operation_one(*x)).tuple_windows()
        .map(|values: (i32, i32)| operation_two(values.0, values.1))
        .filter(|x| check(*x)).collect()
}
</code></pre>

Even including the imports, this is 11 lines shorter and, in my opinion, far more readable. It also has only a single allocation (in the `collect()` call) and no mutable bindings.

So, all is rosy and fun with functional programming, right? Not quite. Enter fallibility.

## Fallible Maps

I work with microservices a lot, and one of the big problems with microservices is the introduction of system boundaries where all type information is lost.

For instance, many architectures involve passing around JSON, and therefore every ingestion routine has to consider the possibility that it was called with nonsensical input; JSON that parses is not necessarily JSON that makes sense.

Often, the desired behavior is to simply ignore the problematic input. This is easy with [`flat_map`](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.flat_map): 

<pre><code class="rust">
// Given that parse() return a Result&lt;Item, whatever&gt;
let parsed: Vec&lt;Item&gt; = inputs.iter()
    .flat_map(|input| parse(input).into_iter())
    .collect();
</code></pre>

This results in throwing away the errors, which may not be what we want. We could `filter` with a `match` expression, or do other iterator magic, if we simply wanted to log the errors, but what if we want to abort on error?

Fortunately, `Result<_, _>` [implements](https://doc.rust-lang.org/std/iter/trait.FromIterator.html#implementors) a trait called `FromIterator`, which allows the `collect()` iterator method to collect into a result, inverting the containment relationship:

<pre><code class="rust">
let items_or_errors: Vec&lt;Result&lt;Item, Error&gt;&gt;;
let single_error_or_items: Result&lt;Vec&lt;Item&gt;, Error&gt; =
    items_or_errors.into_iter().collect();
</code></pre>

This is really cool! In cooperation with [the `?` operator](https://doc.rust-lang.org/book/second-edition/ch09-02-recoverable-errors-with-result.html#propagating-errors), this can be used to easily, efficiently and concisely abort a function as soon as an error value is encountered - as long as you only have to do it once.

## Chaining Fallible Operations

In a real-world scenario, there are often a number of fallible operations that have to be performed. For example, the microservice that inspired this post had to:

1. Ingest untrusted input into a list of unnormalized input values, possibly rejecting the input as unparsable
2. Normalize the input values, possibly rejecting them as unnormalizable
3. Perform a computation on the values, possibly rejecting them as uncomputable
4. Aggregate the results into a single resulting value

Because of the nature of the service, the correct behavior was to abort as soon as a nonsensical, unnormalizable, or unprocessable value was encountered.

<pre><code class="rust">
struct DataUnnormalized { data: i32 }
struct Data { data: i32 }

fn ingest(input: String) -&gt; Result&lt;Vec&lt;DataUnnormalized&gt;, String&gt; {
    // In reality, this can also fail
    Ok( vec![DataUnnormalized { data: 1 }] )
}

impl DataUnnormalized {
    fn normalize(self) -&gt; Result&lt;Data, String&gt; {
        // In reality, this can fail
        Ok(Data {data: self.data})
    }
}

impl Data {
    fn perform_operation(&self) -&gt; Result&lt;i32, String&gt; {
        // In reality, this can fail too
        Ok(self.data)
    }
}

fn aggregate(agg: i32, curr: i32) -> i32 {
    agg + curr
}
</code></pre>

Unfortunately, composing these functions wasn't as simple as I expected. I eventually came up with the following solution:

<pre><code class="rust">
fn compute(input: String) -> Result&lt;i32, String&gt; {
    Ok(ingest(input)? // Ingest and abort if error. Simple enough
        .into_iter()
        .map(|i| i.normalize()) // Maps into Vec&lt;Result&lt;Data, String&gt;&gt;
        .collect::&lt;Result&lt;Vec&lt;Data&gt;, String&gt;&gt;()? // Flatten and abort if error
        .into_iter()
        .map(|i| i.perform_operation()) // Maps into Vec&lt;Result&lt;i32, String&gt;&gt;
        .collect::&lt;Result&lt;Vec&lt;i32&gt;, String&gt;&gt;()? // Flatten and abort if error
        .into_iter()
        .fold(0, |agg, curr| aggregate(agg, curr?))) // Aggregate
} </code></pre> 

This is... not good. It has two allocations which are effectively unnecessary, and are only there to convert between types. It has lots of syntactic noise, and is difficult to read.

After asking around, I found out that I was thinking about this wrong. Rather than mapping from success to success/failure, I should have started out by embracing the fallibility.

Making each `map` operation take a `Result` type makes this much prettier, and with the newly stablized [`try_fold`](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.try_fold), it can be done in only a few lines. This is because the `?` operator is allowed to operate correctly in these contexts.

<pre><code class="rust">
fn compute(input: String) -> Result<i32, String> {
    ingest(input)? // Ingest and abort if error. Simple enough
        .into_iter()
        .map(|i| i.normalize()) // Maps into Result<Data, String>
        .map(|i| i?.perform_operation()) // Maps into Result<i32, String>
        .try_fold(0, |agg, curr| Ok(aggregate(agg, curr?))) // Maximize
}
</code></pre>

## Lessons

Thanks for reading, and a huge thanks to [CUViper](https://www.reddit.com/r/rust/comments/8ts8xn/how_can_i_map_across_multiple_fallible_operations/e19v5w0/?st=jiw27tg4&sh=a6a16d53) for helping me come up with this solution to my problem. The takeaways from this process for me have been threefold:

1. Functional combinators in Rust are very powerful. They fit well in any problem that involves processing many homogeneous data points.
2. Design around fallibility. Pretending that fallible operations can't fail is a recipe for convoluted, type-weird code.
3. When a problem can't be solved at one level, step back and look a the level above it. I spent hours trying to work out the types for the `.collect::<>()` call when I should have reexamined the whole pipeline.

If you enjoyed this tutorial, you might like to read [more about Rust types](/tutorial/a-gentle-introduction-to-practical-types/), or perhaps learn how [session types](/tutorial/session-types/) can make your code better. Also, please consider supporting me on Patreon.

