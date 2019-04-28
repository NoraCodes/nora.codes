---
date: 2016-07-11 15:33:38+00:00
slug: porting-deucalion-to-rust
title: Porting Deucalion to Rust
categories:
- Programming
- Rust
description: A quick note on my experience with Rust for game programming.
---

A few months ago, I made a proof-of-concept for an RPG engine based on SFML and the Tiled map editor, called Deucalion. Over time, the source code became unwieldy, leaked a great deal of memory, and was nearly impossible to build. I ended up spending more time configuring build systems than actually working on the code, and I abandoned it in favor of SBrain and schoolwork.

Recently, though, the Rust game development story has gotten a lot better, and I've gotten a bit of free time. With the help of a friend of mine, Dan Janes, I've been porting the existing code to Rust and refining the design for the game-dev-facing API. It's been interesting, since it's my first time running a project on which I am not the sole contributor.

I've certainly run into some problems because of the relative immaturity of the Rust ecosystem - for example, many projects don't use the standard Error trait, which makes using the handy macros that rely on it such as try! nearly impossible, but I've also found that as a whole, the community is very responsive to having these issues pointed out and solved.

Deucalion isn't quite at the level it was before I decided to port it - I'm still struggling to get tilemaps to draw with decent performance, and a lot of design work needs to be done - but it's doing better than I thought it would, and I've discovered some of the best features of Rust so far.

For example, while Rust doesn't have exceptions (because exception handling requires a heavyweight runtime), the convention of returning Result<T, Error> from functions that might fail allows programs to act as if it did. Deucalion implements a single, shared error struct DeucalionError that encapsulates every possible error type (Currently IoError, LuaError, TiledError, NotImplementedError, and Other), allowing callers of risky functions to act according to the actual failure that occurred.

I also like the module system much more than I thought I would at first. While learning when and where to use mod vs use can be a bit of a hassle, the fact that multiple includes create an automatic compiler error is very welcome when compared with C++.

Rust is a great language, and its ecosystem is on its way to becoming as good as that of Python or Ruby. I'm excited for every step along the way.
