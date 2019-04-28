---
date: 2017-11-16 18:29:37+00:00
slug: an-intro-to-x86_64-reverse-engineering
title: An Intro to x86_64 Reverse Engineering
categories:
- Programming
- Hacking
- Assembly
description: A thorough introduction to understanding the radare reverse engineering toolkit, demonstrated by taking apart some programs compiled for Intel x86_64, a.k.a. AMD64, under Linux.
---

This document presents an introduction to x86\_64 binary reverse engineering, the process of determining the operation of a compiled computer program without access to its source code, through a series of CrackMe programs. 

There are a lot of excellent tutorials out there, but they mostly focus on the 32-bit x86 platform. Modern computers are, almost without exception, 64-bit capable, so this tutorial introduces 64-bit concepts immediately.

A CrackMe is an executable file which takes (typically) a single argument, does some check on it, and returns a message informing the user if it's correct or not.  The challenge is to determine the correct argument _without_ looking at the source code. Here, I present some CrackMe programs which I wrote, and demonstrate how to arrive at their solutions. 

If you enjoy this tutorial, please consider contributing to my [Patreon](https://www.patreon.com/noracodes) so I can make more like it.

# Prerequisites 

## Knowledge

This tutorial assumes a reasonable familiarity with programming, but not any particular skill with assembly, CPU architecture, or even C programming specifically. **You should know what a compiler does**, but you don't have to know how to implement one. Similarly, **you should know what a register is**, but you don't need to have the x86 registers or instructions memorized. I certainly don't.

If you are a fluent programmer but don't know assembly, I suggest you look at the [x86 Crash Course](https://www.youtube.com/watch?v=75gBFiFtAb8). It's a 10 minute video that should give you the background you need to understand this tutorial.

## The CrackMe Programs

You can find the CrackMes discussed here on [GitHub](https://github.com/noracodes/crackmes). Clone that repository and, _without looking at the source code_, build each CrackMe with `make crackme01`, `make crackme02`, etc.

## Tools and Software

These CrackMes only work on Unix systems, and I wrote this tutorial using Linux. You need the essentials of a development environment installed - a C compiler (`gcc`), object inspection utilities (`objdump`, `objcopy`, `xxd`), et cetera. This tutorial will also show you how to use the program [Radare2](http://radare.org/r/pics.html), an advanced open-source reverse engineering toolkit. On Debian-derived systems, the following command should get you set up:

`sudo apt install build-essential gcc xxd binutils`

You can install Radare2 from [here](https://github.com/radare/radare2).

On other systems, install the equivalent packages from your package manager.

# The Solutions

> **Note:** In later solutions, I discuss file offsets. These may be different for you, but I never use them without discussing where I got them, so if you're confused, just search for the offset using Ctrl+F or Find In Page and you should see how I got them quite easily.

## crackme01.c

`crackme01.64` is a relatively simple program. When you run it, you'll see this output:

<pre><code>
$ ./crackme01.64
Need exactly one argument.
</code></pre>

Provide some random argument. I used `lmao`:
<pre><code>
$ ./crackme01.64 lmao
No, lmao is not correct.
</code></pre>

This is as expected. We don't know the password. The first thing to try, when faced with a problem such as this, is to think about what the program is doing. The simplest way to check a string is to simply compare it against another string, stored in the binary. The binary may appear opaque to us, but in fact it is not. It is a file full of data like any other; it's just structured in a special way.

> **Try It Yourself:** Try examining the executable file with `cat`, `less`, or your favorite text editor.

If we simply `cat` it, we get gibberish. Luckily, there is a standard Unix tool called `strings` which will try to extract all the valid pieces of text (printable characters followed by the null character) in a given file.

<pre><code>
$ strings ./crackme01.64
/lib/ld-linux.so.2
WXZd
libc.so.6
_IO_stdin_used
__printf_chk
puts
__cxa_finalize
__libc_start_main
_ITM_deregisterTMCloneTable
__gmon_start__
_Jv_RegisterClasses
_ITM_registerTMCloneTable
GLIBC_2.3.4


...

.dynamic
.data
.bss
.comment
.debug_aranges
.debug_info
.debug_abbrev
.debug_line
.debug_str
.debug_loc

</code></pre>

This is a _lot_ of output. We can learn some useful things from it, but for now, we're looking for a password. 

> **Try It Yourself:** Try to find the password in the output of `strings`. This is all you need to solve the challenge!

### Solution

In this case, it is enough to simply scroll through the listing. Eventually, you'll see these lines:

<pre><code>
...
[^_]
Need exactly one argument.
password1
No, %s is not correct.
Yes, %s is correct!
;*2$"
...
</code></pre>

You can see two strings we already know about: `Need exactly one argument.` and `No, %s is not correct.`. Note that `%s` is the control sequence that tells C's `printf` function to print a string, presumably the one we entered at the command line. 

Between these known strings is something rather suspicious looking. Let's try it:

<pre><code>
$ ./crackme01.64 password1
Yes, password1 is correct!
</code></pre>

Success! You'd be surprised how much useful knowledge can come from a simple invocation of `strings` on a binary.

> **Exercise**: There is a file called `crackme01e.c` which can be solved using this same technique. Compile and attempt to solve it, to cement your skills.

## crackme02.c

This CrackMe is a little more difficult. You can try the same procedure as above, but the password you uncover won't work!

> **Try It Yourself:** Try to figure out why this might be, without reading ahead.

We'll need to look at the actual behavior of the program using `objdump`. You may need to install it with your system's package manager.
`objdump` is an extremely powerful tool for examining binaries. 

A binary program like this is a sequence of machine instructions, represented as number. `objdump` allows us to disassemble these machine instructions and represent them as slightly more readable assembly mnemonics.

In this case, if we run `objdump -d crackme02.64 -Mintel | less`, we will get an assembly listing. I've piped it through `less` because it's very long.

The first line tells us what we're looking at: `crackme02.64:     file format elf64-x86-64`. It's a 64-bit ELF executable file, on the Intel x86_64 (that is, AMD64) CPU architecture. Following this line are a number of sections that look like this:

<pre><code>
Disassembly of section .init:

0000000000000590 <_init>:
 590:   48 83 ec 08             sub    rsp,0x8
 594:   48 8b 05 3d 0a 20 00    mov    rax,QWORD PTR [rip+0x200a3d]        # 200fd8 <__gmon_start__>
 59b:   48 85 c0                test   rax,rax
 59e:   74 02                   je     5a2 <_init+0x12>
 5a0:   ff d0                   call   rax
 5a2:   48 83 c4 08             add    rsp,0x8
 5a6:   c3                      ret 
 ...
</code></pre>

Most of these are inserted by the linker immediately after compilation, and so aren't associated with the the algorithm for checking the code. We can skip everything but the `.text` section. It begins like this:

<pre><code>
Disassembly of section .text:

00000000000005e0 <_start>:
 5e0:   31 ed                   xor    ebp,ebp
 5e2:   49 89 d1                mov    r9,rdx
 5e5:   5e                      pop    rsi
 5e6:   48 89 e2                mov    rdx,rsp
 5e9:   48 83 e4 f0             and    rsp,0xfffffffffffffff0
 5ed:   50                      push   rax
 5ee:   54                      push   rsp
 ...
</code></pre>

Again, this is a stub function inserted by the linker. We don't care about anything until the `main` function, so keep on scrolling until you see it.

<pre><code>
0000000000000710 <main>:
 710:   48 83 ec 08             sub    rsp,0x8
 714:   83 ff 02                cmp    edi,0x2
 717:   75 68                   jne    781 <main+0x71>
 719:   48 8b 56 08             mov    rdx,QWORD PTR [rsi+0x8]
 71d:   0f b6 02                movzx  eax,BYTE PTR [rdx]
 720:   84 c0                   test   al,al
 ...
</code></pre>

On the far left column, the addresses of each location are listed (in base 16). Just to the right are the raw machine code bytes, represented as hex pairs (pairs of base 16 digits). Finally, `objdump` generates and displays the equivalent assembly to the far right.

Let's start picking apart this program. First, we see `sub rsp,0x8`. This is moving the stack pointer down by 8, allocating space on the stack for 8 bytes worth of variables. Note that **we don't know anything about these variables yet**. These could be 8 `char`s, or a single pointer (remember, it's a 64-bit executable).

Moving on, there's a pretty standard jump-if condition:

<pre><code class="x86asm">
cmp    edi,0x2
jne    781
</code></pre>

If you don't know what these instructions do, you can look them up; in this case, we're comparing (`cmp`) the `edi` register to the hex number 2, and then jumping if it's not equal (`jne`).

So the question is, what's in that register? This a Linux x86_64 executable, so we can look up the calling convention ([Wikipedia](https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI) is your friend). It turns out that `edi` is the lower 32 bits of the Destination Index register, which is where the first argument to a function goes. If you recall how the `main` function is written in C, its signature is: `int main(int argc, char** argv)`. So this is the register holding the first argument: `argc`, the number of arguments to the program. 

### Finding String Literals

So that compare-and-jump is checking if there are exactly two arguments to the program. (Note: the first argument is the name of the program, so it's really checking if there's _one_ user-supplied argument.) If not, it jumps to another part of the main program, at 781:

<pre><code class="x86asm">
lea    rdi,[rip+0xbc]
call   5c0 <.plt.got>
mov    eax,0xffffffff
jmp    77c <main+0x6c>
</code></pre>

Here, we're loading the address (`lea`) of a value into `rdi` (if you remember, this is the first argument to a function) and then calling a function at address 5c0. Looking at the disassembly of that line, we see:

<pre><code>
5c0:   ff 25 02 0a 20 00       jmp    QWORD PTR [rip+0x200a02]        # 200fc8 <puts@GLIBC_2.2.5>
</code></pre>

`objdump` has helpfully annotated this instruction, telling us that it is jumping into the `libc` function `puts`. If you look up that function, you'll see that it takes a single argument: a pointer to a string, which is then printed to the console. So this block prints a string. But what string?

To answer that, we need to see what's being inserted into `rdi`. If we look at that instruction, it says: `lea rdi,[rip+0xbc]`. That means calculate the pointer to the position 0xbc ahead of the instruction pointer (which will be pointing at the next instruction) and store that address in `rdi`.

So whatever we're printing is 0xbc bytes ahead of this instruction. We can do the math ourselves: 0x788 (next instruction) + 0xbc (offset) = 0x845. 

We can use another standard Unix binary tool to view the raw data from a specific offset: `xxd`. In this case, let's issue 
`xxd -s 0x844 -l 0x40 crackme02.64`. `-s` is "seek" or "skip"; it makes the hexdump start at the offset we're interested in. `-l` is "length"; it makes the hexdump only 0x40 characters long, rather than the whole rest of the file. We see:

<pre><code>
$ xxd -s 0x844 -l 0x40 crackme02.64
00000844: 4e65 6564 2065 7861 6374 6c79 206f 6e65  Need exactly one
00000854: 2061 7267 756d 656e 742e 004e 6f2c 2025   argument..No, %
00000864: 7320 6973 206e 6f74 2063 6f72 7265 6374  s is not correct
00000874: 2e0a 0070 6173 7377 6f72 6431 0059 6573  ...password1.Yes
</code></pre>

So, now we know. This block prints a string reading "Need exactly one argument.", as you'd expect from looking at the program's behavior when too many or too few arguments are specified. 

### Basic Flow Analysis

The most important part of this block is the unconditional jump at the end, which goes to address 77c:

<pre><code class="x86asm">
add    rsp,0x8
ret 
</code></pre>

This block removes the local variables from the stack and returns. So, that's it. If there aren't exactly 2 strings provided to the binary - its own name and one command line argument - it will quit.

We can start to write this program out in C code:

<pre><code class="c">
int main(int argc, char** argv){
    if (argc != 2) {
        puts("Need exactly one argument.");
        return -1;
    }

    // Magic happens here
}
</code></pre>

To find out just _what_ magic happens in that lower part of the program, we need to look at the program's flow. Assuming the `argc` check succeeds ( the jump at 0x717 isn't taken), program execution proceeds into this block:

<pre><code class="x86asm">
mov    rdx,QWORD PTR [rsi+0x8]
movzx  eax,BYTE PTR [rdx]
test   al,al
je     761 <main+0x51>
</code></pre>

That first instruction moves the quadword (64-bit value) at the address `[rsi+0x8]` into `rdx`. What's in `rsi`, the full 64-bit Source Index register? Turns out, that's the _second_ argument in the Linux x86_64 calling convention. So in C, this is the value `argv + 8` or, because `argv` is of type `char**`, `argv[1]`.

The next instruction moves and extends with zeroes (`movzx`) a single byte from the memory address stored in `rdx`; in other words, `*argv[1]`, or `argv[1][0]`. The Accumulator register now has all zeroes except for the last 8 bits, which contain the first byte of `argv[1]`, the the command line argument to the program.

`test al,al` is equivalent to `cmp al, 0`. `al` is the lower 8 bits of the Accumulator register. Essentially, this block is equivalent to the C code:

<pre><code class="c">
if (argv[1][0] == 0) {
    // do something
}
</code></pre>

So, what's at address 0x761? It's the following block:

<pre><code class="x86asm">
lea    rsi,[rip+0x119]        # 881 <_IO_stdin_used+0x41>
mov    edi,0x1
mov    eax,0x0
call   5c8 <.plt.got+0x8>
mov    eax,0x0
add    rsp,0x8
ret    
</code></pre>

One of the most important skills for a reverse engineer is noticing patterns, and you should be seeing one right away. Here, the program `lea`s a relative offset from the instruction pointer into `rsi` and then calls a function.

That function is, according to the same technique used above, `printf`. `printf` takes a format string and a variable number of arguments. All variadic functions need the Accumulator register to hold a value telling them how many arguments to look for in the FPU registers (in this case, none, as we can see from the `mov eax, 0x0` instruction). The `rdx` register already holds the pointer `argv[1]`, so that's the second argument.

So what's the format string? We use the same technique as before, but this time I didn't take out the comment that `objdump` added in where it did the math for us.

So, running `xxd -s 0x881 -l 0x40 crackme02.64` gives us the answer: the format string here is `Yes, %s is correct!`. This looks promising!
Additionally, we can see that, after the function call (at 0x77c; this is a useful address to remember), the space for local variables is removed from the stack, and the function returns. The return value is always placed in the Accumulator, so here the program returns a zero; success!

So, our C code looks like this:

<pre><code class="c">
int main(int argc, char** argv){
    if (argc != 2) {
        puts("Need exactly one argument.");
        return -1;
    }

    if (argv[1][0] == 0) {
        printf("Yes, %s is correct.", argv[1]);
    }

    // Magic happens here
}
</code></pre>

All we have to to is provide a string whose first byte is 0; that is, the empty string: 

<code><pre>
$ ./crackme02.64 ""
Yes,  is correct!
</code></pre>

In a certain sense, we're done, but it is instructive to look at the remainder of the code. Let's move on.

If that check fails, the code goes to this block (at 0x724):

<pre><code class="x86asm">
cmp    al,0x6f
jne    794 <main+0x84>
</code></pre>

Recall that, since we're working from the assumption of that jump to success not being taken, `al` has `argv[1][0]` in it still. Here, we're checking if it's not equal to 0x6f (111 in decimal; 'o' in ASCII). If so, we jump to address 0x794. 

<pre><code class="x86asm">
lea    rsi,[rip+0xc4]        # 85f <_IO_stdin_used+0x1f>
mov    edi,0x1
mov    eax,0x0
call   5c8 <.plt.got+0x8>
mov    eax,0x1
jmp    77c <main+0x6c>

</code></pre>

This is another print-and-return block; that unconditional jump (`jmp`) at the end jumps to 0x77c, where the program removes its stack space for local variables and returns. 

Rather than printing the success message, this block prints "No, %s is not correct." formatted with the command line argument, and then returns a failure code (1). **We now know, for sure, that the correct message starts with the letter 'o'**, because without it, there's an automatic fail condition.

<pre><code class="c">
int main(int argc, char** argv){
    if (argc != 2) {
        puts("Need exactly one argument.");
        return -1;
    }

    if (argv[1][0] == 0) {
        printf("Yes, %s is correct.", argv[1]);
        return 0;
    }

    if (argv[1][0] != 'o') {
        printf("No, %s is not correct.", argv[1]);
        return 1;
    }

    // Magic happens here
}
</code></pre>

Assuming that jump is _not_ taken, we go on to this code, at 0x728:

<pre><code class="x86asm">
mov    esi,0x1
mov    eax,0x61
mov    ecx,0x1
lea    rdi,[rip+0x139]        # 877 <_IO_stdin_used+0x37>
movzx  ecx,BYTE PTR [rdx+rcx*1]
test   cl,cl
je     761 <main+0x51>
</code></pre>

Here, we load up some registers with constants, and then load a pointer into `rdi`. That pointer is to the string "password1". But we know this isn't the right password. So what's going on?

The next instruction places the byte at the address `rdx + rcx`. What's in `rdx`? Well, if we backtrack to 0x719, we see that it was loaded with the value at `rsi + 0x8`; a.k.a. `argv[1]`. So we're indexing into that string here; `ecx` = `argv[1][1]`.

Again, the most important skill for reverse engineering is recognizing patterns. Here's a common one we saw above: `test`ing a register against itself, followed by a `je`, means "jump if that register is zero".

So, if there's a zero at `argv[1][1]`, we jump to 0x761. Where is that? It's the block we just reversed above; it prints the success string and exits with the return code of 0. Our pseudocode looks like this:

<pre><code class="c">
int main(int argc, char** argv){
    if (argc != 2) {
        puts("Need exactly one argument.");
        return -1;
    }

    if (argv[1][0] == 0 || argv[1][1] == 0) {
        printf("Yes, %s is correct.", argv[1]);
        return 0;
    }

    if (argv[1][0] != 'o') {
        printf("No, %s is not correct.", argv[1]);
        return 1;
    }

    // Magic happens here
}
</code></pre>


What happens if it's not zero, though? The program progresses down to 0x746.

<pre><code class="x86asm">
movsx  eax,al
sub    eax,0x1
movsx  ecx,cl
cmp    eax,ecx
jne    794 <main+0x84>
</code></pre>

Here, we zero out all but the lowest 8 bits of `eax`, then subtract one from it. Then we zero out all but the last 8 bits of `ecx` and compare `eax` to `ecx`. If they're not equal, we jump to 0x794. Where is that? It's another block we've already reversed; it prints the failure string and exits with the return code of 1.

What is this actually doing? From above, `eax` contains a single byte; 0x61 (decimal 97, or 'a' in ASCII). It has one subtracted from it, so it's 0x60 (decimal 96, '\`' in ASCII). So we know that the first two characters of the key are 'o\`'. Our pseudocode looks like this:

<pre><code class="c">
int main(int argc, char** argv){
    if (argc != 2) {
        puts("Need exactly one argument.");
        return -1;
    }

    if (argv[1][0] == 0 || argv[1][1] == 0) {
        printf("Yes, %s is correct.", argv[1]);
        return 0;
    }

    if (argv[1][0] != 'o' || argv[1][1] != 0x60) {
        printf("No, %s is not correct.", argv[1]);
        return 1;
    }

    // Magic happens here
}
</code></pre>

If they are equal, we move on to 0x753:

<pre><code class="x86asm">
add    esi,0x1
movsxd rcx,esi
movzx  eax,BYTE PTR [rdi+rcx*1]
test   al,al
jne    73e <main+0x2e>
</code></pre>

First off, the program increments `esi`. (`esi` was set to 1 in the previous block.) It then moves that value into the lower 32 bits of `rcx`.

Then, the program loads a single byte from `rdi + rcx`. `rdi` is `argv[1]`, and `rcx` is `esi + 1` (which is, at this moment, 2). So here, the program is loading `argv[1][2]`. More correctly, though, it's loading `argv[1][rcx]` (you'll see why this matters in a moment).

The program then checks if that value is zero; if not, it jumps to 0x73e. Where is that?

<pre><code class="x86asm">
movzx  ecx,BYTE PTR [rdx+rcx*1]
test   cl,cl
je     761 <main+0x51>
</code></pre>

We've seen this before. It's just the check from a few sections above. It loads a byte from `argv[1][ecx]` and checks if it's zero; if so it jumps to success, and if not it moves down to the code we just reversed. This is another pattern you should recognize in the future: **a loop**.

Now that we've identified the whole loop, let's look at all of its instructions, from 0x73e to 0x75f.

Recall that, a few blocks ago, `rdi` was loaded with the address of the string `password1`, but this wasn't the right password. Here we can see why; bytes are being loaded from that string and having one added to them before they're compared with the actual input.

<pre><code class="x86asm">
movzx  ecx,BYTE PTR [rdx+rcx*1] ; load a byte from argv[1]
test   cl,cl                    ; check if that byte is zero
je     761 <main+0x51>          ; if so, jump to success
movsx  eax,al                   
sub    eax,0x1                  ; decrement comparison byte
movsx  ecx,cl                   
cmp    eax,ecx                  ; check if the correct byte == the input byte
jne    794 <main+0x84>          ; if it doesn't match, jump to failure
add    esi,0x1                  ; increment index into comparison string
movsxd rcx,esi                  ; place that index in CX
movzx  eax,BYTE PTR [rdi+rcx*1] ; load the next byte from the comparison string
test   al,al                    ; Check that that byte isn't zero
jne    73e <main+0x2e>          ; If it's not zero, loop
</code></pre>

While we as humans would write this like the following C code, the compiler actually moved the second part of the loop check to the _end_ of the loop, and loads the next byte for the comparison string there.

Note: This code was incorrect in the original version of this tutorial. Thanks to [empwill](https://github.com/empwill/) for correcting it. I note this so that readers will be aware that even experienced reverse engineers make mistakes, and that such mistakes are both expected and recoverable!

> **Try It Yourself:** The following C code should be all you need to determine the correct code. Try it!

<pre><code class="c">
int main(int argc, char** argv){
    if (argc != 2) {
        puts("Need exactly one argument.");
        return -1;
    }
    // This pointer is in rdi in our disassembled binary
    char* comparison = "password1";
    // This is the value used to index the argv[1]
    int i = 0;

    while (argv[1][i] != 0 && (comparison[i]) != 0) {
        if (argv[1][i] != comparison[i] - 1) {
            printf("No, %s is not correct.", argv[1]);
            return 1;
        }
        i++;
    }

    printf("Yes, %s is correct.", argv[1]);
    return 0;
}
</code></pre>

This is really all we need. Simply subtracting one from each letter of `password1` in ASCII gives us "o\`rrvnqc0". Let's try it:

<pre><code>
$ ./crackme02.64 o\`rrvnqc0
Yes, o`rrvnqc0 is correct!
</code></pre>

The sharp-eyed among you might have noticed that this binary has a problem, however; it will actually accept _any_ of these characters. o, o\`, o\`r, o\`rr, et cetera all work! Clearly not a very good method to use for your product key. Also, as pointed out by [an astute GitHub commenter](https://github.com/noracodes/crackmes/issues/2), an empty password (`./crackme02.64 ""`) will be accepted.

If you made it this far, good job! Reverse engineering is difficult, but this is the core of it, and it only gets easier from here.

> **Exercise**: There is a file called `crackme02e.c` which can be solved using this same technique. Compile and attempt to solve it, to cement your skills.

## crackme03.c

The next crackme is slightly more difficult. In `crackme02`, we were able to manually inspect each branch, building up the flow of execution mentally. This technique breaks down as programs become more complex.

### The Radare Analysis Tool

Fortunately, the reverse engineering community is rife with smart people, and there are excellent tools to automate a great deal of this analysis. Some of them, like Ida Pro, cost as much as $5000, but my personal favorite is Radare2 (**Ra**ndom **da**ta **re**covery), which is entirely free and open source.

Running `crackme03.64`, we can see that it (basically) works about the same way as the other two. It needs exactly one argument, and when we provide it with one, it helpfully tells us that it's wrong.

This time, rather than `objdump`ing the file, we're going to open it with `radare2` (or `r2`): `r2 ./crackme03.64`. You'll see a prompt. Type "?", and you'll get a help listing. Radare is an immensely powerful tool, but for this challenge we won't need most of its functionality. I've removed many entries, paring it down to the bare necessities.

<pre><code>
[0x000005e0]> ?
Usage: [.][times][cmd][~grep][@[@iter]addr!size][|>pipe] ; ...
Append '?' to any char command to get detailed help
Prefix with number to repeat command N times (f.ex: 3x)
| a[?]                    Analysis commands
| p[?] [len]              Print current block with format and length
| s[?] [addr]             Seek to address (also for '0x', '0x1' == 's 0x1')
| V                       Enter visual mode (V! = panels, VV = fcngraph, VVV = callgraph)
</code></pre>

The important thing to note is that Radare is **self-documenting**. If you ever want to know what you can do with a command, simply type a ? after it. For instance, we want to `a`nalyze the current program:

<pre><code>
[0x000005e0]> a?
|Usage: a[abdefFghoprxstc] [...]
| ab [hexpairs]    analyze bytes
| aa[?]            analyze all (fcns + bbs) (aa0 to avoid sub renaming)
| ac[?] [cycles]   analyze which op could be executed in [cycles]
| ad[?]            analyze data trampoline (wip)
| ad [from] [to]   analyze data pointers to (from-to)
| ae[?] [expr]     analyze opcode eval expression (see ao)
| af[?]            analyze Functions
| aF               same as above, but using anal.depth=1
| ag[?] [options]  output Graphviz code
| ah[?]            analysis hints (force opcode size, ...)
| ai [addr]        address information (show perms, stack, heap, ...)
| ao[?] [len]      analyze Opcodes (or emulate it)
| aO               Analyze N instructions in M bytes
| ar[?]            like 'dr' but for the esil vm. (registers)
| ap               find prelude for current offset
| ax[?]            manage refs/xrefs (see also afx?)
| as[?] [num]      analyze syscall using dbg.reg
| at[?] [.]        analyze execution traces
Examples:
 f ts @ `S*~text:0[3]`; f t @ section..text
 f ds @ `S*~data:0[3]`; f d @ section..data
 .ad t t+ts @ d:ds
</code></pre>

> **Try It Yourself:** I suggest traversing the help for a while. Google every term you don't understand. There is a lot of cool functionality that I won't touch on here, but which might inspire you to try something.

### Automatic Analysis

It turns out that the command we want it `aaaa`: `a`nalyze using `a`ll functions using `a`ll normal and `a`ll experimental techniques.

This means that Radare has a list of functions available to us. We can view it with `afl`: `a`nalyze `f`unctions, displaying a `l`ist.

<pre><code>
[0x000005e0]> afl
0x00000000    3 73   -> 75   fcn.rsp
0x00000049    1 219          fcn.00000049
0x00000590    3 23           sym._init
0x000005c0    1 8            sym.imp.puts
0x000005c8    1 8            sym.imp.__printf_chk
0x000005d0    1 16           sym.imp.__cxa_finalize
0x000005e0    1 43           entry0
0x00000610    4 50   -> 44   sym.deregister_tm_clones
0x00000650    4 66   -> 57   sym.register_tm_clones
0x000006a0    5 50           sym.__do_global_dtors_aux
0x000006e0    4 48   -> 42   entry1.init
0x00000710    7 58           sym.check_pw
0x0000074a    7 203          main
0x00000820    4 101          sym.__libc_csu_init
0x00000890    1 2            sym.__libc_csu_fini
0x00000894    1 9            sym._fini
</code></pre>

We only care about `main` and `check_pw`.

> **Try It Yourself:** Figure out why I can immediately discard the other functions for initial analysis. Search engines are your friend.

Radare can disassemble just a single function for us with `pdf@main`: `p`rint `d`isassembly of a `f`unction `@` (at) the symbol called `main`. Radare also supports tab completion in many contexts. For instance, if you type `pdf@sym.` and hit the tab key, you'll get a list of all the functions in the symbol table.

Anyway, the first thing to notice is that Radare does syntax highlighting, adds a lot of comments, and even names some variables for us. It does some analysis to determine the types of variables, too; in this case, we have 9 local stack variables. Radare names them `local_2h`, `local_3h`, et cetera based on their offsets from the stack pointer.

The beginning of our program is pretty familiar. Starting at 0x74a:

<pre><code class="x86asm">
push rbx
sub rsp, 0x10
cmp edi, 2
jne 0x7cc
</code></pre>

We have the function prologue allocating 16 bytes of memory for our local variables and an `if` statement. Recall that the DI register holds the first argument, and since this is `main`, that argument is `argc`. So, `if (argc != 2) jump somewhere`.

In Radare, look to the left of the `jne` instruction. You'll see an arrow coming out of that instruction and running down to 0x7cc, where we see:

<pre><code class="x86asm">
lea rdi, str.Need_exactly_one_argument. ; 0x8a4 ; "Need exactly one argument." ; const char * s
call sym.imp.puts           ; int puts(const char *s)
mov eax, 0xffffffff         ; -1
jmp 0x7c6
</code></pre>

Remember how annoying it was to search for string literals in our binary? Radare does it for us, giving us the address, a convenient alias, and the value of the string literal. 
It also analyzes the functions being called, which is very convenient. Here, we can see without much effort that the binary is printing the string "Need exactly one argument.".

It then loads `eax` with -1 and jumps to 0x7c6. We can follow the arrow (or just scroll to the address) to see where that is, but there's a more interesting way.

### Visual Flow Analysis

Radare provides a feature known as "visual mode". To use it, we need to move Radare's internal cursor to the function we want to analyze with the `s`eek command: `s main`. You'll notice that the prompt changes from `[0x000005e0]>` to `[0x0000074a]>`, indicating that the current location has moved to the first instruction in the `main` function. Then, type `VV` (`V`isual mode 2). You should see ASCII-art boxes with different parts of the program.

Every time a jump instruction appears, the block ends and lines come out, pointing to other blocks. For instance, in the top block (the beginning of the function), the `jne` instruction which checks for the argument number causes a red line to come out to the left and a green one to the right. 

On the right, you should see a block that looks a bit like this:

<pre><code>
.---------------------------------------------.                                
|  0x7cc ;[ga]                                |                                
|      ; const char * s                       |                                
|      ; 0x8a4                                |                                
|      ; "Need exactly one argument."         |                                
| lea rdi, str.Need_exactly_one_argument.     |                                
| call sym.imp.puts;[gh]                      |                                
|    ; -1                                     |                                
| mov eax, 0xffffffff                         |                                
| jmp 0x7c6;[gg]                              |                                
`---------------------------------------------'
</code></pre>

That's the block we just analyzed. Use the arrow keys to follow the blue (unconditional) line down to see what happens after this block. You'll see, at the bottom of the graph, a block at 0x7c6 that is unconditionally jumped to from a number of places in the program:

<pre><code class="x86asm">
add rsp, 0x10                                                      
pop rbx                                                 
ret
</code></pre>

This simply frees stack space and returns. So this program behaves like the others we've looked at: if there aren't the right number of arguments, it prints a string and exits with an error code (remember, `eax` was loaded with -1).

> **Try It Yourself:** Look at the rest of the program in the control flow graph. Try to find the block that prints the failure message. There are two checks that can lead there; can you figure out what they do? 
>
> Recall that `test eax, eax` followed by `je` means "jump if `eax` is zero", and that the x86 instruction set is well documented; if you don't know what an instruction does, look it up!

If we progress down the red branch from the first block, where execution flows if the `jne` isn't taken (that is, if there are exactly 2 strings passed to the binary), you'll see these instructions at 0x754:

<pre><code class="x86asm">
mov dword [local_9h], 0x426d416c ; [0x426d416c:4]=-1
mov word [local_dh], 0x4164 ; [0x4164:2]=0xffff
mov byte [local_fh], 0
mov word [local_6h], 0
mov byte [local_8h], 0
mov byte [local_2h], 2
mov byte [local_3h], 3
mov byte [local_4h], 2
mov byte [local_5h], 3
mov byte [local_6h], 5
mov rbx, qword [rsi + 8]    ; [0x8:8]=0
mov eax, 0
mov rcx, 0xffffffffffffffff
mov rdi, rbx
repne scasb al, byte [rdi]
cmp rcx, 0xfffffffffffffff8
je 0x7df
</code></pre>

Mostly all this block does is load a bunch of values into memory. Here, rather than showing the actual addresses, Radare has named each local variable based on its stack offset. Scrolling up to the initial block, we can see that `local_2h` through `local_fh` are all of type `int` (or, at least, that's what Radare thinks) and they're each one byte in size.

After loading those values into local variables, it loads something in memory at the address `rsi + 8` into `rbx`. If we recall the SystemV x86_64 calling convention, `rsi` is the second argument: `argv`. So `rsi + 8` is `argv[1]`. It then loads up `eax` with 0, `rcx` with `0xffffffffffffffff` , and `rdi` with `rbx`, the value just loaded from `argv[1]`.

Then it runs `repne scasb`. This is a weird but fast quirk of x86: it has a _native instruction_ for string length determination. `repne` means `rep`eat while `n`ot `e`qual, and `scasb` means `s`tring `c`ompare `a`nd `s`ubtract, `b`yte variant - see the reference [here](http://www.felixcloutier.com/x86/SCAS:SCASB:SCASW:SCASD.html) for more info. 

So, this instruction compares bytes to the value of `al` (which is zero here), starting at the memory address in `rdi` and counting up, while subtracting from `rcx` (remember, C is the counter register). In essence, this instruction is measuring the length of a string. Isn't x86 a fun time?

Anyway, once the `repne scasb` operation is done, `rcx` will hold 0xffffffffffffffff minus the length of the string, and we can see that the next instruction compares it to 0xfffffffffffffff8. Therefore, if the string is 0xffffffffffffffff - 0xfffffffffffffff8 = 7 bytes long (note that this includes the terminating character), the jump is taken; otherwise it is not.

If the jump isn't taken, the code proceeds to a block at 0x7a8, where the failure string is printed. Therefore, we can immediately determine that any correct passcode will be precisely six bytes (plus the null terminator).

### Functions

What is more interesting is the block that is executed when the jump _is_ taken.

<pre><code class="x86asm">
lea rdx, [local_2h]
lea rsi, [local_9h]
mov rdi, rbx
call sym.check_pw
test eax, eax
je 0x7a8
</code></pre>

The binary loads the addresses of some local variables, loads `argv[1]` again (`rbx`, remember?), and then `call`s a function: `sym.check_pw`. Of course, the actual binary just has the offset of the function, but Radare was smart enough to look up that offset in the symbol table and put the name in for us. `check_pw` sounds pretty promising, as cunction names go, and we can verify that by continuing: after the call, the program jumps to failure if the function returned zero, and continues on to success if it did not (recall that `test eax, eax` followed by `je` jumps if `eax` is zero).

So what exactly does this function do? First, recall that the SystemV x86_64 calling convention says that `rdi`, `rsi`, and `rdx` (the three registers loaded prior to the call) are the first three arguments to the function. So in C, the call looks like this:

<pre><code class="c">
int result = check_pw(argv[1], &local_9h, &local_2h);
if (result == 0) {
    // fail
} else {
    // succeed
}
</code></pre>

The question, then, is what, exactly, does `check_pw` do? To find that out, we need to exit visual mode (`q` followed by `q`) and `s`eek to it (`s sym.check_pw`), then look at the flow diagram (`VV`).

It is immediately clear that this function contains a loop. Unlike the main function, which continues consistently downward no matter which jumps you take, in `check_pw` a block near the bottom has a `jne` that jumps up to the top. Looking a little more closely, we can see that there are three opportunities to return. One of them (at 0x73e) returns 0 (failure) and the other two (at 0x744 and 0x748) return 1 (success).

This kind of high-level analysis is only possible with a flow diagram, and is one of the major advantages of using a tool like Radare. When I was getting started  with reverse engineering, I drew out flow diagrams by hand, simply because I was unaware that free tools existed. Don't do that; it's a waste of time.

The function starts off by loading `r8d`, a 64-bit general purpouse register, with the value 0. It then jumps to the following block (at 0x716):

<pre><code class="x86asm">
movsxd rax, r8d
movzx ecx, byte [rdx + rax]
add cl, byte [rsi + rax]
cmp cl, byte [rdi + rax]
jne 0x73e;[gb]
</code></pre>

This block sets `rax` to `r8d` (which we know is zero), then loads a single byte from its third argument, indexed by `eax`. Going back to our arg list, this argument is `&local_2h`, so it's loading `(&local_2h)[0]`. 

It then adds a byte loaded from the second argument indexed by `eax` (`(&local_9h)[0]`), and compares it to a byte loaded from its first argument indexed by `eax` (`argv[1][0]`). Remember that this is a loop, so eax will change. In other words:

<pre><code class="c">
while (/* something?? */) {
    char temp  = arg3[eax] + arg2[eax];
    if (temp != arg1[eax]) {
        return 0; // failure
    }
}
</code></pre>

If the jump isn't taken, this code is run (at 0x725):

<pre><code class="x86asm">
add r8d, 1 
movsxd rax, r8d
cmp byte [rsi + rax], 0
je 0x744;[gd] 
</code></pre>

This increments the loop counter, then checks if the second argument indexed by the loop counter is zero. If so, it jumps to code (at 0x744) that returns success. Otherwise, it continues looping. Our updated C code looks like this:

<pre><code class="c">
while (arg2[eax] != 0) {
    char temp  = arg3[eax] + arg2[eax];
    if (temp != arg1[eax]) {
        return 0; // failure
    }

    eax++;
}

return 1;
</code></pre>

At this point, it's pretty easy to see what `check_pw` is doing: it's comparing two strings, but it's modifying each byte of one of the strings.

Looking at the arguments passed in `main`, we can see that the program is adding `(&local_2h)[eax]` to `(&local_9h)[eax]`. I suggest going back to the main function (exit visual mode; `pdf@main`) to look at what each of those values will be.

Both of these are just locations on the stack. We also know that `check_pw` will only be run on a string with 6 characters in it, so we only need to look at 6 values. Here are the values after `local_2h` (you can see them being set in `main`): 2, 3, 2, 3, 5. That's only 5 values. What's going on?

If we look again, the stack variables are set starting at offset 0x754:

<pre><code class="x86asm">
mov dword [local_9h], 0x426d416c ; [0x426d416c:4]=-1
mov word [local_dh], 0x4164 ; [0x4164:2]=0xffff
mov byte [local_fh], 0
mov word [local_6h], 0
mov byte [local_8h], 0
mov byte [local_2h], 2
mov byte [local_3h], 3
mov byte [local_4h], 2
mov byte [local_5h], 3
mov byte [local_6h], 5
</code></pre>

Prior to the values being set in order from `local_2h` to `local_6h`, by moving byte-sized values into them, `local_6h` (that is, `rsp+0x6`) is loaded with a word-sized 0 (this is Intel-syntax, so a word is 16 bits; see [this](https://en.wikipedia.org/wiki/Word_(computer_architecture)#Size_families) historical note) value. That means that **both `rsp+0x6` and `rsp+0x7`** are set to zero.

Note that Radare didn't even realize that these values were in an array, let alone tell us what it was initialized to, despite it being entirely static. This is the part of reverse engineering that requires a human brain; the computer knows what's there, but it can't know what it's being used for.

Anyway, our table of values starting at `local_2h` is `[2, 3, 2, 3, 5, 0]`. These aren't printable ASCII characters, so presumably the hard-coded password is stored at the other input value: `local_9h`. 

The `mov` instruction above is sized for a double word (`dword`); a 32-bit value. It's followed by a word-sized value, then a byte-sized zero. That works out to 4+2 = 6 bytes, plus a null terminator, so it's a good bet that these three locations together form a string. Indeed, if we write out the values with byte seperation, it makes sense: `42 6d 41 6c 41 64 00` is a well-formed null-terminated string, with values in the printable ASCII range.

All that's left is to add the offsets to them, giving us `44 70 43 6e 44 64 00`. Translating these to ASCII, we get: `DpCnDd`.

Sure enough, putting this into the binary gives us... the failure message. What happened?

> **Try It Yourself:** Why didn't this work? It's something pretty fundamental about how x86 processors organize data.

What's happening here is that x86 processors are little-Endian. That means that bytes are read from the right to the left, not the other way around, in multi-byte values. This is easily corrected by just flipping the order of `local_9h` and `local_dh`. `42 6d 41 6c` becomes `6c 41 6d 42` and `41 64` becomes `64 41`, making our whole string `6c 41 6d 42 64 41 00` and our correct string `6e 44 6f 45 69 41 00` or `nDoEiA`.

Congratulations on making it through this part of the tutorial. You've now got all the tools you need for static reverse engineering! Don't forget to solve the exercises to cement your skills.

## crackme04.c

Now that you've seen all the tools and techniques you need to reverse engineer these crackmes, I'm only going to highlight the most important sections of each crackme. With crackme04, you can use the same basic process as before: open it up in Radare, run analysis, and seek to the main function. The flow graph should lead you right to a loop at the heart of the code, which looks like this:

<pre><code class="x86asm">
movsx eax, al
add esi, eax
add ecx, 1
movsxd rax, ecx
movzx eax, byte [rdx + rax]
test al, al
jne 0x72e;[ge]  
</code></pre>

That jump is either back to the top if `al` (a single byte from the input string) isn't zero, or to a block that just compares `ecx` with 0x10 and exits with failure if it's not equal, and to another check if it is. That check jumps to failure if `esi` isn't 0x6e2, and success if it is.

So what are `ecx` and `esi`? It's easy to see that `ecx` is just a counter; at each loop iteration, it gets incremented, and is used to index the input string. So, after the loop is done, it's equal to the number of non-zero bytes in the string.

`esi` is only modified in one line in the loop: it's the sum of the numerical values of the characters in the string. It's later compared to 0x632. So we need a 16-character string with the sum 0x6e2 (1762). 

My method is to simply divide and then add in the remainder. 1913 over 16 is 110 remainder 2, so we just use character 110 ('n') followed by a single 112 ('p'): `nnnnnnnnnnnnnnnp`. 

# Appendix

## The Makefile

The Makefile used is fairly straightforward, but it does have a few quirks. Prime among them is the use of `objcopy` on the compiled executables. I use it to strip out the `FILE` symbol, which would otherwise be used by Radare to display source code listings alongside the disassembly, completely defeating the purpouse of the exercise.

## Exercises

The files named `crackme01e.c`, `crackme02e.c`, et cetera are modified versions of their non-suffixed counterparts. They are intended as exercises, and can be solved with exactly the same techniques as are shown in the respective sections of this tutorial. You will have a much better experience if you solve them before moving on.

## Media

Saturday, Jan. 6, 2018: This tutorial was featured on Hackaday, which briefly brought down my server. Hi to all of you who make it here from there. Please consider checking out my other tutorials and, if you want more content like this to be created, contributing to my [Patreon](https://noracodes.com).
