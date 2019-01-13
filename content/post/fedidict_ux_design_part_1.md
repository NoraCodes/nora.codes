---
title: "FediDict UX Design, Part 1"
slug: "fedidict_ux_design_part_1"
date: 2018-08-23 18:04:14+00:00
categories:
- Fedidict
- Programming
- Rust
description: My first stab at defining the user experience for my federated dictionary software.
---

_This is the first of several posts on the design of my current ActivityPub-enabled
Rust web application, FediDict. Each of these posts was be released to my Patreon
patrons before being made publicly available._

FediDict, a portmanteau of Federated and Dictionary, is my current open source project.
Most of the time, I've taken the approach of "dive in and write some code; the design
will shake itself out". This isn't a great idea for something I want to be widely
adopted, so I took a different approach for this project.

I do have some code written - mostly dealing with Diesel and the database, as well as
simple HTTP server boilerplate - but I have yet to work on the meat of the application.
In this series, I'll walk through some of the design work.

# What does it do?

FediDict lets people define jargon that they know and look up jargon that they don't, and
share those definitions between disciplines and across the web.

'Jargon' is an important word here; I want FediDict to help people who are reading a text
from a specific discipline find out the meaning of words they don't know, and get some
context on those words in other disciplines as well.

While the word "proband" probably only has meaning in the medical community, something
like "stall" means one thing in aviation and another in automotive repair.
Some words, like "[normal](https://en.wikipedia.org/wiki/Normal)", are so overloaded as
to have multiple meanings in multiple fields. Wikipedia alone lists six in psychology,
one in chemistry, two in physics, and upwards of 25 in various fields of mathematics,
including linear algebra, geometry, and statistics!

Such words can be confusing if misinterpreted, so I'd like each instance of FediDict to
connect to others. That way, they can send definitions around, with attribution, so
that each FediDict instance has access to the whole corpus of information while making it
clear where each definition comes from.

# Seeking Knowledge

## The Guest

A guest coming to the FediDict site just wants to know a specific piece of information.
Let's imagine that I host "computerjargon.online" and that Danielle A. Hacker wants to
know what a "PIC" is. She knows that computerjargon.online is the place to be to learn
about computer jargon, so she enters that URL.

My top priority here is to get out of her way and let her get the info she needs. She
needs to have a big search bar, front and center, where she can start typing the word
she wants to define, so we need a distraction-free home page with search bar.

While she's typing, she might wonder if the ComputerJargon FediDict instance even has
the definition for "PIC". To reassure her, and to make typing long terms (or terms a
user doesn't fully know how to spell) easier, it would be great to have some kind of
typeahead for terms the FediDict instance already knows.

Once the term is entered, FediDict needs to search the database for all the terms
matching the search, and display them.

It's also important to have some way to order these terms. In order to maintain relevance,
the terms from the instance the user actually searched on should come first. In addition,
there should be some mechanism for determining whether or not a given definition is
accurate and relevant. For now, that's the "score"; manipulating it comes later.

## The Member 

Imagine that another person, Jake P. Grammer, wants to look up the same information.
He has a good idea that a "PIC" is a programmable _something_, but he isn't sure what.
He goes through basically the same workflow as Danielle the guest, but with two pre-
conditions: he's made an account, been approved by the moderation team, and signed in.

He navigates to computerjargon.online, enters "PIC" in the search bar, and gets a list
of results, sorted as mentioned above. Unlike Danielle, however, he has the option to
voice his opinion on the relevance of the results. Let's say the results are as follows.

> **PIC**, _n._, by duck3345
>
>    A PIC, or photonic integrated circuit, is an electronic component
>    which uses light rather than electricity for its internal operation.

> **PIC**, _n._, by user123
>
>    A PIC, standing for programmable interrupt controller, is a component
>    of a computer which does x, y, z, and q, and blah blah.

> **PIC**, _n._, by farmer1372
>
>   PIC, particulate inorganic carbon, is some chemistry thing that Leo
>   doesn't know about, but it's certainly not relevant to
>   ComputerJargon.online's database.

The first definition is totally irrelevant to Jake, but he doesn't know if it's wrong, and
it doesn't break any rules. He does, however, know that the entry by user123 on
programmable interface controllers is relevant to his needs. He should be able to signal
this by liking the definition. This action is associated with his account and increments
the score of the post.

The third definition might also catch his eye. It makes no sense to have a chemistry term
on the ComputerJargon database, and Jake wants to help out the site by making the
moderator team aware of this. He should be able to do that by reporting the definition,
another action associated with his account. It will reduce the score of the definition,
as well as sending a message of his choosing to the moderators of the site.

Jake has successfully gotten the information he needs, and has contributed some
contextual information back to the site. As more and more users use the site and look up
"PIC", the most domain-relevant information will be liked most often and will drift
to the top of the list, while lesser-used information will drift downwards and totally
irrelevant information (and downright rule-breaking) can be deleted by moderators after
being reported.

# Sharing Knowledge

Perhaps another user of the site, Eric D. Velopoer, just attended a seminar on frobnitzem
and wants to make sure that everyone knowns the definition of "frobnicate".

He follows the basic flow of the other users to determine whether or not the
definition already exists in the database. In this case, it doesn't exist in the 
ComputerJargon database, but it might exist on other instances. Whether or not it does,
the result of the search is clear.

Once he has found that the definition doesn't exist, Eric has a clear path to
create a new definition.
(He also has the option to add a new definition for which terms already exist.)

Note that I deliberately do _not_ want people to be able to add definitions without
searching for them first, because that is likely to lead to duplication.

Once Eric has decided to add a new definition, he's presented with a form to
add the definition. It has a prefilled, uneditable entry with the term being defined,
a dropdown for part of speech (verb, noun, etc), and a large text box for the definition.

It might also be useful to provide a way to enter related terms or "see also"s for a
definition. For instance, Eric might want to mention that "frobnicate" is related to
"frobnitz", "twiddle", and "tweak".

Eric's definition might end up looking like this (from the Original Hacker's Dictionary):

> **frobnicate**, _v._
>
> To manipulate or adjust, to tweak. Derived from "frobnitz".
> Usually abbreviated to frob. Thus one has the saying "to frob a frob". 
> "Frob", "twiddle", and "tweak" sometimes connote points along a continuum.
> Frob connotes aimless manipulation; twiddle connotes gross manipulation,
> often a coarse search for a proper setting; tweak connotes fine-tuning.
> If someone is turning a knob on an oscilloscope, then if he's carefully
> adjusting it he is probably tweaking it; if he is just turning it but
> looking at the screen he is probably twiddling it; but if he's just doing
> it because turning a knob is fun, he's frobbing it. 
> 
> See also: frobnitz, tweak, twiddle.

That definition, once submitted, goes onto a moderation queue, so that the site
moderation team can decide whether or not to allow it to go public.

# Next Steps

These are the basic workflows for FediDict users - looking things up, giving feedback on
existing definitions, and submitting new definitions. In the next post, we'll look at the
security aspect of design, including anti-abuse tooling.

