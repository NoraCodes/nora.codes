---
date: 2017-11-16 18:29:37+00:00
slug: an-intro-to-x86_64-reverse-engineering
title: An Intro to x86_64 Reverse Engineering
categories:
- Programming
- Hacking
tags:
- c
- programming
- hacking
- cracking
- reverse engineering
toc: true
---

This document presents an introduction to x86\_64 binary reverse engineering, the process of determining the operation of a compiled computer program without access to its source code, through a series of CrackMe programs. 

There are a lot of excellent tutorials out there, but they mostly focus on the 32-bit x86 platform. Modern computers are, almost without exception, 64-bit capable, so this tutorial introduces 64-bit concepts immediately.

A CrackMe is an executable file which takes (typically) a single argument, does some check on it, and returns a message informing the user if it's correct or not.  The challenge is to determine the correct argument _without_ looking at the source code. Here, I present some CrackMe programs which I wrote, and demonstrate how to arrive at their solutions. 

# Prerequisites 

## Knowledge

This tutorial assumes a reasonable familiarity with programming, but not any particular skill with assembly, CPU architecture, or even C programming specifically. **You should know what a compiler does**, but you don't have to know how to implement one. Similarly, **you should know what a register is**, but you don't need to have the x86 registers or instructions memorized. I certainly don't.

If you are a fluent programmer but don't know assembly, I suggest you look at the [x86 Crash Course](https://www.youtube.com/watch?v=75gBFiFtAb8). It's a 10 minute video that should give you the background you need to understand this tutorial.

## The CrackMe Programs

