---
title: "System76 Thelio: A Review"
date: 2019-02-23T11:31:34-06:00
categories:
- Hardware
- Linux
- Open Source
description: "A comprehensive review of the System76 Thelio (thelio-r1)."
---

> **The bottom line**: Thelio, System76’s new “open hardware” desktop, is a
small, beautiful, and powerful desktop computer that hits every high point
anyone could have expected, faltering only in the inherent limitations of its
small size.

> **Pros**: pretty, small, performant, well cooled, well supported, and easy to
modify

> **Cons**: somewhat noisy, no front I/O, not as many USB ports as one might
desire

{{< figure "left" >}}
![The front and wooden side of the Thelio thelio-r1 with the included post-card.](/images/thelio/front_with_postcard.jpg)
The smallest Thelio, thelio-r1, was made available in January 2019.
{{< /figure >}}


In November 2018, System76, renowned Linux laptop company, announced the Thelio
though an ARG-like process of story-based videos. The product itself was rarely
featured, but a [good deal of
hype](https://www.phoronix.com/scan.php?page=news_item&px=System76-Thelio-Specs)
built up around the whole affair. When specs and a configurator were finally
made available, along with the details of the open source I/O add-in board and
the manufacturing process that System76 was spinning up in Colorado, that hype
appeared at least partially justified. I've had a Thelio for a couple of weeks
now, and it's pretty much lived up to its manufacturers lofty promises.

## The Exterior

The smallest Thelio, designated “thelio-r1”, stands 32 centimeters tall, 20
centimeters wide, and 28 centimeters deep, coming to only a total of 18 liters
in volume, a good deal smaller than even the tiniest commercial full-GPU mITX
form factor cases, most of which are at least 22 liters.

{{< figure "right" >}}
![The wooden side with the post-card and a 6inch/15cm ruler.](/images/thelio/side_with_postcard.jpg)
`thelio-r1` is absolutely tiny. The ruler in this photo is 15cm, or 6 inches.
{{< /figure >}}

The front, sides, and top are all made of powder coated metal, colored a deep
and even black. On the front and metal side are subtle laser-etched designs: a
System76 logo and a Colorado mountainscape, respectively. Covering much of the
right side and the right portion of the front is the wrap-around wood accent
panel, which can be purchased in either dark maple or “high-contrast” cherry.

The maple on my unit blends well with my dorm room furniture and will match
with any natural or dark-colored desk or media console. For rooms with a
lighter color scheme, the cherry version might be a better option. The wood is
rough and not finished with a varnish or stain, which could leave it vulnerable
to damage over time.

Aside from a single LED-lit metal button, the front, top, and sides have no
cutouts or ports. The case comes down in the front and back to touch the
surface the computer rests upon, though four small rubber posts on the
underside take the weight of the system itself. Between those areas there is a
small cut-out section to permit sufficient airflow to the underside intake fan.

{{< figure "left" >}}
![The rear panel of the Thelio.](/images/thelio/rear_panel.jpg)
The rear panel provides all the I/O on the entire system.
{{< /figure >}}

The choice of motherboard and GPU determines the back panel’s ports and
configuration. My AMD model has

 * HDMI and DisplayPort from the motherboard
 * line out, line in, and headphone out
 * Gigabit Ethernet 
 * 2xUSB type A 3.1 gen 1
 * 2xUSB type A 3.1 gen 2
 * Video connectors from the GPU

The Intel version also includes a USB type C port, but either way, the small
form factor will mean a shortage of USB ports. I recommend placing a USB 3.1
gen 2 hub either atop the machine or on your desk or console just beside it for
easy access.

Having machined cutouts for the back panel I/O is a boon for reliability and
durability, but it does make upgrading the motherboard very difficult. On the
other hand, System76 has chosen relatively high-end motherboards which are
likely to be supported for some time; AMD in particular plans to release new
CPUs on Socket AM4 as far out at 2022, making Thelio a long-term investment for
those willing to pull out the guts of their machines once or twice.

## The Mechanics

Clustering all the ports on the back contributes to the overall
clean and modern aesthetic, as well as making it very easy to remove the entire
wood and aluminum shroud. It requires only the removal of four tool-free thumb
screws, which are powder-coated to the same color as the case. Once these are
taken out, the whole case shroud simply slides up on a set of extruded rails
that fit like a glove and are simple to slot back into place.

{{< figure "left" >}}
![The metal side of the Thelio, showing the mountainscape engraving.](/images/thelio/metal_side.jpg)
Thelio's metal shroud is decordated with a beautiful stylized mountainscape, echoing its
Colorado origins.
{{< /figure >}}

The metal power button connects to the rest of the system via a set of four
pins which slide in and out of their sockets with ease, comfortably long after
the shroud has settled into its grooves; there’s little danger of bending them
or missing the socket during shroud replacement.

Within is a durable and competently engineered mini-ITX system. My unit came
with a Radeon RX580 GPU and 16GB of RAM, and everything was properly cable
managed, including the thin and bend-sensitive coaxial cables which feed into
the WiFi and Bluetooth antennae. Because the right side panel is wood, not
metal, these antennae can be entirely internal, leaving the exterior smooth and
durable. The chassis design preempts much of the damage than occurs in typical
systems when shipped with metal cross-bars and a well-secured GPU.

{{< figure "right" >}}
![The interior of the Thelio, showing off the excellent cable management.](/images/thelio/cable_management.jpg)
The interior cables are well-managed, which is important in such a tiny space.
{{< /figure >}}

The case design centers on combination of miniaturized computing and modular
storage. All systems ship with an NVMe drive mounted directly on the
motherboard, and include four slots for SATA 6GB/s hard drives. These hard
drives, as well as all the system fans, connect to a custom built open hardware
I/O board which routes SATA traffic and performs smart fan management.

The I/O board is reliable and low-latency, not impacting the read speed or seek
times on my Samsung 850 or 860 EVO SSDs in any detectable way. It also provides
enough juice via the SATA connectors to run four old and inefficient 1TB hard
drives simultaneously. Unfortunately, at the time of writing, System76 does not
make a configuration utility available for this I/O board, although they do
provide kernel modules which allow Linux users to monitor temperatures and
fanspeeds.

The drive slots use rails designed to work with the 16 included
vibration-isolating thumb screws, stored in specially machined screw-holes
inside the chassis. Once inserted, some significant force is required to remove
a drive, but not so much as to place the drive cage at risk of damage.
Unsurprisingly, given the small volume of the system, the cage is too small to
support 3.5 inch drives.

## Thermal and Acoustic Performance

A positive-pressure system fed through a
filtered fan below the chassis provides cooling air to the CPU, which exhausts
through a massive channeled fin cooler, and GPU, which blows out into the
chassis. This reduces the potential of the system internals to build up dust as
well as ensuring that the maximum amount of air passes through the heat
exchanger, though it does have the potential to starve the GPU fans of
room-temperature air.

{{< figure "right" >}}
![The bottom intake of Thelio, showing its Unix epoch solar system.](/images/thelio/bottom_vent.jpg)
Both of the Thelio's vents show the solar system at the time of the UNIX epoch. The filter
on this bottom intake does a good job even in the very dusty environment of my dorm.
{{< /figure >}}

Though not as effective as liquid cooling, this system
proved more than adequate for my system. I was unable to force the system’s
temperature more than about 30℃ above ambient, providing plenty of headroom for
voltage increases for those who wish to overclock. GPU temps were not nearly as
good on my RX 580, reaching almost 50℃ above ambient, but I observed no thermal
throttling. This likely depends on the CPU and GPU configured; luckily, the
Thelio supports all tiers of current-gen AMD Ryzen and Intel Core CPUs and
several Nvidia and AMD GPUs.

Of course, no air-cooled system can be as quiet
as a good water cooling solution, but the Thelio manages to keep its acoustic
load low and low-pitched thanks to the two large, slow-spinning fans and the
efficiency with which it uses the air they move. Because of the
positive-pressure design, it can’t spin down its fans when cool, so the system
is audible all the time, but it’s never obnoxious and rarely ramps up, even
under load.

## Customer Experience

The worst part of my Thelio experience was the
waiting. Not that there was so much of it, mind you; System76 custom-builds
each order in their Colorado factory and ships via UPS, so it arrived about 3
weeks after I ordered it, including the 5-day shipping, which can be upgraded.
System76’s web-based configurator provides many options ranging from low-end to
ultra-powered for CPU, GPU, storage, main memory, and even warranty. The
company even includes the option to get a shipping label to a recycling
facility for an old computer the Thelio might be replacing.

The Thelio’s
packaging did stand out in my campus mail center, with a huge stylized moon
dominating the package’s top along with the word “Thelio” and the System76
logo. Inside was a simple postcard, some instructions on how to get customer
support, and a great deal of what felt to me like very firm foam securing the
system in place. The cardboard provides hand-holds for carrying, a welcome
feature during my trek back across my frozen campus.

## Software 

The included
Pop!\_OS, despite having some of the worst naming on the Linux desktop today,
provides a great Ubuntu-based experience with minimal but well-thought-out
modifications to the GNOME 3 desktop. System76 included the drivers for my AMD
RX 580 and a firmware update program for future use, though no updates were
needed upon arrival. Unfortunately, there is no configuration utility for the
custom I/O board, though while the provided open source drivers do make fan
speed measurements available to the OS. 

The lack of the Ubuntu Snap subsystem
by default makes access to many proprietary apps more difficult than necessary.
On the other hand, the product is targeted at developers, and installing and
using Snap support is trivial from the command line, so this isn’t too much of
a black mark.

{{< figure "right" >}}
![The drive cage of my Thelio with one drive out of its slot.](/images/thelio/drive_cage.jpg)
Drives are inserted on rails using vibration-isolating gromits. If only it were this easy
to get RAID working!
{{< /figure >}}

With so many drive slots, a graphical configuration utility for
software RAID would be great; unfortunately, GNOME Disks no longer supports
creating mdraid volumes, though it can manage them. Again, this is probably
OK, as mdraid is simple to configure from the command line, though I did find
that the motherboard used in the AMD model does not play well with
partition-on-mdraid configurations; rather, each mdraid member needs to be
partitioned and those partitions set up as members of the mdraid array. This is
common, but disappointing.

## Price

For all this, my Thelio weighed in at a hefty
$1,866, plus $31 for 5-day UPS shipping. That’s not a trivial amount of money;
I can easily build a respectable gaming computer for a quarter of that, and
even a decent laptop workstation from Lenovo is just pushing $1,500. Larger
Thelio models like the Thelio Major and Thelio Massive, which starts at $3000,
and even higher-end configurations of the thelio-r1, can reach absolutely
astronomical prices. 

After doing some math, though, it looks like the margins
aren’t particularly high, especially for the level of integration and service
provided. The Thelio costs about 18% to 22% more than the prices of the
individual components; $200 dollars more than an almost equivalent PC Part
Picker build for the base model, or about $400 more for my model. This gap
widens only slightly with the ultra-high-end models.

## The Verdict

System76’s
new “open hardware” desktop, is a small, beautiful, and powerful desktop
computer that hits every high point anyone could have expected, faltering only
in the inherent limitations of its small size. It’s pretty, it’s tiny, it’s
fast, it’s well cooled, and the software support is top-tier. Despite being
somewhat noisy and lacking front I/O, it’s certainly a good machine for any
Linux user who can swallow the 18% - 22% upcharge for assembly and custom
engineering.

## Go/No Go

If you are a Linux user looking for a durable, competently engineered,
and beautiful desktop PC, the System76 Thelio is for you, so long as you have a
flexible budget.

If you don’t currently use Linux and want to get started, the
Thelio can still do what you need, but you might want to go with stock Ubuntu
instead of Pop!\_OS. If you’re a Mac OS developer who is looking to switch to
Linux, I suggest checking out Mike Dominic’s [review](https://dominickm.com/yo-thelio/) 
for a Mac user
perspective.

If you’re a software developer who likes to game, the Thelio is a
no-brainer. With the off-the-shelf hardware and excellent expandability, you
can easily add enough storage for even the largest games and, if needed, dual
boot Windows.

If you need more than one PCI-e expansion card, more than four
USB ports, or more than about 12 TB of storage, you might want to look
elsewhere, though the Thelio Major or Thelio Massive might meet your needs. 

If
you’re comfortable building your own PCs and don’t need to fit a powerful
system into a small space, you can save a buck by going that route instead, and
probably end up with better thermals.

Finally, if you need workstation power
on the go, the Thelio is simply the wrong product, but System76, Lenovo, and
Dell all offer excellent Linux-supporting portable workstations.

