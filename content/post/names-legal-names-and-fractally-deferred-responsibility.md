---
title: "Names, Legal Names, and Fractally Deferred Responsibility"
date: 2020-05-11T00:05:15-05:00
description: "Don't store legal names, at least not as the default. Here's why, and why it matters."
---

A person’s “preferred name” is their name, their “legal name” is something else, and it’s our duty as software engineers not to muddle the two up. 

That means software should avoid keeping track of users’ legal names, and software that must record legal names should have a “name” field and a “legal name” field, rather than a “name” field meant for a legal name and a “preferred name” field.

## Names

I suspect few would disagree that a name is a collection of characters[^not_letters] which represents a collection of sounds,[^letters_and_sounds] which people use to refer to an individual in a particular context.

[^letters_and_sounds]: It is possible that, in our modern society, the letters come before the sounds, and the sounds represent the letters. I’ll leave that one to the philosophers and linguists to hash out.

[^not_letters]: As several commenters pointed out, it's not just letters, as Mrs. O'Leary or X Æ A-12 Musk will tell you.

That statement is worded very deliberately. **People** use names to refer to **other people**; yes, organizations have names, but those are almost always much more fixed and less commonly abbreviated. Elizabeth may be Lizzie, Liz, and Ella to different friends and relatives, and of course William can be Will, Willy, Bill, or Billy, but it's a rare company that gets such nicknames, beyond initilisms.

Names are both sound and text; few people have names that are only written, or only spoken. And the purpose of a name is to refer to an individual within a given context. You may be Danny to your buds at the bar and Mister Khan at the office; those are both "your name."

Experience shows that names are not unique, and are not guaranteed to follow any specific format, to be of any minimum or maximum length, to be the same for one person throughout their life, or even to exist for each person you (or your software) will encounter.[^falsehoods]

[^falsehoods]: “Falsehoods Programmers Believe About Names”, https://www.kalzumeus.com/2010/06/17/falsehoods-programmers-believe-about-names/

Despite this belabourment, names, to the software engineer, are very simple. A name is a collection of characters that identifies a person. It almost certainly doesn’t identify them uniquely[^usernames], and importantly, it need not bear any relation to their “legal name”. As software engineers, we are descendants of a proud[^too_proud] heritage[^esr] of hackers who often take great pleasure in assigning and using names, called "handles", which are completely unrelated to their given or legal names.[^handle]

[^too_proud]: Perhaps too proud, at times.

[^esr]: The prime history of which is, sadly, now maintained by Eric Raymond.

[^handle]: See *The New Hacker Dictionary*, "[handle](http://www.catb.org/jargon/html/H/handle.html)". 

[^usernames]: When we allow a user to choose a name-like unique identifier, it’s often called a username, a handle, or an “at” (from the Twitter convention of using @whatever to signify that “whatever” is inline metadata referring to a user by username.) This is not a name.

## Legal Names

Nonetheless, many national, regional, and local legal systems make some assumptions about names. For example, in the United States, the Social Security Administration assumes that no name will be longer than 32 characters - two rows of 16. Most critically, many legal systems, and thus the bureaucracies attached to them, require that those who interact with them have a name, generally a first and last name, and often no more than a single middle name.[^my_middle_name]

[^my_middle_name]: I have two middle names. While my Social Security card lists part of my second middle name, and my California ID card lists both in their entirety, I rarely see my second middle name survive the machinations of bureaucracy.

Legal names are complex entities that live in a strange world of convoluted, arbitrary, and often contradicting rules and data formats. They are exactly the kind of things programmers should stay away from as much as possible.

## Names and Software Extension

By way of example, let us examine the situations in which I come into contact with my “legal name”. My legal first (or given) name is “Leonora” (lay-oh-no-rah), but nobody really calls me that; I go by “Nora” (no-rah) online, to my family and friends, to my colleagues, and in most contexts in which a human is directly interacting with me.

My college has an ERP system which it uses to, among other things, relate students’ billing profiles to their meal plans. This is, of course, a critical function, as it allows us to eat on campus. The ERP system has a “preferred name” field, but it was added after the critical integrations were made. Therefore, I am “Leonora” to residential life, my professors, and the person who swipes my ID at the student commons.

In my case, this isn’t a huge problem, but it serves to illustrate a point: when a structured collection of data includes a “name” field, it is more likely than not that future developers - either internal to your organization, or external developers writing code that interfaces with your system - will use that field to identify users, preferentially even to other “name-like” fields (such as “preferred name” or “nickname”.)

## Harmful Uses of Legal Names

Hearing my full name called out in roll call, or being addressed strangely at the door to the dining hall, isn’t a terrible burden. There are, however, larger consequences of the neglect of the “preferred name” field.

For instance, many international students choose to adopt an Americanized name to avoid the embarrassment and inconvenience of hearing every professor and staff member in the school mispronounce or outright skip over their name. Transgender people often have to interact with systems that use their legal name unnecessarily, causing distress and sometimes outing them to people to whom they would otherwise not be interested in discussing such a personal aspect of their lives.

Changing one’s legal name, except during marriage, is a long, difficult, and expensive process. I legally changed my name last year, and it cost me over $500; depending on the jurisdiction and the situation, name changes can require months of newspaper listings, thousands of dollars, and even court appearances.

