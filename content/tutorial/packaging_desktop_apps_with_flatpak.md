---
title: "Packaging Desktop Apps With Flatpak"
date: 2019-09-10T15:00:00-05:00
tags: ['rust', 'flatpak', 'programming', 'gui', 'gtk']
description: A step-by-step walkthrough of the process of packaging a desktop app for distribution using the Flatpak format and Flathub.
---
As mentioned in the [previous post](/tutorial/speedy-desktop-apps-with-gtk-and-rust/), the web
platform is popular for creating apps that would make plenty of sense as desktop software.
This is partly because it unifies many platforms, but mostly it's because it solves
the **distribution problem**. That is, having written a program, how do I get it into the
hands of users?

Consider the process of installing a desktop app for a user with a fresh install of their
operating system:

- Download the installer for the application
- Execute the installer
- Run the application

On the other hand, the web platform simplifies all of that into a single step:

- Open the application in the web browser

Package managers remove the requirement to find and execute unvetted installation code,
but they have other flaws, such as requiring specific packaging for each operating system
distribution (Windows, Ubuntu/Debian, Arch, Fedora, CentOS, Nix, etc). Someone has to do
this work, whether or not the software developer themselves take it on.

Flatpak is one way to distribute applications in a more standardized way, and this post
demonstrates how to roll up the application and all its dependencies into a Flatpak
so that users can easily install it.

## The Makefile

