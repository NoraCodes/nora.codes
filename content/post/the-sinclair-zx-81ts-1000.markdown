---
date: 2016-04-10 15:42:56+00:00
slug: the-sinclair-zx-81-ts-1000
title: The Sinclair ZX-81/TS-1000
categories:
- Hardware
- Hacking
description: A beautiful plastic slab of an 80s home computer and how I got it working with a modern dashboard display.
---

![ts_1000_in_styrofoam](/images/ts_1000_in_styrofoam-169x300.jpg) 

I recently watched the excellent film Micro Men, about Sir Clive Sinclair, Sinclair Research, Acorn Computers, and the fight for the British low-end computer home computer market in the early 1980s. This, along with a healthy (unhealthy?) dose of procrastination, lead me to spend a few hours browsing eBay, looking for old PCs. "Timex Sinclair 1000 Retro Home Computer, New In Box". I checked the price - $29.99 plus shipping. It has been one of the most interesting purchases of my life.

The Timex Sinclair 1000 was originally the ZX81, designed by Rick Dickenson for Sir Clive Sinclair's Sinclair Research, and built by Timex (yes, the watch company) in Scotland. It sold extremely well in Britain. In America, however, the Sinclair brand was mostly unknown, so the computer was released there as the Timex Sinclair 1000. It also recieved some modifications and upgrades, most notably a _whole extra kilobyte_ of RAM. (I make fun of it now, but at the time it was a big deal.)
Over its three-year run, the ZX81 sold 1.5 million units under that name and an unknown number in America as the TS1000. In addition, several unauthorized clones were created in the Soviet Union in 1982 and sold well there until at least 1986.

Like most computers at the time, the ZX81 used the Zilog Z80A processor. It was clocked at 3.85 MHz, and that was approaching the practical limits of the Z80, which was not well binned and not very good at disdissipating heat. Even so, industrious Z80 users have managed to increase the clock to as much as [7.3 MHz](http://imgur.com/a/KPnWk). The ZX81 came with 1 KB of RAM, or 2 KB in the TS1000. This was a lot for £99, but it wasn't enough to have, say, a real parser for the included Sinclair BASIC - tokens like 'PRINT' were entered whole, as Shift + P, for example. The Shift key could be used in this way because the ZX81 actually didn't support lower-case characters.

![ts_1000_money_shot](/images/ts_1000_money_shot-300x169.jpg)

Because even 2 KB of RAM isn't really sufficient for anything more than simple BASIC programs, official and unofficial RAM packs became extremely popular. The ZX81 was, however, notorious for connection problems with this hardware, which plugged directly into the mainboard with a very flimsy connector. In addition, the requirement for a tape drive to save and load programs has prompted more modern ZX81 enthusiasts to add EPROM, EEPROM, and flash chips to the system in order to store programs.

If the Z80A processor was the brain of the ZX81, the Ferranti Uncommitted Logic Array was the heart. It did everything the all-digital Z80A couldn't: video and audio synthesis, clock stabilization, and keyboard control, among other tasks.
Because most households in the 1980s didn't have a TV with composite video input, the ZX81's ULA fed a composite signal, PAL for the UK and Europe, and NTSC for America, into the RF modulator. This modulator (pictured below with my bypass buffer modification) made the video appear to be a broadcast TV signal, allowing home TVs to display it with minimal user effort. The TS1000 included an RF switch that allowed connecting the real TV antenna and the computer so that they could be switched between at any time.

![009_compact](/images/009_compact-300x169.jpg)

When I received my Timex Sinclair 1000, I was amazed at the small size. It's really not larger than a typical hardback YA novel (you know the type), and is pretty well put together for something built down to a price. I would have loved to use it unopened, but unfortunately there's no way to do so these days, so I figured I may as well take some photos. You can find them with annotations [here](http://imgur.com/a/R0m4r).
The main modification I've done so far is to bypass the RF modulator. This let me use the computer with my projector, which is the only composite display I owned at the time I bought the computer (I have since bought a $15 car backup camera monitor, which works well with the same modification). I used a single NPN transistor with a 220 Ohm resistor to create a collector follower buffer to avoid stressing the ULA, then added a set of diodes in series to drop the voltage to a level that my projector can tolerate. You can see more in-depth instructions [here](http://imgur.com/a/ipUVq).
I plan to add a reset button, an on/off switch, a power LED, and possibly a stepper switch and a bus visualizer. This would make it much less authentic but also much more interesting to watch in operation, since the entire computer could be stopped and clocked at a human-viewable rate, with the bus visualizer allowing me to view the status of the various interfaces to the CPU. I might also add a fake tape drive or an EEPROM to allow saving and loading programs.
