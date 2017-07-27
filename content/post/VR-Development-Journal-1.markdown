---
date: 2017-06-24
title: VR Development Journal 1
slug: vr-development-journal-1
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

I've learned quite a lot about VR experiences in the last month and a half, by sampling many and reviewing two, [Quanero](/post/2017/05/27/quanero-vr-a-review.html) and [Battle Dome](/post/2017/06/16/battledome-vr-a-review.html). I've tried simple visual demos and complex puzzle games, from AAA developers (_The Lab_ from Valve), mid-size studios (_Raw Data_ from Survios), and indie development teams (_Accounting_ from Crows Crows Crows and Squanchtendo). Each of these experiences was unique and each used the technology slightly differently. 

I've also been working through a 3D modelling course (for the free Blender 3D art package) from Udemy, which has been very informative, and not just in technical knowledge. The course also teaches a lean development model which is optimized for small teams with not many resources - perfect for my situation.

I considered a number of ideas for games before settling on my current project, which I am calling _Rebound_. At first, I wanted to create a low-gameplay experience that would essentially be like a museum exhibit, a 3D rendering of a Mars habitat which the player could explore, with small virtual "plaques" to explain every part of the structure and equipment. I also spent some time on a design for an adventure game.

In the end, it was the locamotion problem that led me to the design of _Rebound_. As I discovered during my time with _Battle Dome_, smooth movement has a tendency to induce very bad motion sickness, [known as "cybersickness"](http://dl.acm.org/citation.cfm?doid=333329.333344), because the perceptions of the eyes and the intertial measurements of the inner ear don't match up. _Battle Dome_ hits a sweet spot by providing a choice between smooth, trackpad-based motion for those who can stomach it and teleporting motion for the rest of us. Many games settle for building a traditionally-balanced game and retrofitting it with arbitrary-position teleporting motion, which tends to produce rather odd gameplay.

My original ideas would have been built around **fixed-position teleportation**, which restricts the player's possible positions to a predefined set of points. This would allow me to focus my artistic efforts on those locations, which could compensate in part for my lack of experience and time. I realized, however, that this kind of game wouldn't be much different from existing experiences like those implemented in _The Lab_ and _Quanero_, so I began to think about the other kind, **arbitrary-position teleportation**. 

In that locamotion system, the player is given the ability to instantaneously transport themselves to anywhere within a predefined area, sometimes with a restricted range and/or a "cooldown" timer between transportations. As mentioned above, many games simply bolt this functionality on, but I wanted to create a game that would require the player to use APT to their advantage. As I was thinking about how, I discovered the VR Development Toolkit ([VRTK](https://vrtk.io)), which provides a level of abstraction over the various VR SDKs (Vive, Oculus, et cetera). Putting these ideas together brought me to _Rebound_.

_Rebound_ is a physics based game, in which the basic interactions are teleporting and bouncing projectiles off of a round shield or racquet. These projectiles bounce off of walls without losing momentum, but their color decays on each bounce (to a lower wavelength, that is) and eventually vanish. If the player is hit, the game is lost. Teleportation makes dodging them easy, but by deflecting them in the right direction, the player break walls, activate buttons, and even destroy the emmitters of the projectiles. Once I have the basic physics working, the main challenge will be designing levels that require the player to take full advantage of both teleportation and deflection.