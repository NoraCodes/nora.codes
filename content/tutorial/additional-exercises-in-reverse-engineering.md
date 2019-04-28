---
date: 2018-02-03T11:00:00-06:00
slug: additional-exercises-in-reverse-engineering
title: Additional Exercises in Reverse Engineering
categories:
- Programming
- Assembly
- Hacking
description: Some more exercises and commentary about reverse engineering 64-bit Intel binaries.
---

This is a sequel to my (rather long) [introduction](an-intro-to-x86_64-reverse-engineering/) to reverse engineering. It is something of a "whirlwind tour" of some useful methods that weren't shown in that tutorial, and provides a number of exercises to hone your skills.

## The CrackMe Programs

You can find the CrackMes discussed here on [GitHub](https://github.com/noracodes/crackmes). Clone that repository and, _without looking at the source code_, build each CrackMe with `make crackme01`, `make crackme02`, etc.

## Tools and Software

These CrackMes only work on Unix systems, and I wrote this tutorial using Linux. You need the essentials of a development environment installed - a C compiler (`gcc`), object inspection utilities (`objdump`, `objcopy`, `xxd`), et cetera. You also need [Radare2](http://radare.org/r/pics.html), an advanced open-source reverse engineering toolkit. On Debian-derived systems, the following command should get you set up:

`sudo apt install build-essential gcc xxd binutils`

You can install Radare2 from [here](https://github.com/radare/radare2).

On other systems, install the equivalent packages from your package manager.

# Solutions

## crackme05.c

This crackme is very similar to those presented in the previous tutorial, but is more modularized. It has functions (remember, `aaa` to analyze and then `afl` to list functions) for success and failure conditions, which print out the appropriate string and then exit.

The main function has a number of calls to `sym.fail`, the failure function, each stemming from a different condition. Only by passing all of these conditions does execution reach 0x880 where RDI (the first function argument) is loaded with the input string and then `sym.success` is called.

Some of these checks are self-evident; for instance, at 0x7d7, the string's length must be exactly 16:

<pre><code class="x86asm">
call sym.imp.strnlen
cmp eax, 0x10
jne 0x850
</code></pre>

Other checks, however, call the function `check_with_mod`, each time with three arguments. For instance, at 0x81a:

<pre><code>
lea rdi, [rbx + 8]
mov edx, 5
mov esi, 4
call sym.check_with_mod
</code></pre>

Here, RBX is `argv[1]`, so this is really `check_with_mod(argv[1] + 8, 5, 4)`. The third argument, here a 4, appears consistent across calls, while the second argument changes. So what does `check_with_mod` do?

As usual, `s sym.check_with_mod` followed by `pdf` will give us the answer. It's actually a very simple function, just 20 instructions. At its heart is a loop which adds up the values of some bytes in the input string (argument 1), dictated by the 3rd argument. In our case this will always be 4 bytes.

The function then performs integer division `idiv r8d`. This divides RDX by R8 (the second argument), saving the quotient in RAX and the remainder in EDX. The code then checks that RDX is zero and discards the quotient, making this a **modulus** operation.

So the mystery is solved. The code checks that the four bytes after each offset sum up to something divisible by the given value. Returning to the `main` function, we can see that there are four chunks, which need to sum up to 3, 4, 5, and 4, respectively.

That's not enough, though. It also compares byte 2 to 'B' and byte 0xd (13) to 'Q'.

So the string looks like this so far: `..B..........Q..`

Now we need to figure out those blank spaces. By doing a little math we can find `EEBD,,,,2222QQOO`, which is correct!

## crackme06.c

This crackme is pretty simple to solve using Radare2 static analysis (that is, just looking at the code), but I'd like to demonstrate another tool in the toolbox of a reverse engineer - `strace`.

`strace` prints out every system call a program makes as it runs. This makes it very useful to figure out the basic behavior of a program, as well as letting you easily isolate specific kinds of behavior like network connections and file I/O.

Here, we'll just run `strace ./crackme06.64 test`:

<pre><code>
execve("./crackme06.64", ["./crackme06.64", "test"], [/* 56 vars */]) = 0
brk(NULL)                               = 0x5645ad7a0000
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=249222, ...}) = 0
mmap(NULL, 249222, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f8dded0d000
close(3)                                = 0
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\340\22\2\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0755, st_size=1960656, ...}) = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f8dded0b000
mmap(NULL, 4061792, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f8dde743000
mprotect(0x7f8dde919000, 2097152, PROT_NONE) = 0
mmap(0x7f8ddeb19000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1d6000) = 0x7f8ddeb19000
mmap(0x7f8ddeb1f000, 14944, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7f8ddeb1f000
close(3)                                = 0
arch_prctl(ARCH_SET_FS, 0x7f8dded0c4c0) = 0
mprotect(0x7f8ddeb19000, 16384, PROT_READ) = 0
mprotect(0x5645ab906000, 4096, PROT_READ) = 0
mprotect(0x7f8dded4a000, 4096, PROT_READ) = 0
munmap(0x7f8dded0d000, 249222)          = 0
brk(NULL)                               = 0x5645ad7a0000
brk(0x5645ad7c1000)                     = 0x5645ad7c1000
openat(AT_FDCWD, "test", O_RDONLY)      = -1 ENOENT (No such file or directory)
dup(2)                                  = 3
fcntl(3, F_GETFL)                       = 0x8402 (flags O_RDWR|O_APPEND|O_LARGEFILE)
fstat(3, {st_mode=S_IFCHR|0620, st_rdev=makedev(136, 1), ...}) = 0
write(3, "PANIC! Aborting due to: No such "..., 50PANIC! Aborting due to: No such file or directory
) = 50
close(3)                                = 0
exit_group(-1)                          = ?
+++ exited with 255 +++
</code></pre>

Much of this output is relatively useless - loading shared library preloads which aren't set, and mapping bits of memory in, all done by the shell. Eventually, though, there's a call to `openat`:

<pre><code>
openat(AT_FDCWD, "test", O_RDONLY)      = -1 ENOENT (No such file or directory)
...
write(3, "PANIC! Aborting due to: No such "..., 50PANIC! Aborting due to: No such file or directory
) = 50
</code></pre>

The program calls `openat` with the filename `test`, attempting to open it in read-only mode, gets an error, and then prints that error out.

Clearly, it expects the file to exist; by creating that file and putting some content in it, we see:

<pre><code>
openat(AT_FDCWD, "test", O_RDONLY)  = 3
fstat(3, {st_mode=S_IFREG|0664, st_size=13, ...}) = 0
read(3, "some content\n", 4096)         = 13
read(3, "", 4096)                       = 0
fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(136, 1), ...}) = 0
write(1, "Access denied.\n", 15Access denied.
)        = 15
</code></pre>