The Makefile in the last post was extremely simple. This one is a bit more general, and
thus a bit more complicated. Firstly, it needs to be changed to support the [Makefile
conventions](https://www.gnu.org/prep/standards/html_node/Makefile-Conventions.html)
recommended by the GNU project, primarily by abstracting the system directories and
installation programs. At the top of the [new Makefile](https://gitlab.gnome.org/NoraCodes/gdiceroller/blob/fb1a884a811c58791211fa0e51d29c09eb802baf/Makefile), there's the following code:

```make
# Install to /usr unless otherwise specified, such as `make PREFIX=/app`
PREFIX=/usr

# What to run to install various files
INSTALL=install
# Run to install the actual binary
INSTALL_PROGRAM=$(INSTALL)
# Run to install application data, with differing permissions
INSTALL_DATA=$(INSTALL) -m 644

# Directories into which to install the various files
bindir=$(DESTDIR)$(PREFIX)/bin
sharedir=$(DESTDIR)$(PREFIX)/share
```

Then, in the `install` and `uninstall` targets, rather than simply `cp`ing files into the
correct places, `$(INSTALL_PROGRAM)` and `$(INSTALL_DATA)` are used, and every filepath is
prefixed with `$(sharedir)` or `$(bindir)`.

This way, if someone wants to install this program onto a distribution that requires
installing into a weird place like `/var/<program name>/whatever`, they can just set the
`PREFIX` variable - and we can use it to do the same for the Flatpak.

## How Flatpak Works

Flatpak packages use a number of Linux kernel technologies (`cgroups`, namespaces, seccomp,
and bind mounts) and some open standards from the Open Container Initiative to provide a
universal and standardized way to distribute applications. It provides a runtime on which
bundled libraries and applications can be executed (more info at the [official docs](http://docs.flatpak.org/en/latest/basic-concepts.html)).

The critical aspects for application developers and users are **security** through isolation
and **dependability** due to built-in dependency management, as well as **ease of use** in
terms of installation.

## Making a Flatpak

Making a Flatpak requres a program called `flatpak-builder`, in addition to the Flatpak
tools themselves. In addition, we need a few extra components:

```bash
# Add flathub and the gnome-nightly repo
flatpak remote-add --user --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
flatpak remote-add --user --if-not-exists gnome-nightly https://sdk.gnome.org/gnome-nightly.flatpakrepo

# Install the gnome-nightly Sdk and Platform runtime
flatpak install --user gnome-nightly org.gnome.Sdk org.gnome.Platform

# Install the required rust-stable extension from flathub
flatpak install --user flathub org.freedesktop.Sdk.Extension.rust-stable//18.08
```

With these installed, we need a Flatpak metadata file. We'll create the development
version first. So, in the `data` directory of the repo, the
[`codes.nora.gDiceRoller-development.json`](https://gitlab.gnome.org/NoraCodes/gdiceroller/blob/fb1a884a811c58791211fa0e51d29c09eb802baf/data/codes.nora.gDiceRoller-development.json) file will contain the app's build metadata:

```json
{
    "app-id" : "codes.nora.gDiceRoller",
    "tags" : [ "development" ],
```

This section establishes the name of this application (`app-id`), which must be globally
unique, and any tags that should be applied to the specific version built by this Flatpak
specification.

```json
    "runtime" : "org.gnome.Platform",
    "runtime-version" : "3.32",
    "sdk" : "org.gnome.Sdk",
    "sdk-extensions" : [
        "org.freedesktop.Sdk.Extension.rust-stable"
    ],
```

This section tells Flatpak and `flatpak-builder` which pieces of software and which
libraries are needed to build and run the application.
The `runtime` should pretty much always be `org.gnome.Platform`, while
the `runtime-version` is dictated by your requirements. Remember that newer versions are
available to fewer users, typically.

`sdk`, like `runtime`, is usually the same, but `sdk-extensions` depends heavily on which
tools and languages you use to create the application. In this case, that's the Rust
extension we installed earlier.

```json
    "command" : "codes.nora.gDiceRoller",
    "desktop-file-name-suffix" : " ☢️",
    "finish-args" : [
        "--socket=x11",
        "--socket=wayland",
        "--device=dri"
    ],
```
First off, the `command` specifies which file should be run to actually start the application.

`desktop-file-name-suffix` specifies that a "radioactive" symbol should be added to the end of the
application's desktop file. This is a common practice for Flatpak devs distributing both
a development and a production version of an app, so people can install both and tell
them apart in their application directory.

The `finish-args` section specifies that the program needs access to the various desktop
subsystems, so it can actually draw windows on the screen. I don't, for instance, specify
`--socket=network` or other external I/O, because the application doesn't need it.

```json
    "build-options" : {
        "append-path" : "/usr/lib/sdk/rust-stable/bin",
        "build-args" : [],
        "env" : {
            "CARGO_HOME" : "/run/build/gRiceRoller/cargo",
            "RUSTFLAGS" : "--error-format=short --remap-path-prefix =../",
            "RUST_BACKTRACE" : "1"
        }
    },
```

`build-options` simply sets the appropriate location for the Rust build tools
and turns on backtraces for debugging.

```json
    "modules" : [
        {
            "name" : "gDiceRoller",
            "buildsystem" : "simple",
            "build-commands": [
                "make",
                "make install PREFIX=/app" 
            ],
            "sources" : [
                {
                    "type" : "dir",
                    "path" : "../"
                }
            ]
        }
    ]
}
```

The `modules` section specifies what code actually needs to be built and how to do that.
In this case, that's a "simple" build system, which essentially means "just
run the commands here". Crucially, the `PREFIX` variable is set to `/app`, which is a
Flatpak requirement.

In the previous section, we set up the Makefile so that setting the `PREFIX` variable will
install the app in a different location. `flatpak-builder` will take the contents of
`/app` and package that as the application, so that's where it gets installed.

For the development version, we specify the local directory so that changes a developer
makes are reflected immediately.

The last step for this is to add building this to the `Makefile`:

```make
# Build a Flatpak package
flatpak-development: target/release/gDiceRoller
    mkdir -p flatpak-development
    flatpak-builder flatpak-development data/codes.nora.gDiceRoller-development.json
```

And, to `clean` the built package:

```make
# Remove supplemental build files
clean :
    rm -rf flatpak/ flatpak-development/
```

## Development vs. Production

The production version, in [`data/codes.nora.gDiceRoller.json`](https://gitlab.gnome.org/NoraCodes/gdiceroller/blob/fb1a884a811c58791211fa0e51d29c09eb802baf/data/codes.nora.gDiceRoller.json), is totally identical, except that:

- There is no `desktop-file-name-suffix` key, since this version doesn't need to differentiate itself
- There is no `tags` key, since this is the gold-standard version and doesn't need a tag
- The `sources` key is different.

Here, `sources` specifies the Git repository, along with the version tag and the commit hash:

```json
"sources" : [
  {
    "type" : "git",
    "url" : "https://gitlab.gnome.org/NoraCodes/gDiceRoller.git",
    "tag": "v1.1.3",
    "commit": "57ad7d8cc886c8d9c838e665441e347d71637b1d"
  }
]
```

Critically, this **decouples** this Flatpak specification from the code itself. This
will be important when uploating it to Flathub to be distributed.

In order to test this easily, there's also an appropriate `Makefile` target:

```make
# Build the Flatpak bundle for release
flatpak-release: target/release/gDiceRoller
	mkdir -p flatpak
	flatpak-builder flatpak data/codes.nora.gDiceRoller.json
```

For either the release or the development version, it's possible to test the build as
documented [here](http://docs.flatpak.org/en/latest/first-build.html#test-the-build).

## Flathub

## AppData
The final purpose of packaging the application into a Flatpak is to distribute it, and
Flathub is the place to be for distribution. In order to be permitted onto Flatpak,
an application must provide:

- Desktop files
- Application icons
- An AppData file

gDiceRoller has everything but an AppData file, so far. You can find the AppData
specification from [FreeDesktop](https://www.freedesktop.org/software/appstream/docs/).
The AppData file for gDiceRoller is at [`gdiceroller/data/codes.nora.gDiceRoller.appdata.xml`](https://gitlab.gnome.org/NoraCodes/gdiceroller/blob/fb1a884a811c58791211fa0e51d29c09eb802baf/data/codes.nora.gDiceRoller.appdata.xml), and the Makefile installs it to `$prefix/share/metainfo/`.

It's an XML document, and is kind of long, so I've excerpted the relevant sections.

```xml
<id>codes.nora.gDiceRoller</id>
<name>Dice Roller</name>
<project_license>GPL-3.0</project_license>
<metadata_license>CC0-1.0</metadata_license>
<developer_name>Leonora Tindall</developer_name>
<summary>Roll dice of many different shapes and sizes in all possible combinations.</summary>
<url type="homepage">https://gitlab.gnome.org/NoraCodes/gDiceRoller/</url>
<update_contact>nora@nora.codes</update_contact>
```

This section specifies the crucial information about the app - the unique ID, the display
name, my name, a short summary, the project's URL, and the relevent licenses.
`metadata_license` is the license for the info _within_ the AppData file and should almost
always be CC0 or similar.
`update_contact` should be an e-mail at which someone can be reached who can actually
change things in the package, if it needs to be updated. This is very important if someone
other than the main development team is maintaining the package.

```xml
<description>
    <p>
    Roll dice of many different shapes and sizes in all possible combinations.

    gDiceRoller provides several kinds of dice, from a d4 to a d100, which can be
    rolled with a simple button press. In addition, users can enter their own
    dice with any number of sides, and perform arbitrary arithemtic on their
    results.
    </p>
</description>
<categories>
    <category>Game</category>
    <category>GTK</category>
</categories>
<screenshots>
    <screenshot type="default">
    <image type="source">https://gitlab.gnome.org/NoraCodes/gDiceRoller/raw/master/data/screenshot.png</image>
    </screenshot>
</screenshots>
<releases>
    <release version="1.1.3" date="2019-06-28"/>
    <release version="1.1.2" date="2019-06-28"/>
    <release version="1.1.1" date="2019-06-28"/>
    <release version="1.1" date="2019-06-27"/>
</releases>
```

This section provides some more metadata about the application. `description` can contain
HTML which most application stores, like Flathub, will render. `screenshots` will be shown
in header, usually below the application's name and above the description.

`categories` is important, because it places the application in view of users in the
relevant categories in app stores.

The `releases` array contains `release` elements. Here, there's nothing within them, but
they can contain sub-`descriptions` which will be rendered, usually below the main
description. Use them to document changes in your application.

```xml
<launchable type="desktop-id">codes.nora.gDiceRoller.desktop</launchable>
<provides>
    <binary>codes.nora.gDiceRoller</binary>
</provides>
```
Finally, a few housekeeping tags. These set the actual desktop file that launches the
application, and the binary it provides, so that app stores can figure out conflicts.

## Publishing
With the AppData file finished, we can move on to actually publishing the project to
Flathub. This is a pretty simple process:

1. Fork the Flathub repository (https://github.com/flathub/flathub)
2. Clone the fork
3. Create a new branch with the name of the app
4. Add and commit the production manifest
5. Make a pull request

For example, [here](https://github.com/flathub/flathub/pull/1054)'s the PR I made for
this application.

Once that process is finished, the Flathub staff will review your application. Assuming
they like it, they'll create a new repository and make it available on a page like [this](https://flathub.org/apps/details/codes.nora.gDiceRoller),
and after a while, it'll show up in desktop app stores.

Congratulations!

