---
title: "Wake on Write: Typed Mutation and Syncronization"
date: 2020-03-01T12:31:12-06:00
categories:
- Rust
- Programming
description: Rust's type system tracks mutable and immutable access to data. We can take
    advantage of that to syncronize threads and asyncronous tasks.
---

For the last few months, I've been working on some heavily `async`-ified Rust code.[^async-why]
It's generally been pretty interesting, with some challenges normally found in syncronous
code simply disappearing and some entirely new and very annoying problems cropping up.
In one case, Rust's type system presented a solution that isn't available in most languages,
or at least couldn't be implemented nearly as elegantly.

[^async-why]: On the rewrite of [Qaul](https://qaul.net/), using [async-std](https://async.rs/) as our async core.

## The Promise

In this case, we had a `Worker` struct which contained some data and a method that would
do some asyncronous computations with that data. The workers placed their results in a
shared `Vec`, which was stored behind an `async`-ified `Mutex`.

We also had a `Manager`, another pretty simple struct that took work and
distributed it among the `Worker`s. As the original author wrote in the
comments, "workers do work, managers take credit." [^katharina]

[^katharina]: https://twitter.com/spacekookie

```rust
struct Manager {
    workers: Arc<Mutex<BTreeMap<usize, Arc<Worker>>>>,
    outbox: Arc<Mutex<VecDeque<usize>>>,
}
```

It had a method, `done()`, that returned a `Future` promising the next piece of finished
work from the system. This is the kind of quick and simple `Future` that seems pretty
easy to make using `poll_fn`. [^poll_fn]

`poll_fn` is one of the standard patterns in `async-std` land to create a new asyncronous task.
It essentially takes a regular syncronous function that returns either "finished" or "not
finished"[^poll] and turns that into a `Future`.

[^poll_fn]: https://docs.rs/async-std/1.5.0/async_std/future/fn.poll_fn.html
[^poll]: Actually, a sum type called [`Poll`](https://docs.rs/async-std/1.5.0/async_std/task/enum.Poll.html).

It's a useful way to avoid having to produce our own `impl Future` types for each
particular use case, and instead just rely on regular syncronous/fail-on-unready APIs.
They are, however, _syncronous_ functions inside the `poll_fn` call, so we can't use the
`.await` syntax on our `Mutex`.

One great feature of Rust's `Future`s is that they're very minimally magical, so even
though we can't `.await` the `lock()`ing of our `Mutex`,[^why_no_await_mutex] we can
simply pass on its readiness state in our own future. [^elision]

[^why_no_await_mutex]: While we could lock the `Mutex` outside of the `poll_fn`, that would mean the `Mutex` would stay locked until the `Future` completed, which isn't until all the `Worker`s are done, and the `Workers` need to lock the `Mutex`, so that's a deadlock.

[^elision]: Note, some complexity is elided here to make the larger point, so this code won't run as is. Sorry about that.

```rust
async fn done(&self) -> usize {
    future::poll_fn(|ctx| {
        let lock_future = &mut self.outbox.lock();
        match lock_future.poll() {
            Poll::Ready(ref mut vec) => {
                if vec.len() > 0 { match vec.pop_front()
                    Some(f) => Poll::Ready(f),
                    None => unreachable!(),
                } else {
                    Poll::Pending
                }
            },
            Poll::Pending => { Poll::Pending },
        }
    }).await
}
```

For those non-Rustaceans out there, that says, "to poll this future, try to lock the
outbox. If you can't, wait. If you can, and there's finished work in there, get it and
you're done. If there's no finished work, wait."

Unfortunately, any function like this is likely to fail in a pretty
unexpected way; if it doesn't succeed on the first try, it will
_never_ succeed at all!

So, what could be causing this issue?

## The Problem

Let's step back and talk about how `Futures` work in Rust. From the standard library's
documentation:

> Futures alone are inert; they must be actively polled to make progress, meaning that each time the current task is woken up, it should actively re-poll pending futures that it still has an interest in.

So, we could write a simple `Futures` executor by simply running a `loop`, polling each
`Future` in turn, then waiting a few milliseconds and polling them all again. This isn't
very efficient, though, and the documentation explicitly warns us against it.

> The poll function is not called repeatedly in a tight loop -- instead, it should only be called when the future indicates that it is ready to make progress (by calling `wake()`). 

Most executors (including `async-std`) implement this strategy. `Future`s are spawned in
a runnable state, and once run, they are put in a sleeping state until they are woken somehow.
If the future is created by a combination of other futures, it will automatically be
woken by the executor when those futures complete. For instance, a future which, halfway
through, `await`s opening a file, will be woken by the executor when that file is opened,
placed in a runnable state, and run at the next opportunity.

Futures like the one defined above, though, have nobody to automatically wake them; the
executor doesn't know about the state of the `Mutex` wrapping `Foo`.[^async_mutices] We
have to manually call a method called `wake()` on a `Waker` associated with the `Future`
to wake it up.

[^async_mutices]: `async_std` actually provides pre-`async`-ified `Mutex` and `RwLock` types to alleviate this issue, but for various reasons they're not applicable in all situations.

## Sketching a Solution

That `Waker` is provided to the `poll_fn` closure through its single argument: `Context`.
What we need to do is to store that `Waker` somewhere and call it when it's time to wake
up the `Future`, and there are a number of ways we could do this.

### Put a Waker inside the Manager

We could easily add an `Option<Waker>` field to the `Manager` and populate it the first
time the manager's `Future` is polled:

```rust
async fn done(&mut self) -> Vec<Foo> {
    future::poll_fn(|ctx| {
        if self.waker.is_none() {
            self.waker = Some(ctx.waker().clone());
        }
        //...
    });
}
```

However, the `Worker`s would have to access it, so it would need to reside within an
`Arc` which they were given access to, which means another `Mutex` and more complexity.

### Put a Waker inside the data structure

To address that issue, we could put the `Waker` somewhere the `Workers` are already
locking: the `Manager`'s outbox! Instead of having a 

