---
title: "Modding, Vim, i3, and Efficiency"
date: 2018-03-06T18:50:13-06:00
categories:
- Modding
- Linux
- Open Source
description: In which I try to justify why it's actually really good that I spend hours trying to pick the right shade of blue for my terminal font.
---

I spend a great deal of time _modding_ my Linux machine. Practiced by many Linux users, modding is the process of making a Linux installation pretty, by changing the color schemes, fonts and font sizes, icons, default applications, and the desktop background. As a noun, a _mod_ is the final product of that process: a computer system which looks pretty while remaining functional.
For example, here are screenshots of my two most recent mods.

> ![Screenshot of my dark NASA mod](/images/rices/2018-nasa-02.png)
> My previous mod, a dark blue/grey theme with red highlights.

&nbsp;

> ![Screenshot of my purple space mod](/images/rices/2018-purple-space-01.png)
> My current mod, a purple theme with pink and white highlights.

A great many other examples of very pretty mods can be found on [Reddit](https://reddit.com/r/unixporn).

I've always found modding my system to be a very calming process. It re-familiarizes me with the machine I use to access the world, makes me re-think my work process, and encourages me to heavily optimize every aspect of what I do.

I have a deep sense of aestheticism regarding the software that I use, and my mods reflect this. I use the [i3](http://i3wm.org) window manager, which rather than drawing borders with titles, close buttons, and resize handles around windows simply "tiles" them so that the whole screen space is always used. It also allows me to use the machine exclusively via the keyboard.

My editor of choice, [vim](http://vim.org), is of a similar philosophy. Simple and configurable, it is almost entirely keyboard driven and easily configurable.

Even my computer's name, Asfaloth (after [Glorfindel's horse](http://lotr.wikia.com/wiki/Asfaloth)), reflects this preference. I don't need it to be fancy; it just needs to get me where I'm going, rapidly and without fail.

Despite this, I've historically used heavy-weight, mouse driven IDEs like IntelliJ for development. Even when not working in an IDE, I tend to use a graphical editor like Visual Studio Code or Atom. I've gotten used to it, but every time I mod my machine, it grates on my. Everything I do in the terminal lets me enjoy my mod, seeing the color scheme and background blend beautifully as I work, but these "advanced" editors don't.

![Screenshot of my VSCode development setup](/images/rices/vscode.png)

The reason I use them is their efficiency - they integrate spell checking, style checking, type checking, container management, find/replace, and many other features into one application. The other day, I realized that there's no real reason I can't do this all through the terminal!

I spent some time setting up Vim and the rest of my environment to replicate the functionality I need from VS Code, but it didn't take nearly as long as I thought it would. It looks a little something like this.

![Screenshot of my terminal-only development setup](/images/rices/vim-only-pink.png)

I found that working this way wasn't faster in terms of code production or editing, but ended up being faster overall, because I spent less time flipping between and skimming files. Rather than typing code to get completion suggestions to refresh my memory (e.g., typing "CompareC", reading the suggested "CompareChartContainer", deleting the symbol, and then opening the appropriate file), I began simply pausing until the appropriate symbol name came to me.

I also noticed that a better mental representation of the overall project structure began to form. Using [CtrlP](https://github.com/kien/ctrlp.vim) rather than a tree-based representation of the file structure forced me to think linearly through the process of making each multi-file change, which in several cases reminded me of other changes I needed to make, or revealed a problem with my approach.

> ![CtrlP in action, completing file names in a fuzzy fashion](/images/rices/ctrl-p.png)
> CtrlP.vim finds files using fuzzy searching. It is very configurable; in this case, it's searching the whole repository but excluding the `node_modules` directory.

I also found that using [ripgrep](https://github.com/BurntSushi/ripgrep) was much faster and easier than using the Visual Studio Code search tool, even accounting for the time required to open files it finds with CtrlP.

> ![ripgrep finds references to CompareChart in my entire project](/images/rices/ripgrep.png)
> ripgrep finds references to CompareChart in my entire project, respecting the .gitignore file

I really enjoy working this way, and I think I'm going to keep it up, at least for a while. While it does feel pretty cool to get code completion, linting, and type checking in the same window as my code, it's even cooler to notice the ways my brain internalizes the structure of a project when I don't have an assistant like Visual Studio Code to help me.

Ultimately, a combination of the two is likely to be most efficient; if I ever figure out the perfect workflow, I'll make sure to write it up.

> NOTE: After publishing this article, I was made aware that the term "ricing" comes from the racing community, where it was originally coined as a pejorative racist term referring to over-modded Japanese motorcycles. Yikes. Because of that, I've decided to use the term "modding" instead.
