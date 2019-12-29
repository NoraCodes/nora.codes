---
title: "U2F Zero in Firefox"
date: 2018-03-26T11:48:21-05:00
categories:
- Linux
- Open Source
- Hardware
description: How to use a U2F device such as the U2F Zero in Firefox on Linux.
---

I recently got a [U2F Zero](https://u2fzero.com), a tiny (about 2 inches long) and cheap (about 8 dollars) device implementing the FIDO Universal Second Factor protocol. It's open source, too, which is awesome.

![A closeup of the U2F Zero](https://camo.githubusercontent.com/8f7abb7f684061138bd2a0aefa631a6fddad0d35/68747470733a2f2f692e696d6775722e636f6d2f4865725a6857512e6a7067)

I primarily use Firefox. Unfortunately, Firefox doesn't support U2F out of the box; you have to enable `security.webauth.u2f` in [about:config](about:config).

Once enabled, I tried to register it with GitHub. Unfortunately, it kept on failing, despite the little green light on the U2F Zero going blue.

Eventually, I discovered that one security measure - Linux isolating Firefox from my USB devices - was getting in the way of this new one. I added the contents of [this file](https://github.com/Yubico/libu2f-host/blob/master/70-u2f.rules) to `/etc/udev/rules.d/10-u2f.rules` and ran `sudo udevadm control --reload` to reload the rules, and it worked!

The process is as follows:

1. Log into a website that supports U2F
1. Enable second factor with some 2FA app, like Google Authenticator
1. Find the interface to register a U2F device
1. Plug in U2F Zero
1. When light is green, press the only button on the device

The light turns blue and, if all goes well, the device will be registered. Now, instead of pulling out your phone and entering a code, just whack in the U2F Zero and hit the button when you log in.

Works like a charm, and the bare matte-black PCB means you can keep your hacker aesthetic going, or 3D print a case to your liking.

