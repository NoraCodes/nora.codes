---
title: "Back to Zsh"
date: 2020-07-14T09:36:23-05:00
description: I stopped using `fish` and switched back to `zsh` - here's how and why.
---

I've gone back to using `zsh`[^zsh] rather than `fish`[^fish] as my primary shell,
for the same reason I use Vim over Emacs - `zsh` is lightweight, compatible, present,
and (with Oh My Zsh![^omz] and Starship[^Starship]) very featureful.

[^zsh]: http://zsh.sourceforge.net/
[^fish]: https://fishshell.com/
[^omz]: https://ohmyz.sh/
[^Starship]: https://starship.rs/ 

First of all, `zsh` feels snappy.
To be totally fair to `fish`, which is a wonderful project, I did not objectively
measure this, but subjectively, `zsh` starts faster than `fish`.
I tried using `zsh` without any extensions for a while,
but found that a lot of `fish` features were missing, and I was a lot less productive.
In the end, I wound up installing Oh My Zsh! and Starship,
but even with the added burden of all this code,
tapping Meta+Enter to pull up a terminal feels snappier with `zsh`.

The big downside of `fish` is that it's not anywhere close to POSIX compliant.
Of course, that's one of its stated goals - to build a _new_ kind of shell -
but, nonetheless, it really hampers its utility in a lot of situations.
While compatibility layers do exist, I found them fiddly at best and hopelessly
broken at worst.
`zsh`, on the other hand, does a great job running all the `bash` and `sh` scripts
and snippets I use to configure machines, most of which I didn't write,
and some of which have been around for nearly a decade.

Of course, it would be possible to take the time to write all of these scripts in
`fish`, or even just port them to nice self-contained shell scripts that can be
run using the appropriate shell.
On the other hand, this code is usually run at most once every few months,
and generally much less.
As [this excellent comic](https://xkcd.com/1319/) demonstrates,
it's probably not worth my time.

Presence is something of the elephant in the room - presence, specifically,
on Mac OS.
I'm beginning a position with CancerIQ, Inc. tomorrow,
and their development environment is standardized on Mac OS.
I've used Apple's operating system professionally before,
but that was back when I, and they, were still blissfully unaware that there were
any alternatives to good old[^applebash] `bash`.

[^applebash]: In their case, [quite old](https://www.reddit.com/r/bash/comments/393oqv/why_is_the_version_of_bash_included_in_os_x_so_old/).

With the introduction of Mac OS Catalina,
Apple's default shell is now `zsh`. [^applezsh]
This means that, with my GNU `stow`-based dotfiles,[^dotfiles]
complete environment configuration is one simple `git clone` away.
`zsh` is also available on pretty much every Linux system I've ever used,
and works alright in WSL.

[^applezsh]: https://www.theverge.com/2019/6/4/18651872/apple-macos-catalina-zsh-bash-shell-replacement-features
[^dotfiles]: https://git.nora.codes/nora/dotfiles

Of course, `zsh` is not as featureful as `fish` - at least not out of the box.
With the addition of Oh My Zsh! and Starship, however,
it's both beautiful and information-dense.

For me, case- and hyphen-insensitive tab completion and a context-aware prompt are
the most critical components of a productive shell environment.
While Oh My Zsh! can provide both, I prefer Starship's aesthetic and functionality.
It's also extremely fast, taking only about ten milliseconds to generate a prompt
in even the most complex environment (`git` detached head with multiple language
environments).

While I loved `fish`, I feel that `zsh` provides most of the features I used while
being much less in my way - it's a fast, compatible shell that's available where I
need it and does what I want.
