---
title: "Simple Elixir Functions"
date: 2019-07-20T12:03:23-07:00
tags: ["elixir", "programming", "frivolous"]
description: A few simple Elixir functions I built and my thoughts on them.
---

I've been playing around with [Elixir](https://elixir-lang.org/), which is a pretty cool
language - specifically, I'm reading [Programming Elixir 1.6](https://pragprog.com/book/elixir16/programming-elixir-1-6)
which is **free** for anyone with a `.edu` email address from The Pragmatic Bookshelf.

## reverse/1

I enjoy functional programming, so of course the first two functions I made were list
operations. First up is `reverse(list)` (written as `reverse/1`, meaning it takes one
argument), which reverses a list.

```elixir
# reverse/1
#   Reverse the given list.
def reverse([]), do: []
def reverse(list) do
    reverse(tl list) ++ [hd list]
end
```

The first `def` line here says that if you pass `reverse/1` an empty list, the result
is an empty list; that's the base case.

The second `def` defines the recursive case, in which `reverse/1` takes the tail of the
list (`tl list`), reverses it, and appends the first element (`hd list`).

## palindrome/1

Another classic FP function is `palindrome(list)`, evaluating to true if the given list
is a palindrome (like `[1, 2, 3, 2, 1]`).

This function takes advantage of the `_` special variable to say that any list with
exactly one element is automatically a palindrome, as a second base case.

It also uses the `|>` pipe operator to turn the pruning operation, which turns 
`[1, 2, 3, 2, 1]` into `[2, 3, 2]` for recursion, from a nested function call
`List.delete_at(tl(list), -1)` into the somewhat more readable `tl(list) |>
List.delete_at(-1)`.

I also went out of my way to use `reverse/1` here, just to say that I did. It's definitely
not the most efficient way to implement this algorithm.

```elixir
# palindrome/1
#   Return true if the given list is palindromic
#   or false otherwise.
def palindrome([]), do: true
def palindrome([_]), do: true
def palindrome(list) do
    (tl(list) == tl(reverse list)) and palindrome(tl(list) |> List.delete_at(-1))
end
```

There's not much point to this post other than to say - Elixir is fun, and a pretty
language!

