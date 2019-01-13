---
title: "A Methodology for Fontconfig Editing"
date: 2018-03-07T11:16:38-06:00
categories:
- Linux
- Open Source
- Modding
description: Making the font cascade behave as you want can be kind of difficult, but it's not impossible, especially with the right methodology and mindset.
---

One of the hardest parts of building beautiful Linux systems is fonts. Font precedence on Linux is generally handled with [fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/).
In essence, `fontconfig` is used to permit many fonts to be installed and uninstalled over time without breaking applications which specify a font or font family, while letting users configure which fonts are used when a missing font, font family, or missing glyph is requested.

This is really useful piece of technology; having a defined configuration system for which fonts are used in which scenarios is a boon for configurability, but fontconfig has no real GUI editors or usable interactive configuration tools. Users are expected to manually edit XML configuration files.

As with most Unix styling topics, Eevee has [a great piece](https://eev.ee/blog/2015/05/20/i-stared-into-the-fontconfig-and-the-fontconfig-stared-back-at-me/) on fontconfig's complexities. She digs into how to disable and re-configure fonts, how to set fallbacks, and how to verify that the correct resolution order is set. Fontconfig relies on a set of config files, generally in `/etc/fonts/conf.d`, which are loaded in alphabetical order. These are usually prefixed with a number, so it's easy to determine the order.

Unfortunately, it can be very complex to determine where a specific font or option is configured. In my recent case, I wanted to switch from `DejaVu` as my default to `Bitstream Vera`, and I spent the better part of an hour flipping around different files changing mentions of `Deja Vu X` to `Bitstream Vera X`. 

Eventually, I settled on the following methodology:

1. Identify problematic resolution result (either by observing it or using `fc-match -s`).
1. In `/etc/fonts/conf.d`, use `grep` or `rg` to search for the incorrectly resolved font (e.g. `rg DejaVu .*`).
1. Open highest-numbered file with a match. For me, this was `69-language-selector-zh-tw.conf`.
1. Determine whether or not this config file is causing the problematic match. In the case of `69-language-selector-zh-tw.conf`, it was only selecting DejaVu Sans Mono for language `zh-tw`, which is actually correct as Bitstream Vera Mono doesn't include `zh-tw` glyphs.
1. If that file might be causing the problematic match, modify it.
1. Check if the problematic resolution still occurs (using fc-match). If so, repeat.

I've been very successful with this methodology so far. In my specific case, I had to modify `56-emojione.conf`, which was setting the default serif, sans serif, and monospace fonts to resolve to DejaVu followed by Emoji One.

