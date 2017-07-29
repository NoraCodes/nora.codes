---
date: 2017-07-02
title: VR Development Journal 2
slug: vr-development-journal-2
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

Since my [last devlog](/post/2017/06/24/vr-development-journal-1.html), I've spent a lot of time thinking about locomotion and the visual cues that VR teleportation gives - and the answer is usually "not many". The view simply fades out and fades back in, or even cuts from to the destination in one frame. This is the best option for minimizing motion sickness - especially with a small delay in between leaving and arriving - but tends to be somewhat disorienting.

In a game like _Rebound_, disorientation isn't really acceptable. The play must always know where they are, where the launchers are, and where the targets are, as well as the relative location least some of the geometry of the level. Therefore, I decided to look into locomotion methods that don't cause sim sickness but still provide the player with some visual cues for their motion.

Fortunately, VRTK (the SDK abstraction layer I'm using for Rebound) has a pre-implemented teleportation mode called "dash" teleporting. Rather than simply plopping the player down at the destination, the player is moved extremely rapidly along a straight path. The very short duration of the movement helps to prevent sim sickness, but the player still has good visual cues as to their new location's relation to the starting point.

One complication is that there may be obstacles between the player and the destination. The sensation induced by a large object approaching at high speed is not particularly pleasant, but VRTK actually provides an almost out of the box solution. Any object between the player and the destination will be detected and reported. Some simple code to hide those objects solves the problem.

As before, I'm convinced that locomotion is the central problem facing VR developers, and I'm a step closer to finding a solution that works well for _Rebound_.
