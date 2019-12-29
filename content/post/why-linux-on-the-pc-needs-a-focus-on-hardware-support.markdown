---
date: 2016-08-02 16:28:47+00:00
slug: why-linux-on-the-pc-needs-a-focus-on-hardware-support
title: Why Linux on the PC Needs a Focus on Hardware Support
categories:
- Open Source
- Culture
- Linux
description: Frustrations with the Linux desktop regarding hardware support.
---

Linux cannot fall behind in hardware support. In fact, if we ever want to capture the desktop market, we have to race ahead.

A few days ago, I had an interesting and somewhat frustrating experience with a friend of mine. Their laptop was dying, so they asked me to give them some suggestions for a new one.

Their requirements were a computer with a display that was good for reading, enough power to be responsive and able to multitask well, and rapidly accessible storage, but not necessarily a lot of it. Of course, I immediately thought of the System76 Lemur, which happened to be on sale at the time; however, after going through a whole list of pros and cons of Ubuntu with the friend, they told me that they wanted to go with Windows or Mac OS X as they “didn’t have time to tinker”.

They’re right, of course, but this really got under my skin. The thing is, if you’re not trying to game on Linux, there are absolutely no problems using it on a desktop in almost all cases. Desktop hardware is nicely standardized and “just works”. But on a laptop? Nope. It’s a crapshoot as to whether your wireless Internet and Bluetooth will work, or if your touchpad’s multi-touch will be usable. On my Dell Inspiron 7000-series laptop, which works almost flawlessly with Ubuntu GNOME, the wireless chipset will occasionally forget that any but a single network, to which I'm not connected, exists.

Why is this? Well, it’s because laptops are often very, very custom. They have custom form factor motherboards with non-standard sets of features. Battery life is often the primary concern rather than compliance, and release cycles are very tight, so if a new hardware system is developed, drivers are produced for the target platform (almost always Windows) and released there. The Linux community has to hack together our own after the fact.

Windows runs on most laptops, and has a lot of big issues. Privacy concerns, resource overutilization, extremely poor real-time performance, and a massive lack of customization are the obvious ones, along with a downright byzantine user interface without much power to back it up (in the consumer versions, that is). Mac OS X looks simple on the surface while still exposing the massive power of a UNIX to its power users and developers (although this is becoming progressively less true). On the other hand, it costs an absolute fortune to buy into that ecosystem, and that is where Linux comes in.

In reality, Linux is a modern UNIX like Mac OS X, and it is far more flexible and powerful, but to many people, it's just "Windows that costs less". What we need to be is a Mac OS X that can run anywhere. Linux needs to be simple on the surface, which most DEs accomplish brilliantly, while exposing the power of the underlying OS, which isn't hard given a terminal emulator. Where Linux falls short is the "runs anywhere" part.

> An update, four years on. Things are better now; a lot better. I've managed to stay
almost entirely on Linux as my desktop OS by relying on the excellent support for ThinkPad
hardware. The mainline kernel supports a lot of hardware out of the box. On the other hand,
as more custom laptops come out with highly integrated hardware, there is a class of
very cheap laptops that would be nice to use as Linux machines that have hardware that isn't
even really well supported on Windows, and is totally unusable on Linux.
