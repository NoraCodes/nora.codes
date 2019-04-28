---
date: 2016-09-13 01:55:38+00:00
slug: am-i-in-a-terminal
title: Am I in a Terminal?
categories:
- Programming
- Python
description: A useful code for determining if a Python program is running in an interactive terminal or not.
---

Sometimes, it can be useful to know if your program is running in a terminal. Since Python 3.3, this functionality has been available in the `os` module:

<pre><code class="py">
#!/usr/bin/env python3

# Test if this Python script is running in a terminal or not.

import os

try:
    size = os.get_terminal_size()
    print("I am in a terminal of size {}x{}"
        .format(size[0], size[1]))
except OSError:
    print("I am not in a terminal.")
</code></pre>

Here is an example of it in operation:

<pre><code class="py">
$ ./am_i_term.py 
I am in a terminal of size 80x24

$ ./am_i_term.py | tee
I am not in a terminal.
</code></pre>

This is useful for many reasons. For example, scripts which have interactive "beautifications" like progress bars, no-freeze spinners, and animations should cease these antics when piped into the input of other programs or redirected to files. Additionally, programs being run from scripts can disable all performance-impacting interactivity, including interactive `KeyboardInterrupt` handling; if a user `Ctrl+C`s a script, they want it to stop, immediately, not ask to quit.
