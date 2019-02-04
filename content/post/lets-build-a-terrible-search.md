---
title: "Let's Build a Terrible Search"
date: 2019-02-03T21:09:52-06:00
categories:
- Python
- Programming
- Algorithms
description: "In which I build a very slow searching algorithm and compare it to the
built-in Python searching algorithm."
---

Searching: the second-most studied problem in computer science, next to sorting (or so
it would seem from my CS curriculum). There are pretty good methods for doing it, too;
for example, everyone's favorite, binary search, can find an item in a list of 260,000
possibilities after looking at just 18 items (the base-2 logarithms of the size of the
array).

Even the most obvious search, linear search, isn't terrible, especially for problem
sizes which can fit in memory close to the CPU, like low-level cache. But, fast
programs are boring. I want a _bad_ search! So, let's get there.

## Linear Search

Let's look at the Python ("executable pseudocode") for a basic linear search:

```python
def pythonseek(array, x):
    return array.index(x)
```

This is great, except that if we call it with an `x` that doesn't exist in the list,
we get an exception. Let's change that:

```python
def pythonseek(array, x):
    try:
        return array.index(x)
    except ValueError as e:
        return
```

So, that's easy. What if we want to see the steps, though? It's pretty easy to build a
simple linear search in Python. We iterate over each element in the array, just checking
if that element is the one we're looking for. If so, we return the index.

```python
def linearseek(array, x):
    for (index, element) in enumerate(array):
        if element == x:
            return index
```

## Random Search

This feels a little too... systematic, though. What if we didn't look through in any
particular order? Maybe we can just, you know, cast about at random.

```python
import random
def randomseek(array, x):
    index = random.randint(0, len(array) - 1)
    while array[index] != x:
        index = random.randint(0, len(array) - 1)
    return index
```

This does have a problem. If the element isn't in the array, it will never
terminate! So, we need to keep track of whether or not we've searched the whole array.

It's pretty easy to add in a hash table to do this. Every time we look at an index,
we add it to the hash table, and we keep searching until the hash table is the same size
as the input array.

```python
import random
def randomseek(array, x):
    already_looked = {}

    while len(already_looked) < len(array):
        i = random.randint(0, len(array) - 1)
        if array[i] = x:
            return i
        else:
            already_looked[i] = True
```

Beautiful! Now, let's figure out exactly how bad this is. Python has first-class
functions, which is a boon for comparing algorithms. We can create a simple function to
run a bunch of searches and time them.

## Benchmarking

In essence, this function just takes a function of the form of the ones above, and runs
it a thousand times, averaging the time per element and printing it.

```python
def test_seek(f, name):
    total_elems = 0
    total_time = 0
    size = 5000
    arr = [random.randint(0, size) for index in range(0, size)]
    for in in range(1, 1000):
        x = random.randint(0, size)
        t1 = time.time
        f(arr, x)
        t2 = time.time
        nanoseconds = (t2 - t1) * 1000000000
        total_elems += n
        total_time += nanoseconds
    print(name + ": On average " + str(total_time/total_elems) + "ns per element")
```

I also threw in "Sort and Seek", which runs `.sort()` ([Timsort](https://www.geeksforgeeks.org/timsort/)) on the
array and then binary searches it. And the results are in:

- Python `.index` linear search: ~130 ns/element
- Custom linear search: ~130 ns/element
- Sort and Seek: ~82 ns/element
- Randomseek: ~131,831 ns/element

(These results are from my ThinkPad T430s with a Core i5 3320M and 16GB of memory.)

So, yes, randomseek is a very bad search. In fact, it's almost exactly a thousand times
worse than linear search! But it's still not _that_ bad.

## Conclusion

130,000 nanoseconds is not a long time. According to Wolfram Alpha, it's about
a fifth as long as a baseball remains in contact with the bat during a hit, or
half the period of the highest tone audible to most humans. We could search
through an array of 7,585 items in one second. That's a lot.

On the other hand, 130,000 nanoseconds _is_ a long time. Light can travel 25 kilometers
in that amount of time. It's 82 times the half-life of radium; in the time our search has
finished one item, the amount of radium in a sample will have diminished to basically
nothing. It would take almost 12 hours to search a list regarding the whole population of
the U.S., which our other searches can do in under a minute.

So, the moral of this story? As Julia Evans tells us, [computers are fast](https://computers-are-fast.github.io/).
But they are not fast enough to hide our sins. Randomseek is a silly example, but we do
lots of things that waste time. Things like [reaching out to the network before launching
a program that doesn't need the internet](https://en.wikipedia.org/wiki/Always-on_DRM),
or [launching an entire virtual machine and generic layout and rendering engine to power
a chat application](https://slack.com/). Don't write randomseek.