Proving that one’s legal name has changed is also difficult. Even in cases where software systems are set up to accept legal name changes, there is almost always a human in the loop somewhere verifying that the documents submitted by the user are valid. This is harmful to the user, who must sometimes even appear in person to prove their name change, and to the operators of the software, who must dedicate personnel to the mind-numbing task of verifying court orders.

As I mentioned, I changed my name last year. I am still finding systems in which my name hasn’t been updated; I’ve long surpassed the commonly-cited 100 hour mark,[^100hours] and I’m sure I’ve cost at least that much time in labor from other institutions.

[^100hours]: https://medium.com/bits-and-behavior/100-hours-of-name-change-labor-c652c22a89b9

## Fractal Deferment of Responsiblity

I was prompted to write this article by the experience of a friend of mine who was somewhat embarrassed by having his full, legal name called out in front of a room full of people who only knew him by another name. The was the result of a software system suffering from precisely the ailment I described: someone used the “name” field for the name displayed to kitchen workers who were preparing an order. There was no need for his legal name to be used there, but either the integration was made before the “preferred name” field was added, or the developer of the integration simply didn’t think to use it.

Upon asking about why that name had been used, he was told that “they just used what was in the system.” And, of course, that’s true! It’s not their fault. In fact, there was nothing they could do about it. There’s probably nothing their boss could do about it, either, or his boss, or the CIO of the college, or the President of the college.

“That’s just how the software works.”

For another example, my email address uses the fancy new “.codes” top level domain. Many old software systems don’t think it’s valid, which makes it somewhat hard for me to sign up for things. This happened with my health insurance company, and I had to spend three hours on the phone to get a complaint registered. They special-cased my account after a month; I still don’t think they’ve fixed the system.

We, as software engineers, build software that is inscrutable to the majority of our own, let alone people who don’t work with computers. Even open source software is difficult to change on a moment’s notice, and I would bet my bottom dollar that this software was not open source.

So, people will fractally defer their responsibility. It’s not the line workers’ fault. It’s not their boss’s fault. It’s not the fault of the person who procured the software, or the manager of the engineers, and if you ever make it this far down the chain, I'll give you even odds that the engineer who made the decision has left for greener pastures.

We have to get things like this right, or they’ll never get fixed, because they’re nobody’s responsibility.

So, returning to names: what should we do?

## Do Not Record Legal Names (Unless You Have To)

My advice is to save yourself and your organization the trouble. If you’re not paying people or interacting with ICANN, the NHS,[^nhs] or the Selective Service System, **it’s unlikely that you need to record legal names**.

If people are paying you, you’re almost certainly recording a billing address for each transaction; let them enter a payment name there. That’s likely to be useful for a variety of reasons, from corporate reimbursement to parental charity. In any case, it’s possible that their "legal name" _isn’t_ the name on their credit card or bank account, if, for instance, they recently changed it;[^illegal] if your software can't account for that, it's not just annoying, it's incorrect.

If you create a form with a "name" field, label it somehow with the information that it isn't asking for a "legal name". Many websites with forms have hover-based explanatory text. This is a great place to put something like, "Whatever most people call you," "What you'd like us to call you," or, if you're particularly stuffy, "Legal name not required."

[^illegal]: Contrary to what many told me, I’ve been assured by the court that changed my name that this is perfectly legal, at least in California. You’re under no obligation to tell anyone in particular about your name change.

[^nhs]: In the UK, there isn't a single concept of a "legal name", and indeed, a lot of software that makes this assumption is guilty not only of being annoying and incorrect but also US-centric. The NHS, however, does require an "official name" which can be verified against birth date, NHS ID, and other information.

## Names Aren’t Legal Names

If you do find yourself needing to record legal names, **those shouldn’t be the main way your system refers to your users**. Give them a “Name” and “Legal Name (if different)” field on signup forms, or let them uncheck a “This is my legal name” checkbox if you feel like that adds too much clutter. Then let them change their name any time, and only use the “legal name” field when you need to.

## Names Are Mutable

Furthermore, the "name" field should be trivial for a user to change.[^jcrawfordor] Becuase a "legal name" field can be used when official documents are involved, there are few ramifications that won't be controlled by existing social structures. This means using something else as a unique key, like a UUID or even a simple serial index. Many operating systems, for instance, are guilty of making it very difficult to change a username. 

[^jcrawfordor]: Hacker News user jcrawfodor rightly suggested that this point be discussed more directly. It's very important.

I promise you that your users with long names, your transgender users, and your users who are getting ready to get married, or divorced, or to leave abusive living situations and want to leave their old name behind[^abuse], will thank you. And so will your customer success team, because they won’t have to process stacks of name change papers!

[^abuse]: Some commentary brought up the possibility of abuse of name-change mechanisms. While I think it's possible that name changes could be abused to, e.g., create offensive-sounding names, this strikes me as rare and easy to detect - and if changing a name isn't a big deal in your system, someone can just change it back. Furthermore, a lot of of people do actually have names that you might consider to be "offensive". After the initial publication of this article, someone with the surname "Hooker" contacted me to tell me that he had been denied access to systems in the past because people assumed his name was an attempt to insert offensive content.

Most critically, though, **be clear about how information will be used**, especially names, at the time it is collected. If you're asking for my name on this particular form so you can give me a W2, say, "This is the name we'll put on your W2." If you're asking my what my name is so you can strip everything but the first cluster of letters and put it in your newsletter, say so!

