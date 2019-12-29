---
date: 2017-05-02 00:00:00+00:00
title: SBrain, an extension of BrainF.ck
slug: sbrain-an-extension-of-brainfzck
categories:
- Programming
- Rust
- Machine Learning
description: A summary of some adventures in esolang design and genetic programming.
---

**SBrain**, or *Semantic Brain*, is a language based on Urban Müller's famous language **Brainf*ck** with only 8 symbols (3 bit instructions). SBrain's additions increase the number of symbols to 32 (6 bit instructions), including bit-shifting and arithmetic, and add a stack and a register. 

Having these additional facilities allows SBrain to be far more expressive while retaining its attractiveness as a genetic medium for evolutionary algorithms. I've been interested in genetic programming for a long time. After my less-than-stellar results from a Python implementation of a Brainf\*ck (which you can see [here](https://github.com/noracodes/evolve_bf), though the code is truly awful), I decided to try a new tack. I designed SBrain over about two years and implemented an interpreter in Rust, which you can find [here](https://github.com/noracodes/sbrain).

SBrain is a strict superset of its parent language. It does have some important differences other than its additional instructions, most notably that unbalanced jump
(`[` and `]`) instructions are valid. For instance, the program `.+]` will print ascending numbers forever (as opposed to the strictly legal `[.+]` required in BF.)

## Evolving Computer Programs

  Once I finished an initial implementation, I decided to try and get evolution working. I ended up with a program whose output looks a bit like this:

<pre><code>

Generation     1 Cost  3075: aS|.p!.S
Generation     5 Cost  3074: aS|.^!p!.S
Generation     6 Cost  3073: aS|.^!ap!S.S
Generation     7 Cost  2052: aS|.^!ap-.S.S
Generation     9 Cost  1028: a&|.^!ap..S.S
Generation    11 Cost     7: a&|.^!ap..S..S
Generation    16 Cost     5: a&|.^-!ap..S..S
Generation    29 Cost     3: a&m|.^-!ap..+..S
Generation    34 Cost     2: a&&m|.^-!ap..+.d.S
Generation   285 Cost     1: a&&m|+.^-!p..+.d.S
Generation  1101 Cost     0: a&&m|+.^!p.+.+.d.S
Program found after 1101 tries.
a&&m|+.^!p.+.+.d.S
Ran for 128 cycles and did not halt
[] -> a&&m|+.^!p.+.+.d.S -> [1, 2, 3, 4, 5]

</code></pre>

  This particular run was looking for the output [1,2,3,4,5] given no input.
  Built with `--release`, it took about 3 seconds (2.93, to be precise). The algorithm is fully parallelized using [Rayon](https://crates.io/crates/rayon) 
  and keeps all four of my laptop's CPU cores at around 80 to 100 percent most of the time. The program it found 
  is not particularly efficient (the optimal program being `+.+.+.+.+.`), but it's not extremely inefficient either.

## How it works
  The actual algorithm is pretty simple. It consists of a few basic steps:

  1. Generate an inital, entirely random population of programs.
  2. "Cost" the population (that is, execute them and see how far off they are from the desired outputs)
  3. Remove the bottom 50% of programs and replace them with variants of the top program, crossed with the programs in the top 50%.
  4. Repeat until at there is a program with the cost of 0

  The program reports its state only when there is an improvement; as you can see above, improvements come thick and fast in the beginning and slow down a great deal later.

### Generation
  The generation step is fairly simple. A set of valid source code symbols is hard-coded into the program, and the initally generated population is a set of purely random strings of those characters.

### Costing
  Costing is the most important part of the process, but is really quite simple in this case. The program is run in the SBrain VM and provided with the correct input. Its output is then analyzed against the expected output. There are two steps of this analysis. If the lengths of the actual vs. expected outputs don't match, the
  program is heavily penalized (2 to the 10th points for each missing or extra character). Then, the characters of each input are compared and their difference 
  is added to the cost.

### Mutation
  Mutation actually occurs at two levels: the population level and the program level. The population is sorted by cost, then essentially cut in half. The top half,
  the most successful programs, are mutated, and their mutated genomes are crossed with a mutated variation of the top program. 

  This crossing is immensely important to the rapid generation of workable algorithms. It allows two relatively sucessful genomes to share their best features, which
  further allows "breakthrough" steps in evolution. I initally implemented the algorithm without crossing, and it was incredibly slow at finding even very simple 
  programs.

  At the program level, mutation is merely a random choice between three alternative: adding a source character, changing a source character, and deleting a source character.

## Conclusions

  This algorithm has proven to be pretty effective for trivial problems like addition and generating ranges of values. There is currently a great deal of copying present in the code, but I hope to spend some time in the next few weeks (once finals are over, that is) making a zero-copy or almost-zero-copy implementation. 

  If you have ideas for how to make my code or algorithm more effective or faster, please oh please email me or send in a PR! The code is [on Github](https://github.com/noracodes/evolve-sbrain).
