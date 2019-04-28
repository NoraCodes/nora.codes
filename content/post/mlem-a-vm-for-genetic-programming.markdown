---
date: 2017-04-17 04:48:50+00:00
title: MLeM, a VM for genetic programming
slug: mlem-a-vm-for-genetic-programming
categories:
- Programming
- Machine Learning
description: A summary of a virtual machine I designed as a genetic programming target.
---

I've recently been working on a project called the Machine Learning Machine, or MLeM.
It's a VM implemented in the Rust programming language which I hope to use as a basis for some genetic programming.

It's a Harvard architecture machine, meaning that it has separate representations and memory for data and program segments. While this is not the way most modern computers work, it does model the more secure W XOR X functionality that exists in operating systems such as BSD and allows me to properly utilize the amazing type system of the Rust language to do compile time verification of a lot of the system.

The machine has 8 general purpose registers, a “hardware” stack (that is, it has Stack Pointer and Base Pointer registers and PUSH and POP instructions), and built in I/O instructions. This makes is slightly abstracted over a real machine, but still makes any code written for it relatively easy to turn into actual machine code for real CPUs.

All data is stored in 64-bit registers and memory cells. Instructions, however, are represented using Rust’s enums. I chose to go with a unified-addressing scheme. This glosses over the realities of memory access, but because all accesses are already tagged by virtue of the system itself, I can easily add performance penalties for main memory access and even a cache simulation when needed.

A sample instruction looks like this:

`Add(a, b)`

where a and b are of type `Address`. So

`Add(RegAbs(R1), MemReg(R2))`

would add the value in R1 to the value at the memory address in R2 and place the result in R1. Note also that R1 and R2 are enum variants of `Register`; trying to
create something like

`Add(RegAbs(R1), MemReg(100))`

would produce a compiler error. 

I’ve also developed an extremely simple assembly syntax for testing purposes; the above instruction would look like:

`add r:r1 p:r2`

That is, add (register) r1 to (pointer) r2.


The primary goal for this project is to provide a more stable and closer-to-real-CPU language than Muller’s Brainf\*\*\* or my own [SBrain](https://github.com/noracodes/sbrain), both of which I’ve [previously used](https://github.com/noracodes/evolve_bf) for genetic programming with rather mixed results.

The ability to generate instructions I know are valid helps short-circuit the process of culling unparseable genes from the population, which, while not particularly difficult, slows the process of evolution significantly.

Look out for a future post to discuss my previous experiences with GP and the future of this project.
