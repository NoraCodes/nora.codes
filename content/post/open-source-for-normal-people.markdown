---
date: 2016-04-11 05:54:17+00:00
title: Open Source for Normal People
slug: open-source-for-normal-people
categories:
- Culture
- Open Source
description: An explanation of what open source is and why it matters, for non-techies.
---

This is not an article for technical people. It is an article normal people who just use their computers to get things done: to look at Facebook and Twitter, check their work email, write novels, design houses… whatever.

Maybe you’ve heard about open source by using a open source program like Firefox, OpenOffice, or Inkscape. Maybe a friend suggested you look into it. Whatever the reason, you’re here, so here you go: a simple, non-technical explanation of what it is and why it’s important.

**TL;DR (Too long, didn't read)**

For the quick overview: Most software is secret; open source software isn't. As counterintuitive as it may seem, it's not necessary to keep software secret in order to make money, and when software is not secret, competition is fiercer and innovation often progresses at a faster pace. Keeping software transparent also prevents the authors of that software from using anti-consumer and anti-competitive tactics like proprietary format lock-in.

#### Background

Some background first (really, just a little bit, I promise!)

Computers really just do math. That’s it; they don’t know what words are, or what a web page is. **Software** turns things humans are interested in, like words or web pages or building plans, into numbers that computers know how to work with. Software is made of **code**. First, there’s the **source code**, which is a kind of code that humans can (sort of) read. Programmers make source code that describes what a computer should do to numbers to get the result that you, the user, want. It’s a little something like a recipe.

That recipe gets “baked” into **machine code**, which very, very few humans understand. It's only for computers. That’s what you download from the App Store or get on a CD when you buy a piece of software like Angry Birds or Microsoft Word.

#### Closed Source

Most software, like Microsoft Word or Angry Birds, is **closed source** or **proprietary**. This means that all you, the customer, get is the machine code. You can’t read it, but your computer can, so you can click on the little W icon and your computer will open up a window with a blank page, ready to be filled with the next great American novel. The machine code tells the computer what each keypress means, how to save and load files, and when to draw that little red squiggly to inform you that you spelled “antidiesstablishmentarianism” wrong.

That seems all fine and dandy, right? Well, it is, except for two things.

**First, the people who made the software are the only ones who know how it works.**

This is problematic for a lot of reasons. There’s the “bus factor”: what happens if the developer is hit by a bus, or, more realistically, the company shuts down or decides to stop supporting that product? No more updates for you, and there’s nothing anyone can do about it, because nobody has the recipe, only the pre-baked machine code. It also means that, if the people who made the software don’t want you to use it with their competitors’ software, they can stop you (or, at least, try).

This happens a lot in the real world, and it’s called **lock-in**. If your whole company uses Solidworks or Adobe Photoshop, for example, it’s going to be a real pain for you to switch to an alternative, even if some other company came out with a better, cheaper CAD package or photo editor. Once you’ve started using a particular piece of proprietary software, there’s no guarantee that you can switch to a better or cheaper program without a lot of headache. This is nothing, though, compared to the other problem.

**Second, the people who made the software are the only ones who are really sure about what it does.**

In the same way that the cook is the only one who _really_ knows what’s in the cake you’re eating, the developers are the only ones who _really_ know what their program does. This is really, really bad when the closed source software is something that’s essential to a lot of people’s everyday lives and businesses, like Microsoft Windows, which, starting with version 10, forces users to install updates, sends information about them back to Microsoft, and even removes programs they’ve installed [without asking](http://www.makeuseof.com/tag/x-programs-windows-10-may-remove-device/). Imagine if that happened to a program you used for work the day before a major deadline!

The creators of closed source software can also artificially limit what the software is able to do. This is the model of the EagleCAD printed circuit board design program, which sells licenses ranging from $0 to $50,000. The software you get with each license is exactly the same, but the more you pay, the more features get unlocked, so most users end up with a powerful CAD package they can never use to its fullest extent.

#### Open Source to the Rescue

These problems are why many users and even businesses are embracing **open source** software. Open source software is software whose source code, or recipe, is available for everyone to see. This means that anyone who can understand source code (a lot more people than can understand machine code) can make sure that the software does only what it’s supposed to.

It also means that anyone can _change_ that code. Not like a wiki, mind you ! Changes have to be reviewed by the product's maintainers before they are accepted into the mainstream distribution of the product, so if you decide to download an open source product, you can be sure that you're getting the best version that's available. Just like deciding to throw in that pinch of cinnamon, even if the original baker hated spices,  you can change your own copy whenever you like, and (usually) redistribute that copy to others. If you have a problem with the way your software works, you can just change it or, if you can’t program, you can hire any developer in the world to do it for you. This may not seem like a big deal, but it has far-reaching consequences.

Imagine trying to get Microsoft to change something in Windows or Word for you, or trying to convince Adobe to add your favorite feature to Photoshop. They’d never do it because they can’t justify the cost, and even if they did agree to your change, it might take years before it was incorporated into a consumer version. On the other hand, with an open source product, you can download the software and use it just like with a closed source product, but if you need something to be changed and can’t wait for the people who made the software to do it, you can get it done _right away_ to your exact specifications. Open source gives you that freedom. If you don't like the way something works, you can often change it yourself, and if you can't, any developer in the world can.


#### The Open Source Business

While on the surface it might seem impossible to make money on open source software, many companies do so. Red Hat Inc. sells an enterprise operating system based on the open source Linux kernel. While their product is almost entirely open source, and is freely available to anyone, many industrial facilities, financial institutions, and governments pay huge sums for technical support. MongoDB Inc. publishes an eponymous open source database solution under a similar business model.

Other companies, especially small ones with between one and ten developers, work partially on what are called **open source bounties**. The software is free, but customers can make a contract with the developers which essentially says, "for some amount of money, the developers will create some feature by some specific date". This allows developers to make money, allows the customer to be sure that they will get the features they need when they are needed, and allows the entire community to benefit from these new features once they've been created.

Even electronic hardware companies are embracing open source. While Linux, KiCAD, and other open source software has been popular in electronics for a long time, businesses like SparkFun, Adafruit Industries, and many others are creating **open source hardware**. Electronics that are open source are sold by the companies that create them just like normal, but all the schematics, diagrams, printed circuit board designs, and bills of materials are distributed for free to anyone who wants them.

#### Open Source and Competition

All of these practices encourage competition and make anticompetitive tactics like lock-in via proprietary file formats impossible. What is more, the moment an open source product is improved (whether by the community or the product's owners), that innovation is available to everyone. Other developers can immediately begin to improve it and build upon it, and users can immediately begin to integrate it into their workflows and business processes.

#### Try It!

Open source is not the answer to every problem, but it is the answer to many of them. Open source products exist for nearly everything.

  * Office applications (like word processing, presentations, and spreadsheets);

  * web browsing;
 	
  * computer aided design in architecture, mechanical engineering, and electrical engineering;
 	
  * digital photo processing;
 	
  * digital art;
 	
  * professional and amateur video editing;
 	
  * accounting and financial auditing;
 	
  * tax processing;
 	
  * professional and amateur audio processing;
 	
  * web servers;
 	
  * blogging software (in fact, this site is powered by the open source Jekyll software);
 	
  * almost anything you can think of


It costs you nothing but a download and a few clicks to try a piece of open source software, so go ahead. If you have a problem, most products have a method by which you can bring that problem to the developers' attention (called **filing a bug** against the product), so they can fix it.

If you're curious about open source as a concept, and how it can help you or your business, please contact me at lfstindall@gmail.com. I'm more than happy to answer any questions you might have.
