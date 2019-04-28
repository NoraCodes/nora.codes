---
title: "Improved User Interface 0.3.0!"
date: 2018-06-13T08:46:55-05:00
categories:
 - Open Source
 - Programming
 - Rust
description: Release announcement for the 3rd pre-release of the Improved User Interface library.
---

The [Improved User Interface crate](https://crates.rs/crates/iui) has had its 0.3.0 release, adding new input fields ([Checkbox](https://docs.rs/iui/0.3.0/iui/controls/struct.Checkbox.html) and [Combobox](https://docs.rs/iui/0.3.0/iui/controls/struct.Combobox.html)), new layout options ([LayoutGrid](https://docs.rs/iui/0.3.0/iui/controls/struct.LayoutGrid.html)), as well as finally working 100% on Windows, and with many bug fixes.
This comes with the 0.1.3 release of the underlying [ui-sys crate](https://crates.rs/crates/ui-sys) to support these features.

It's been a big undertaking to get to this point, and I'm excited to grow from here, now that `libui` itself is moving forward again as well.

I want to give a huge shoutout to several GitHub users who helped with the project, specifically:

* pgvee, who helped fix Windows CI and worked on LayoutGrid
* huangjj27, who did an amazing job fixing Windows builds
* ZakCodes, who fixed an API bug
* masche842, who implemented several new features

From the project management side, we have a vastly improved README, a CONTRIBUTING guideline file, and have resolved to keep a changelog.

I'd like to extend a huge thank you to all my [patrons](https://patreon.com/noracodes) for keeping this ball rolling. I'm excited to see where we go from here.

