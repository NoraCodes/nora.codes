---
title: "Stop Making Students Use Eclipse"
date: 2020-04-11
categories:
- Essay
- Rant
- Education
description: Computer science education doesn't adequately educate in some key areas. These are my personal thoughts on that issue and how it might be corrected - by not relying on IDEs!
---

Using Java or Python in a professional IDE like IntelliJ IDEA, NetBeans, PyCharm, or Eclipse is not a good first introduction to programming for computer science students, whether they're in the field to become web developers, systems software engineers, or academic computer science researchers.

Some IDEs, such as jGRASP,[^jGRASP] provide education-specific tooling, like the "automatic generation of visualizations for improving the comprehensibility of the software"; that is, when you write a linked list and run your code in jGRASP, you see a diagram of a linked list on the screen.

[^jGRASP]: Cross, Hendrix, and Umphress, 2004. DOI 10.1109/FIE.2004.1408803

{{< figure "right" >}}
![jGRASP in action in IntelliJ IDEA.](https://www.jgrasp.org/images/ij_plugin.png)

jGRASP, an education-focused IDE, can be used on its own or, as here, integrated with IntelliJ IDEA.
{{< /figure >}}

Most IDEs, though, primarily serve two purposes for students. First, in a Java-focused curriculum, it **insulates the student** from the `javac` command line program, and the command line environment itself.
Second, it catches some basic mistakes and allows the student to **defer learning** about the finnicky language requirements that aren't deemed core to the curriculum, like imports and file naming requirements.

Crucially, I do believe that these are real problems. A student who has not written an `if` statement doesn't need to understand the philosophy behind putting each public class in its own file, or what `public` or "class" even means.[^teach_python]

[^teach_python]: This is why I think Python 3 is a better teaching language than Java, but that's beside the point here.

## What's the deal with these IDEs?

The whole purpose of an IDE is to provide an **integrated** environment - it's even in the name.
It also provides a homogeneous environment for the instructor to instruct on; rather than teaching students how to install Java and a platform-specific programmer's editor on each platform, they can just say "install this IDE, open it, and click New Project."
This is valuable in an introductory course, as it avoids wasting class time and lowers the barrier to entry, but it's crucial to introduce these inconvenient details eventually.

In my experience as a student, after the first or second year of CS classes, students tend to grasp the basic control structures.
They can write branches and loops and functions and use basic data structures like lists and maps.
The ACM's 2013 guidance on CS curricula agrees with me here; they recommend a focus on basic structures of programming languages and problem solving.

What they can't do, unless they've figured it out on their own, is operate a computer outside of the confines of the IDE they've been taught.
In none of the curricula I've seen, through personal experience or reading syllabi provided by other students, is there a place for students to get past the myriad of barriers that constitute the use of a computer in the modern day.

Students who use Windows aren't taught that, while their file system is case-insensitive, not all filesystems are. They probably aren't taught that a "file system" is a concept until a 300-level operating systems course.
Students who use Mac OS aren't taught what the `.DS_Store` directory is, or why it's irrelevant to their project submissions.
Students learning Java don't know that `javac` is their compiler and `java` their virtual machine, at least until they take a course in compilers.
Nobody focuses on things like ASCII, Unicode, and UTF-8, or on how programs interoperate, or on how to share and distribute programs that students write.

Introductory CS curricula focus on abstract ideas of programming, and use IDEs to accomplish that.

## Why is this a problem?

So, okay, students are being taught how to write code and solve problems with math instead of grapple with the intricate details of the eldrich monstrosities that are modern operating systems. Surely that's a good thing?

The issue is, as Kevlin Henney is fond of saying, "Software is nothing *but* the details."
When students don't understand what a file is, or haven't ever edited text in anything but Microsoft Word and don't realize they _can_ edit code outside of an IDE[^msword], they will not be able to do the crucial work of self-directed learning that is a hallmark of all computer science success.
When students have only ever programmed in Java using some bespoke learning library provided by their professor, it will take them much longer than necessary to figure out other languages, other libraries, and other approaches.
In a field that moves as fast as this one does, that's a very serious problem.

[^msword]: Example given by [feonixrift](https://hackers.town/@feonixrift/103761036258585619)

It also undermines their ability to learn in a classroom setting going forward.
Among my fellow students, those who merely do what is expected of them - merely attend each and every class, do the reading, and do their problem sets and homework - aren't prepared to learn about networking, or `git`, or project management, because their knowledge is disconnected from the real world.
Teaching someone to use `git` is very difficult if they've never been taught that a file is an logical unit composed of bytes and metadata.
Teaching someone the importance of agile methodology is difficult if they've never distributed a piece of software, even to their professor or friends.

Most importantly, though, it limits the ability of their _peers_ to learn.
If a 300-level software engineering class which budgeted a week to teach basic version control skills has to take a two-day detour to teach the Windows users how to get rid of the CRLFs in their commits, and teach the Mac users to remove the `.DS_Store` files from their repositories, and get everyone set up in Eclipse EGIT, that's wasted time.
If the professor has to schedule time with students outside of class to demonstrate their code because so many students aren't able to submit their code in a form that successfully runs on the professor's computer, that's wasted time for both professors and students, and it undermines important lessons about portability and good practices.

Ultimately, my core belief is this: Students need to know how to use computers before they can program them in a serious way.

## Moving forward - or backwards, or sideways?

I should be clear - I'm not talking about MIT. I'm not talking about CMU or UC Berkeley (oh, I'm sorry, "Cal").
I'm not even talking about Stanford.
I'm talking about other places - the SLACs that have four or five CS professors, the polytechnic universities, everywhere else.
The places where "computer science" and "computer engineering" might not be different majors, and even if they are, they aren't really different departments, different buildings, different schools.

So how do we move forward?
How do we build computer science programs that prepare students to build their own futures?
We have to recognize what the ACM has been telling us since 2002 - that there are two or three tracks to "computer science": what we might call "theoretical computer science"; what we might call "computer engineering"; and what we might call "software engineering".
We move forward by giving students a taste of all three when they enter the field, and letting them specialize as they wish.

I am very sympathetic to the idea that getting students programming quickly is important.
So do that.

* Provide a standardized environment - as a VM, perhaps, or using something like repl.it or ideone - perhaps a similar software designed specifically for education.

* Use a language that teaches the fundamentals of the paradigm you're interested in, like Scheme or Python. (Please, please not Java.)

* Provide support for students who are interested in doing their own thing. This means having teaching assistants (and paying them!) who can help students beyond just answering questions about their homework.

Even just an effective Python or Scheme curriculum at the 100 level would be a big improvement, but it's not enough. We need to teach students about _computers_ themselves.

* After the first foray into programming, take time to teach students about the UNIX command line. (If you're really, really committed to Windows for some reason, PowerShell is a great piece of software too!)

* Show students how things they're familiar with - graphical file managers, for instance - interact with these new command line skills, and how their programming languages interact with those.

* Commit to teaching a standardized environment that all students have access to. Ubuntu LTS is great for this, because people with their own computer can run it in a VM and the school can provide computers, and adventurous Windows users can use WSL.

And of course, mathematics is the foundation of computer science.
Blending that perspective with more practical elements is important so that students learning algorithms can implement them in the real world, and students building software can analyze it appropriately. Alongside the purely mathematical understanding and models of computing should come some exposure to "weird" programming languages like Haskell, Erlang, Prolog, Elm, etc.

These are hard changes for a lot of departments, especially those with faculty that hasn't writted production software in decades, or has only used a non-UNIX OS for a long time. I don't think that there's any good reason to force students to figure this stuff out on their own.

Students who have a good understanding of theory, competence in a standard UNIX environment with some language of choice, and some exposure to other languages will be better professors, better engineers, and better web developers. Let's improve this field for the future of computing!

> A commenter on Lobste.rs pointed out that there are some great resources from MIT around learning tooling and build systems: [The Missing Semester of Your CS Education](https://lobste.rs/s/ti1k98/missing_semester_your_cs_education_mit)