Here, the code successfully opens the file and gets a file descriptor back (3). It reads the file and gets back "some content\n" and then reads it again and gets nothing. Finally, it prints "Access denied".

So we know the program is reading bytes from a file. Diving into Radare, it's trivial to see that those bytes are then compared to the string "scrambled egg 42". If you load that file with these bytes, the crackme will succeed.

Taking the time to trace the program statically would have yielded the same result, but it would have taken a lot longer. Dynamic analysis has a place, especially for identifying critical sections of the target executable.

## crackme07.c
This crackme is a pretty simple one, but it uses a slightly odd mechanism for input. It compares its first arg to a static string, but only succeeds if the current time is in a certain range.

There is a function `sym.cur_hour` which makes a call to the libc function `localtime`. It takes the resulting `localtime` struct and returns an quadword from 8 bytes into that struct.

<pre><code class="x86asm">
call sym.imp.localtime
mov edi, dword [rbx]
test edi, edi
jne 0x8f9                   ; jump to error handling if error
mov eax, dword [rax + 8]    ; this value gets returned
</code></pre>

That struct looks like this, according to the documentation for [localtime (3)](https://linux.die.net/man/3/localtime):

<pre><code class="c">
struct tm {
    int tm_sec;         /* seconds */
    int tm_min;         /* minutes */
    int tm_hour;        /* hours */
    int tm_mday;        /* day of the month */
    int tm_mon;         /* month */
    int tm_year;        /* year */
    int tm_wday;        /* day of the week */
    int tm_yday;        /* day in the year */
    int tm_isdst;       /* daylight saving time */
};
</code></pre>

So 8 bytes in is `tm_hour`, the current hour. In the main function, that value is used thus:

<pre><code class="x86asm">
call sym.cur_hour
mov ebx, eax
...
sub ebx, 5
cmp ebx, 1
jbe 0x985
</code></pre>

In other words, the hour must be between 5 and 6 (that is, the time must be between 0500 and 0659). Either stay up late or just temporarily change your system clock to get this crackme to succeed.

## crackme08.c

This is another "computed" crackme; that is, it computes the correct code on the fly.

One important feature of this program is that it allocates memory, at 0x869:

<pre><code class="x86asm">
mov edi, 0xf
call sym.imp.malloc
</code></pre>

This is a very common call for C programs - it's how they get memory on the heap - but previous crackmes haven't really needed to do this. This particular call allocates 0xf (15) bytes.

Immediately afterward, there's something that isn't normally seen: the `cpuid` instruction. This instruction is a way for a CPU to identify itself to software, and it places "characteristic bytes" in EBX, EDX, and ECX, in that order. While these could be any bytes, they are usually printable ASCII characters. Intel processors return the bytes "GenuineIntel", AMD processors return "AuthenticAMD" (or "AMDisbetter!" on old engineering samples), and other vendors have their own patterns ranging from the utilitarian "Vortex86 SOC" to the tongue-in-cheek "CyrixInstead".

Anyway, these bytes are then moved into other registers and passed to the function `sym.shift_int_to_char` along with the pointer returned by that previous call to `malloc`. Looking at that function, it's pretty simple; it moves the first, second, third, and fourth bytes of the given doubleword into memory sequentially. This is being used my the `main` code to destructure the weird encoding from `cpuid` into an actual string.

Hopping back to main, we see some other bytes get set: '3', 'Q', and the null byte. It's a good bet that the password is made up of the three doublewords from the CPUID instruction plus "3Q", and this checks out: three doublewords is 12 characters, plus two more is 14 plus null is 15 (or 0xf), the length of the allocated buffer.

Sure enough, that buffer is immediately checked using `strcmp` against the input and then released back to the system with `free`.

This is the first of many crackmes that will have different solutions on different computers. For me, it's "GenuineIntel3Q".

# Appendix

Thank you for reading this second tutorial! I hope it's been instructive. My next priority is to create some more real-world use cases, such as reverse engineering nontrivial C applications, looking at minified JavaScript, and doing some more in-depth dynamic analysis with GDB.

If you enjoyed these crackmes, please consider contributing to my Patreon! For those of you who are already patrons, thank you so much for supporting my work.