You can find the CrackMes discussed here on [GitHub](https://github.com/leotindall/crackmes). Clone that repository and, _without looking at the source code_, build each CrackMe with `make crackme01`, `make crackme02`, etc.

## Tools and Software

These CrackMes only work on Unix systems, and I wrote this tutorial using Linux. You need the essentials of a development environment installed - a C compiler (`gcc`), object inspection utilities (`objdump`, `objcopy`, `xxd`), et cetera. This tutorial will also show you how to use the program [Radare2](http://radare.org/r/pics.html), an advanced open-source reverse engineering toolkit. On Debian-derived systems, the following command should get you set up:

`sudo apt install build-essential gcc xxd binutils radare2`

On other systems, install the equivalent packages from your package manager.

# The Solutions

## crackme01.c

`crackme01.64` is a relatively simple program. When you run it, you'll see this output:

{{< highlight text >}}
$ ./crackme01.64
Need exactly one argument.
{{< /highlight >}}

Provide some random argument. I used `lmao`:
{{< highlight text >}}
$ ./crackme01.64 lmao
No, lmao is not correct.
{{< /highlight >}}

This is as expected. We don't know the password. The first thing to try, when faced with a problem such as this, is to think about what the program is doing. The simplest way to check a string is to simply compare it against another string, stored in the binary. The binary may appear opaque to us, but in fact it is not. It is a file full of data like any other; it's just structured in a special way.

> **Try It Yourself:** Try examining the executable file with `cat`, `less`, or your favorite text editor.

If we simply `cat` it, we get gibberish. Luckily, there is a standard Unix tool called `strings` which will try to extract all the valid pieces of text (printable characters followed by the null character) in a given file.

{{< highlight text >}}
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

{{< /highlight >}}

This is a _lot_ of output. We can learn some useful things from it, but for now, we're looking for a password. 

> **Try It Yourself:** Try to find the password in the output of `strings`. This is all you need to solve the challenge!

### Solution

In this case, it is enough to simply scroll through the listing. Eventually, you'll see these lines:

{{< highlight text >}}
...
[^_]
Need exactly one argument.
password1
No, %s is not correct.
Yes, %s is correct!
;*2$"
...
{{< /highlight >}}

You can see two strings we already know about: `Need exactly one argument.` and `No, %s is not correct.`. Note that `%s` is the control sequence that tells C's `printf` function to print a string, presumably the one we entered at the command line. 

Between these known strings is something rather suspicious looking. Let's try it:

{{< highlight text >}}
$ ./crackme01.64 password1
Yes, password1 is correct!
{{< /highlight >}}

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

{{< highlight text >}}
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
{{< /highlight >}}

Most of these are inserted by the linker immediately after compilation, and so aren't associated with the the algorithm for checking the code. We can skip everything but the `.text` section. It begins like this:

{{< highlight text >}}
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
{{< /highlight >}}

Again, this is a stub function inserted by the linker. We don't care about anything until the `main` function, so keep on scrolling until you see it.

{{< highlight text >}}
0000000000000710 <main>:
 710:   48 83 ec 08             sub    rsp,0x8
 714:   83 ff 02                cmp    edi,0x2
 717:   75 68                   jne    781 <main+0x71>
 719:   48 8b 56 08             mov    rdx,QWORD PTR [rsi+0x8]
 71d:   0f b6 02                movzx  eax,BYTE PTR [rdx]
 720:   84 c0                   test   al,al
 ...
{{< /highlight >}}

On the far left column, the addresses of each location are listed (in base 16). Just to the right are the raw machine code bytes, represented as hex pairs (pairs of base 16 digits). Finally, `objdump` generates and displays the equivalent assembly to the far right.

Let's start picking apart this program. First, we see `sub rsp,0x8`. This is moving the stack pointer down by 8, allocating space on the stack for 8 bytes worth of variables. Note that **we don't know anything about these variables yet**. These could be 8 `char`s, or a single pointer (remember, it's a 64-bit executable).

Moving on, there's a pretty standard jump-if condition:

{{< highlight asm >}}
cmp    edi,0x2
jne    781
{{< /highlight >}}

If you don't know what these instructions do, you can look them up; in this case, we're comparing (`cmp`) the `edi` register to the hex number 2, and then jumping if it's not equal (`jne`).

So the question is, what's in that register? This a Linux x86_64 executable, so we can look up the calling convention. It turns out that `edi` is the lower 32 bits of the Destination Index register, which is where the first argument to a function goes. If you recall how the `main` function is written in C, its signature is: `int main(int argc, char** argv)`. So this is the register holding the first argument: `argc`, the number of arguments to the program. 

### Finding String Literals

So that compare-and-jump is checking if there are exactly two arguments to the program. (Note: the first argument is the name of the program, so it's really checking if there's _one_ user-supplied argument.) If not, it jumps to another part of the main program, at 781:

{{< highlight asm >}}
lea    rdi,[rip+0xbc]
call   5c0 <.plt.got>
mov    eax,0xffffffff
jmp    77c <main+0x6c>
{{< /highlight >}}

Here, we're loading the address (`lea`) of a value into `rdi` (if you remember, this is the first argument to a function) and then calling a function at address 5c0. Looking at the disassembly of that line, we see:

{{< highlight text >}}
5c0:   ff 25 02 0a 20 00       jmp    QWORD PTR [rip+0x200a02]        # 200fc8 <puts@GLIBC_2.2.5>
{{< /highlight >}}

`objdump` has helpfully annotated this instruction, telling us that it is jumping into the `libc` function `puts`. If you look up that function, you'll see that it takes a single argument: a pointer to a string, which is then printed to the console. So this block prints a string. But what string?

To answer that, we need to see what's being inserted into `rdi`. If we look at that instruction, it says: `lea rdi,[rip+0xbc]`. That means calculate the pointer to the position 0xbc ahead of the instruction pointer (which will be pointing at the next instruction) and store that address in `rdi`.

So whatever we're printing is 0xbc bytes ahead of this instruction. We can do the math ourselves: 0x788 (next instruction) + 0xbc (offset) = 0x845. 

We can use another standard Unix binary tool to view the raw data from a specific offset: `xxd`. In this case, let's issue 
`xxd -s 0x844 -l 0x40 crackme02.64`. `-s` is "seek" or "skip"; it makes the hexdump start at the offset we're interested in. `-l` is "length"; it makes the hexdump only 0x40 characters long, rather than the whole rest of the file. We see:

{{< highlight text >}}
$ xxd -s 0x844 -l 0x40 crackme02.64
00000844: 4e65 6564 2065 7861 6374 6c79 206f 6e65  Need exactly one
00000854: 2061 7267 756d 656e 742e 004e 6f2c 2025   argument..No, %
00000864: 7320 6973 206e 6f74 2063 6f72 7265 6374  s is not correct
00000874: 2e0a 0070 6173 7377 6f72 6431 0059 6573  ...password1.Yes
{{< /highlight >}}

So, now we know. This block prints a string reading "Need exactly one argument.", as you'd expect from looking at the program's behavior when too many or too few arguments are specified. 

### Basic Flow Analysis

The most important part of this block is the unconditional jump at the end, which goes to address 77c:

{{< highlight asm >}}
add    rsp,0x8
ret 
{{< /highlight >}}

This block removes the local variables from the stack and returns. So, that's it. If there aren't exactly 2 strings provided to the binary - its own name and one command line argument - it will quit.

We can start to write this program out in C code:

{{< highlight c >}}
int main(int argc, char** argv){
    if (argc != 2) {
        puts("Need exactly one argument.");
        return -1;
    }

    // Magic happens here
}
{{< /highlight >}}

To find out just _what_ magic happens in that lower part of the program, we need to look at the program's flow. Assuming the `argc` check succeeds ( the jump at 0x717 isn't taken), program execution proceeds into this block:

{{< highlight asm >}}
mov    rdx,QWORD PTR [rsi+0x8]
movzx  eax,BYTE PTR [rdx]
test   al,al
je     761 <main+0x51>
{{< /highlight >}}

That first instruction moves the quadword (64-bit value) at the address `[rsi+0x8]` into `rdx`. What's in `rsi`, the full 64-bit Source Index register? Turns out, that's the _second_ argument in the Linux x86_64 calling convention. So in C, this is the value `argv + 8` or, because `argv` is of type `char**`, `argv[1]`.

The next instruction moves and extends with zeroes (`movzx`) a single byte from the memory address stored in `rdx`; in other words, `*argv[1]`, or `argv[1][0]`. The Accumulator register now has all zeroes except for the last 8 bytes, which contain the first byte of `argv[1]`, the the command line argument to the program.

`test al,al` is equivalent to `cmp al, 0`. `al` is the lower 8 bytes of the Accumulator register. Essentially, this block is equivalent to the C code:

{{< highlight c >}}
if (argv[1][0] == 0) {
    // do something
}
{{< /highlight >}}

So, what's at address 0x761? It's the following block:

{{< highlight asm >}}
lea    rsi,[rip+0x119]        # 881 <_IO_stdin_used+0x41>
mov    edi,0x1
mov    eax,0x0
call   5c8 <.plt.got+0x8>
mov    eax,0x0
add    rsp,0x8
ret    
{{< /highlight >}}

One of the most important skills for a reverse engineer is noticing patterns, and you should be seeing one right away. Here, the program `lea`s a relative offset from the instruction pointer into `rsi` and then calls a function.

That function is, according to the same technique used above, `printf`. `printf` takes a format string and a variable number of arguments. All variadic functions need the Accumulator register to hold a value telling them how many arguments to look for in the FPU registers (in this case, none, as we can see from the `mov eax, 0x0` instruction). The `rdx` register already holds the pointer `argv[1]`, so that's the second argument.

So what's the format string? We use the same technique as before, but this time I didn't take out the comment that `objdump` added in where it did the math for us.

So, running `xxd -s 0x881 -l 0x40 crackme02.64` gives us the answer: the format string here is `Yes, %s is correct!`. This looks promising!
Additionally, we can see that, after the function call (at 0x77c; this is a useful address to remember), the space for local variables is removed from the stack, and the function returns. The return value is always placed in the Accumulator, so here the program returns a zero; success!

So, our C code looks like this:

{{< highlight c >}}
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
{{< /highlight >}}

At first glance, we would appear to be done! However, it's impossible (from the command line) to inject a command line argument whose first byte is zero; it would simply be an empty string, and the shell would not add it to the count of arguments, even if you could type it in. Let's move on.

If that check fails, the code goes to this block (at 0x724):

{{< highlight c >}}
cmp    al,0x6f
jne    794 <main+0x84>
{{< /highlight >}}

Recall that, since we're working from the assumption of that jump to success not being taken, `al` has `argv[1][0]` in it still. Here, we're checking if it's not equal to 0x6f (111 in decimal; 'o' in ASCII). If so, we jump to address 0x794. 

{{< highlight asm >}}
lea    rsi,[rip+0xc4]        # 85f <_IO_stdin_used+0x1f>
mov    edi,0x1
mov    eax,0x0
call   5c8 <.plt.got+0x8>
mov    eax,0x1
jmp    77c <main+0x6c>

{{< /highlight >}}

This is another print-and-return block; that unconditional jump (`jmp`) at the end jumps to 0x77c, where the program removes its stack space for local variables and returns. 

Rather than printing the success message, this block prints "No, %s is not correct." formatted with the command line argument, and then returns a failure code (1). **We now know, for sure, that the correct message starts with the letter 'o'**, because without it, there's an automatic fail condition.

{{< highlight c >}}
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
{{< /highlight >}}

Assuming that jump is _not_ taken, we go on to this code, at 0x728:

{{< highlight asm >}}
mov    esi,0x1
mov    eax,0x61
mov    ecx,0x1
lea    rdi,[rip+0x139]        # 877 <_IO_stdin_used+0x37>
movzx  ecx,BYTE PTR [rdx+rcx*1]
test   cl,cl
je     761 <main+0x51>
{{< /highlight >}}

Here, we load up some registers with constants, and then load a pointer into `rdi`. That pointer is to the string "password1". But we know this isn't the right password. So what's going on?

The next instruction places the byte at the address `rdx + rcx`. What's in `rdx`? Well, if we backtrack to 0x719, we see that it was loaded with the value at `rsi + 0x8`; a.k.a. `argv[1]`. So we're indexing into that string here; `ecx` = `argv[1][1]`.

Again, the most important skill for reverse engineering is recognizing patterns. Here's a common one we saw above: `test`ing a register against itself, followed by a `je`, means "jump if that register is zero".

So, if there's a zero at `argv[1][1]`, we jump to 0x761. Where is that? It's the block we just reversed above; it prints the success string and exits with the return code of 0. Our pseudocode looks like this:

{{< highlight c >}}
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
{{< /highlight >}}


What happens if it's not zero, though? The program progresses down to 0x746.

{{< highlight asm >}}
movsx  eax,al
sub    eax,0x1
movsx  ecx,cl
cmp    eax,ecx
jne    794 <main+0x84>
{{< /highlight >}}

Here, we zero out all but the lowest 8 bytes of `eax`, then subtract one from it. Then we zero out all but the last 8 bytes of `ecx` and compare `eax` to `ecx`. If they're not equal, we jump to 0x794. Where is that? It's another block we've already reversed; it prints the failure string and exits with the return code of 1.

What is this actually doing? From above, `eax` contains a single byte; 0x61 (decimal 97, or 'a' in ASCII). It has one subtracted from it, so it's 0x60 (decimal 96, '\`' in ASCII). So we know that the first two characters of the key are 'o\`'. Our pseudocode looks like this:

{{< highlight c >}}
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
{{< /highlight >}}

If they are equal, we move on to 0x753:

{{< highlight asm >}}
add    esi,0x1
movsxd rcx,esi
movzx  eax,BYTE PTR [rdi+rcx*1]
test   al,al
jne    73e <main+0x2e>
{{< /highlight >}}

First off, the program increments `esi`. (`esi` was set to 1 in the previous block.) It then moves that value into the lower 32 bits of `rcx`.

Then, the program loads a single byte from `rdi + rcx`. `rdi` is `argv[1]`, and `rcx` is `esi + 1` (which is, at this moment, 2). So here, the program is loading `argv[1][2]`. More correctly, though, it's loading `argv[1][rcx]` (you'll see why this matters in a moment).

The program then checks if that value is zero; if not, it jumps to 0x73e. Where is that?

{{< highlight asm >}}
movzx  ecx,BYTE PTR [rdx+rcx*1]
test   cl,cl
je     761 <main+0x51>
{{< /highlight >}}

We've seen this before. It's just the check from a few sections above. It loads a byte from `argv[1][ecx]` and checks if it's zero; if so it jumps to success, and if not it moves down to the code we just reversed. This is another pattern you should recognize in the future: **a loop**.

Now that we've identified the whole loop, let's look at all of its instructions, from 0x73e to 0x75f.

Recall that, a few blocks ago, `rdi` was loaded with the address of the string `password1`, but this wasn't the right password. Here we can see why; bytes are being loaded from that string and having one added to them before they're compared with the actual input.

{{< highlight asm >}}
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
{{< /highlight >}}

It's important to note that, while we as humans would write this like the following C code, the compiler actually moved the second part of the loop check to the _end_ of the loop, and loads the next byte for the comparison string there.

> **Try It Yourself:** The following C code should be all you need to determine the correct code. Try it!

{{< highlight c >}}
int main(int argc, char** argv){
    if (argc != 2) {
        puts("Need exactly one argument.");
        return -1;
    }
    // This pointer is in rdi in our disassembled binary
    char* comparison = "password1";
    // This is the value used to index the argv[1]
    int i = 0;

    while (argv[1][i] != 0 && (comparison[i] + 1) != 0) {
        if (argv[1][i] != comparison[i]) {
            printf("No, %s is not correct.", argv[1]);
            return 1;
        }
        i++;
    }

    printf("Yes, %s is correct.", argv[1]);
    return 0;
}
{{< /highlight >}}

This is really all we need. Simply adding one to each letter of `password1` in ASCII gives us "o\`rrvnqc0". Let's try it:

{{< highlight text >}}
$ ./crackme02.64 o\`rrvnqc0
Yes, o`rrvnqc0 is correct!
{{< /highlight >}}

The sharp-eyed among you might have noticed that this binary has a problem, however; it will actually accept _any_ of these characters. o, o\`, o\`r, o\`rr, et cetera all work! Clearly not a very good method to use for your product key.

If you made it this far, good job! Reverse engineering is difficult, but this is the core of it, and it only gets easier from here.

> **Exercise**: There is a file called `crackme02e.c` which can be solved using this same technique. Compile and attempt to solve it, to cement your skills.

## crackme03.c

The next crackme is slightly more difficult. In `crackme02`, we were able to manually inspect each branch, building up the flow of execution mentally. This technique breaks down as programs become more complex.

### The Radare Binary Analysis Tool

Fortunately, the reverse engineering community is rife with smart people, and there are excellent tools to automate a great deal of this analysis. Some of them, like Ida Pro, cost as much as $5000, but my personal favorite is Radare2 (**Ra**ndom **da**ta **re**covery), which is entirely free and open source.

Running `crackme03.64`, we can see that it (basically) works about the same way as the other two. It needs exactly one argument, and when we provide it with one, it helpfully tells us that it's wrong.

This time, rather than `objdump`ing the file, we're going to open it with `radare2` (or `r2`): `r2 ./crackme03.64`. You'll see a prompt. Type "?", and you'll get a help listing. Radare is an immensely powerful tool, but for this challenge we won't need most of its functionality. I've removed many entries, paring it down to the bare necessities.

{{< highlight text >}}
[0x000005e0]> ?
Usage: [.][times][cmd][~grep][@[@iter]addr!size][|>pipe] ; ...
Append '?' to any char command to get detailed help
Prefix with number to repeat command N times (f.ex: 3x)
| a[?]                    Analysis commands
| p[?] [len]              Print current block with format and length
| s[?] [addr]             Seek to address (also for '0x', '0x1' == 's 0x1')
| V                       Enter visual mode (V! = panels, VV = fcngraph, VVV = callgraph)
{{< /highlight >}}

The important thing to note is that Radare is **self-documenting**. If you ever want to know what you can do with a command, simply type a ? after it. For instance, we want to `a`nalyze the current program:

{{< highlight text >}}
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
{{< /highlight >}}

> **Try It Yourself:** I suggest traversing the help for a while. Google every term you don't understand. There is a lot of cool functionality that I won't touch on here, but which might inspire you to try something.

### Using Radare for Analysis

It turns out that the command we want it `aaa`: `a`nalyze using `a`ll techniques on `a`ll functions. This gives us a little output, including:

{{< highlight text >}}
[x] Constructing a function name for fcn.* and sym.func.* functions
{{< /highlight >}}

This means that Radare has a list of functions available to us. We can view it with `afl`: `a`nalyze `f`unctions, displaying a `l`ist.

{{< highlight text >}}
[0x000005e0]> afl
0x00000590    3 23           sym._init
0x000005c0    1 8            sub.__cxa_finalize_200_5c0
0x000005c8    1 8            sub.__cxa_finalize_224_5c8
0x000005d0    1 16           sub.__cxa_finalize_248_5d0
0x000005e0    1 43           entry0
0x00000610    4 50   -> 44   sym.deregister_tm_clones
0x00000650    4 66   -> 57   sym.register_tm_clones
0x000006a0    5 50           sym.__do_global_dtors_aux
0x000006e0    4 48   -> 42   sym.frame_dummy
0x00000710    7 58           sym.check_pw
0x0000074a    7 203          sym.main
0x00000820    4 101          sym.__libc_csu_init
0x00000890    1 2            sym.__libc_csu_fini
0x00000894    1 9            sym._fini
{{< /highlight >}}

We only care about `main` and `check_pw`.

> **Try It Yourself:** Figure out why I can immediately discard the other functions for initial analysis. Search engines are your friend.

Radare can disassemble just a single function for us with `pdf@sym.main`: `p`rint `d`isassembly of a `f`unction `@` (at) the `sym`bol table's entry called `main`. Radare also supports tab completion in many contexts. For instance, if you type `pdf@sym.` and hit the tab key, you'll get a list of all the functions in the symbol table.

Anyway, the first thing to notice is that Radare does syntax highlighting, adds a lot of comments, and even names some variables for us. It does some analysis to determine the types of variables, too; in this case, we have 9 local stack variables. Radare names them `local_2h`, `local_3h`, et cetera based on their offsets from the stack pointer.

### Automatic Flow Analysis

The beginning of our program is pretty familiar. Starting at 0x74a:

{{< highlight asm >}}
push rbx
sub rsp, 0x10
cmp edi, 2
jne 0x7cc
{{< /highlight >}}

We have the function prologue allocating 16 bytes of memory for our local variables and an `if` statement. Recall that the DI register holds the first argument, and since this is `main`, that argument is `argc`. So, `if (argc != 2) jump somewhere`.

In Radare, look to the left of the `jne` instruction. You'll see an arrow coming out of that instruction and running down to 0x7cc, where we see:

{{< highlight asm >}}
lea rdi, qword str.Need_exactly_one_argument. ; 0x8a4 ; str.Need_exactly_one_argument. ; "Need exactly one argument." @ 0x8a4
call sub.__cxa_finalize_200_5c0
{{< /highlight >}}

Remember how annoying it was to search for string literals in our binary? Radare does it for us, giving us the address, a convenient alias, and the value of the string literal. 

This is a little different from our previous binary; rather than loading that string, calling `printf`, and returning a value, this code calls `sub.__cxa_finalize_200_5c0`. What the heck is that?

Turns out, this binary has a slightly different structure than the previous ones. (Thanks `gcc`.) 

> **NOTE:** This section is incomplete. I will update it soon!

# Appendix

## The Makefile

The Makefile used is fairly straightforward, but it does have a few quirks. Prime among them is the use of `objcopy` on the compiled executables. I use it to strip out the `FILE` symbol, which would otherwise be used by Radare to display source code listings alongside the disassembly, completely defeating the purpouse of the exercise.

## Exercises

The files named `crackme01e.c`, `crackme02e.c`, et cetera are modified versions of their non-suffixed counterparts. They are intended as exercises, and can be solved with exactly the same techniques as are shown in the respective sections of this tutorial. You will have a much better experience if you solve them before moving on.
