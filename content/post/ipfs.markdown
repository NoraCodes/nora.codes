---
date: 2016-04-10 15:37:11+00:00
slug: ipfs-the-interplanetary-file-system
title: IPFS, the Interplanetary File System
categories:
- Networking
- Open Source
description: A quick primer on the InterPlanetary File System (IPFS), including how it works and what its benefits and drawbacks are for hosting.
---

**IPFS**, the **InterPlanetary File System**, is a content-addressable network.
This means that rather than asking the network for a particular site or domain name (like google.com), you ask for a particular piece of content, and you're guaranteed to recieve it.
There's no possibility of a "monkey in the middle" attack, in which someone maliciously modifies the web page you're trying to access.

To explain it another way, on the normal Web, when you access google.com, the network translates it to an IP address, like `216.58.216.14` or `2607:f8b0:4003:c00::6a`. Then, your computer connects to the server that address refers to and asks it, "Could you send me the content for google.com, please?". This means that Google can change the content on their front page whenever they like - or, if someone malicious is inbetween you and Google, they could change the content. They might, for example, change the login form so that the passwords you enter are sent to them.

On IPFS, however, when you ask for something, you don't request an IP address from the network, but instead ask for a _hash_ of a file - a web page, an image, a video, or whatever. For example, /ipfs/QmbKM1C3ggNVdQtTnQuhvWruyodK6TUnoxjYwg31Q3crcn is the address of a specific version of my GNU/Linux tutorial series. If I change the content, the hash changes.

Of course, people still want to be able to change their content without breaking all the links to it. For that, we have **IPNS**, **InterPlanetary Name System**. IPNS allows you to securely point to mutable content with a hash-like address (/ipns/<whatever>).

These addresses are still not human-readable, but DNS can be used to resolve human-readable names to IPNS addresses, just like it's currently used to resolve IP addresses from human-readable names.

### Benefits

One of the coolest things about IPFS is that the load of serving files is shared between users.
Ever heard of the "Slashdot effect", wherein something cool gets linked to on social media and the server hosting it collapses under the sudden load? With IPFS, that won't happen.

That's because when you request a particular piece of content over IPFS, it gets cached to your machine and sits there until the allotted space is used up, or you decide to remove it.
Anyone who asks for that piece of content has a chance of connecting to your IPFS node before anyone elses and recieving the content from you.
They in turn will cache it, and serve it to future clients.
Everyone who is looking at a particular piece of content shares the load of serving it to everyone else.

### Technical Concerns

There have been worries about hash collisions in the data store. IPFS uses multihash, which allows its hashing function to be upgraded, and currently, implementations use SHA-256. [This](https://stackoverflow.com/questions/4014090/is-it-safe-to-ignore-the-possibility-of-sha-collisions-in-practice) StackOverflow post makes a good point about the likleyhood of such a collision:

> If we have a "perfect" hash function with output size n, and we have p messages to hash (individual message length is not important), then probability of collision is about p2/2n+1 (this is an approximation which is valid for "small" p, i.e. substantially smaller than 2n/2). For instance, with SHA-256 (n=256) and one billion messages (p=109) then the probability is about 4.3*10-60. A mass-murderer space rock happens about once every 30 million years on average. This leads to a probability of such an event occurring in the next second to about 10-15. That's **45** orders of magnitude more probable than the SHA-256 collision. **Briefly stated, if you find SHA-256 collisions scary then your priorities are wrong.**

So, perhaps, when IPFS is truly interplanetary, we will have to switch to a new hash function. That's fine - the way IPFS is built, that's entirely possible.

