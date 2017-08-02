---
date: 2017-07-11
title: VR Development Journal 3
slug: vr-development-journal-3
categories:
- Programming
- Game Programming
tags:
- vr
- virtual reality
- htc vive
- locomotion
- teleportation
---

Progress on Rebound is good. It's in a state where I've been able to experiment a bit with level design. As I suspected, juding the physics of a ball bounce is quite hard, so I've decided to make it a priority to make that part as easy as I can. One major component of this is letting the player know where a ball is coming from before it's actually hurtling toward them.

I originally tried to do this with sound. Sound is very important, and I'm definitely proud of how I got the sound effects to sync up. In VR, however, the primary sense being engaged is still sight, so visual cues are very important. In order to let the player know which launcher is about to launch a ball, I have a ring of particles appear around the barrel and slowly draw closer, synced up with the "vwoooop" sound effect. Those same particles are then sprayed out in a randomized shower just as the ball is created and sent hurtling at the player.

This double use of the particle effect - to signal the "preparing to fire" state of the launcher, and to mask the creation of the ball - helps to provide a visual unity to the launchers, as well as providing a simple gameplay effect. I'm still trying to decide whether or not to make the balls emit such particle effects on contact with the walls and other obstacles, or only when they finally run out of bounces - on the one hand, it would make their impacts feel much more energetic and dangerous; on the other hand, it will create a lot of visual clutter.
