---
date: 2016-08-29 22:06:19+00:00
slug: learning-japanese-the-python-way
title: Learning Japanese the Python Way
categories:
- Programming
- Python
---

Now that I'm in college, I'm taking a lot of non-computer science classes, and one of them is Japanese. I'm just starting out, and I need to be able to rapidly read numbers in Japanese and think about them without translating them consciously. I could make a bunch of flash cards, or use a service like Quizlet... or I could write some Python!

For those of you who are unfamiliar, Japanese doesn't have the ridiculous numerical system that English does. One through ten are defined, and eleven is simply (ten)(one). Twenty three, for example, is (two)(ten)(three) (に じゅう さん). This means that rather than having a long list of numbers and special cases, I can just have the numbers zero to ten "hard coded".

After that, the program is pretty simple: if the number is less than 11, simply look it up. If it's more than 11 but less than 20, build it with じゅう plus the second digit. If it's larger than 20, build it with the first digit plus じゅう plus the second digit.

The interactive part is pretty simple too: it runs a loop that randomly generates numbers, checking that they haven't been done before, translates them, and asks me to translate them back. If I succeed, it moves on; if not, it doesn't record the number as having been completed, so I have to do it again at some point in the same run.

This [simple program](https://gist.github.com/leotindall/ecb9dcbe44091b9f077d0cb4e0147b0a) came out to 136 lines of very verbose and error-checked Python. It's a good piece of code for a beginner to try and modify - for example, can you get it to incorporate the alternate form of four (し) as well as the primary form? Can you make one that teaches Kanji numbers? (I plan to do both of those things at some point.)
