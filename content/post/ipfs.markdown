---
date: 2016-04-10 15:37:11+00:00
slug: ipfs-the-interplanetary-file-system
title: IPFS, the Interplanetary File System
categories:
- Internet
- Open Source
tags:
- internet
- programming
- python
- software
- open source
---

I've been using (and working on) a project called IPFS. It's a new way of distributing content - like the Internet, but it doesn't go away if one server goes down. People much smarter than me came up with the idea; I'm working on the Python implementation of the standard, to allow people to embed it in their Python applications.

<!-- more -->


### What is IPFS/IPNS?

**IPFS**, the **InterPlanetary File System**, is a content-addressable network. This means that rather than asking the network for a particular site or domain name (like google.com), you ask for a particular piece of content, and you're guaranteed to recieve that content - there's no possibility of "man-in-the-middle" attacks replacing webapps with malicious ones.

To explain it another way, on the normal Web, when you access google.com, the network translates it to an IP address, like 216.58.216.14 or 2607:f8b0:4003:c00::6a. Then, your computer connects to the server that address refers to and asks it, "Could you send me the content for google.com, please?". This means that Google can change the content on their front page whenever they like - or, if someone malicious is inbetween you and Google, they could change the content. They might, for example, change the login form so that the passwords you enter are sent to them.

On IPFS, however, when you ask for something, you don't request an IP address from the network, but instead ask for a _hash_ of a file - a web page, an image, a video, or whatever. For example, /ipfs/QmbKM1C3ggNVdQtTnQuhvWruyodK6TUnoxjYwg31Q3crcn is the address of a specific version of my GNU/Linux tutorial series. If I change the content, the hash changes.

Of course, people still want to be able to change their content without breaking all the links to it. For that, we have **IPNS**, **InterPlanetary Name System**. IPNS allows you to securely point to mutable content with a hash-like address (/ipns/<whatever>).

These addresses are still not human-readable, but DNS can be used to resolve human-readable names to IPNS addresses, just like it's currently used to resolve IP addresses from human-readable names.

### Some interesting IPFS links

NOTE: All of these links are to gateway.ipfs.io, the publicly available gateway for IPFS. If you want to use them for more than a short time, please install IPFS and this browser extension: ([Chrome](http://ipfs.io/ipfs), [Firefox](https://addons.mozilla.org/en-uS/firefox/addon/ipfs-gateway-redirect/?src=cb-dl-recentlyadded)). It produces less load on the public gateway and is really the way IPFS is meant to be used. Without it, you may as well just be using the normal 'net!



	
  * Every Atari 2600 game ever made, hosted [by IPNS](https://gateway.ipfs.io/ipns/QmcvijUD6yUtq2ciKkv9HW38Xx9PQk44LhQvDxAcqpQZkg/) or at its [static IPFS address](http://gateway.ipfs.io/ipfs/QmacAqRVhJX9eS7YJX1vY3ifFKF9CduDqPEgaCUSa4x5xb/).

	
  * [ipfs.pics](http://ipfs.pics), a frontend for storing pictures in IPFS.

	
  * [Homestar Runner](http://gateway.ipfs.io/ipfs/QmVJ5LiYPQzZ3DGgLs5fAXFX7E8v4mZPmrBwfVo56Dvt7S/www.homestarrunner.com/ccdo7b.html), a popular cartoon, now hosted in IPFS.

	
  * A JavaScript [OpenRISC emulator](http://ipfs.io/ipfs/QmRymducEftvkHuYEVJFkcNtSiooUVXzsvNoPnNEPu2o1a) running Linux.

	
  * [Star Trek Horizon](http://ipfs.io/ipfs/QmYGD41np8igDK743qMxKuXcJ7JVdryhNnRgXAypD6HH83) (fan made)










There have been worries about hash collisions in the data store. IPFS uses multihash, which allows its hashing function to be upgraded, and currently, implementations use SHA-256. [This](https://stackoverflow.com/questions/4014090/is-it-safe-to-ignore-the-possibility-of-sha-collisions-in-practice) StackOverflow post makes a good point about the likleyhood of such a collision:


If we have a "perfect" hash function with output size n, and we have p messages to hash (individual message length is not important), then probability of collision is about p2/2n+1 (this is an approximation which is valid for "small" p, i.e. substantially smaller than 2n/2). For instance, with SHA-256 (n=256) and one billion messages (p=109) then the probability is about 4.3*10-60. A mass-murderer space rock happens about once every 30 million years on average. This leads to a probability of such an event occurring in the next second to about 10-15. That's **45** orders of magnitude more probable than the SHA-256 collision. **Briefly stated, if you find SHA-256 collisions scary then your priorities are wrong.**


So, perhaps, when IPFS is truly interplanetary, we will have to switch to a new hash function. That's fine - the way IPFS is built, that's entirely possible.


