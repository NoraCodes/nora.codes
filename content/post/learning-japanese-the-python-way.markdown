---
date: 2016-08-29 22:06:19+00:00
slug: learning-japanese-the-python-way
title: Learning Japanese the Python Way
categories:
- Programming
- Python
description: A short discussion and source listing of a program I used to study Japanese.
---

Now that I'm in college, I'm taking a lot of non-computer science classes, and one of them is Japanese. I'm just starting out, and I need to be able to rapidly read numbers in Japanese and think about them without translating them consciously. I could make a bunch of flash cards, or use a service like Quizlet... or I could write some Python!

For those of you who are unfamiliar, Japanese doesn't have the ridiculous numerical system that English does. One through ten are defined, and eleven is simply (ten)(one). Twenty three, for example, is (two)(ten)(three) (に じゅう さん). This means that rather than having a long list of numbers and special cases, I can just have the numbers zero to ten "hard coded".

After that, the program is pretty simple: if the number is less than 11, simply look it up. If it's more than 11 but less than 20, build it with じゅう plus the second digit. If it's larger than 20, build it with the first digit plus じゅう plus the second digit.

The interactive part is pretty simple too: it runs a loop that randomly generates numbers, checking that they haven't been done before, translates them, and asks me to translate them back. If I succeed, it moves on; if not, it doesn't record the number as having been completed, so I have to do it again at some point in the same run.

This [simple program](https://gist.github.com/NoraCodes/ecb9dcbe44091b9f077d0cb4e0147b0a) came out to 136 lines of very verbose and error-checked Python. It's a good piece of code for a beginner to try and modify - for example, can you get it to incorporate the alternate form of four (し) as well as the primary form? Can you make one that teaches Kanji numbers? (I plan to do both of those things at some point.)

```python
#!/usr/bin/env python3

"""
This is a program to help you study the Japanese numbers.
It currently goes from 0 to 99; I will extend it at a later date.

It can be executed as follows:

    ./numerica.py

which will do all the available numbers, or

    ./numberica.py 10

which will go only up to 10.
"""

numbers = ['ZERO',
           'いち',
           'に',
           'さん',
           'よん',
           'ご',
           'ろく',
           'なな',
           'はち',
           'きゅう',
           'じゅう',
           ]


class OutOfRangeException(Exception):
    pass


def small_to_japanese(n):
    "Convert a number (0-10) to Japanese."
    if n > 10 or n < 0:
        raise OutOfRangeException

    return numbers[n]


def medium_to_japanese(n):
    "Convert a number from 11 - 100 to Japanese"
    if n > 100 or n < 11:
        raise OutOfRangeException

    digits = list(map(
        int, str(n)
    ))

    out = ""

    # Omit いち in numbers > 10
    if digits[0] > 1:
        out += numbers[digits[0]] + " "
    out += numbers[10] + " "
    out += numbers[digits[1]]
    return out


def number_to_japanese(n):
    try:
        return small_to_japanese(n)
    except OutOfRangeException:
        pass

    try:
        return medium_to_japanese(n)
    except OutOfRangeException:
        pass

    print("No way to represent numbers of that magnitude!")


if __name__ == "__main__":
    from random import randint
    from sys import argv

    # Check if there is a command line option for max numbers
    if len(argv) >= 2:
        try:
            MAX_NUM = int(argv[1])
        except ValueError:
            MAX_NUM = -1

        # A little edge case handling
        if MAX_NUM > 99:
            print("Impossible - this program doesn't "
                  "work with numbers over 99.")
            exit(1)

    else:
        # If a max wasn't given, default to 99
        MAX_NUM = 99

    given = ""
    done_so_far = []
    number_done = 0

    while True:
        n = randint(0, MAX_NUM)
        # If and as long as n has already been done, get a new number.
        while n in done_so_far:
            n = randint(0, MAX_NUM)

        try:
            given = input("What is {} in Roman numbers? ".format(
                number_to_japanese(n)))
        except KeyboardInterrupt:
            print("Bye!")
            exit(1)
        except EOFError:
            print("Bye!")
            exit(1)

        if given.lower() == 'quit':
            print("Bye!")
            exit(0)
        if number_done >= MAX_NUM:
            print("You did all the numbers in that set!")
            exit(0)

        try:
            given_n = int(given)
        except ValueError:
            given_n = -1

        if given_n == n:
            print("You got it!")
            done_so_far.append(n)
            number_done += 1
        else:
            print("No, that's incorrect. This is {}.".format(n))
```

