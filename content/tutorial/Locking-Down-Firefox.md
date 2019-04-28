---
title: "Locking Down Firefox"
date: 2017-11-06T11:32:43-06:00
categories:
- Open Source
- Privacy
description: Some tips and configuration switches to throw in order to make a Firefox Quantum installation more secure and privacy-protecting.
---

I recently switched back to Firefox from Google Chrome. Firefox has gotten a lot faster recently, and is [in the process](https://www.cnet.com/news/firefox-quantum-challenges-chrome-in-browser-speed/) of getting event faster, partly thanks to Mozilla's [Rust language](https://rust-lang.org), which is being used to write important components in a provably memory-safe way. This allows them to be made much more concurrent and parallel, leading to large speed improvements.

One big draw of Firefox is that it's not a data collection tool for Google, and I'd like to double down on that promise. So, here's a list of simple (and not-so-simple) ways to lock down Firefox 57, to keep your browsing private.

## Extensions

### Ad Blockers

First off, there are a lot of great browser extensions (using the standard WebExtension platform) which can improve your experience. The most important on the modern web is an ad-blocker of some kind. I personally use [AdNauseam](https://addons.mozilla.org/en-US/firefox/addon/adnauseam/), which attempts to not only block ads but actually pretends to click on them, throwing off tracking and costing advertisers money. There are many other options available; I used to use the software that AdNauseam is based on, which is called [uBlock Origin](https://addons.mozilla.org/en-US/firefox/addon/ublock-origin/); it's an excellent piece of software which simply blocks ads, with a very low memory and CPU overhead.

### CDN Caching

Many websites use pieces of JavaScript and CSS code imported from CDNs (Content Delivery Networks) elsewhere on the web. Unfortunately, this allows the publishers of those libraries to track anyone who visits multiple websites that use them. It's also kind of slow. [Decentraleyes](https://decentraleyes.org/) saves many of these shared pieces of code to your computer, then intercepts requests for them and uses your saved copy. This both prevents the CDN operators from tracking you and speeds up your browsing experience.

### Adaptive Tracker Blocking

The [Electronic Frontier Foundation](https://eff.org) publishes and maintains a great piece of software called [Privacy Badger](https://www.eff.org/privacybadger). This browser extension learns which domains appear over and over again on different websites, and uses some logic to determine whether or not they might be tracking you. It then gives you the option to allow, block, or block cookies from these domains. Combined with DecentralEyes, this is unlikely to break your browsing experience, but because it adaptively learns from your browsing behavior, it's very effective at preventing certain kinds of tracking.

### Fingerprint Resistance

There are a lot of pieces of information that a website can use to track you, even without explicitly dropping a cookie or other tracking data onto your computer. This is called browser fingerprinting; you can learn more at the EFF's [Panopticlick](https://firstpartysimulator.net/) project. Firefox has a built-in tracking resistance mode, which I'll cover later, but it's very aggressive and breaks some websites. For me, this wasn't good enough, so I built [an extension](randomua/) that changes your User-Agent string, a major part of the browser's fingerprint, on every single request. This makes it very hard to correlate your behavior.

### HTTPS Everywhere

HTTPS is the basic underlying encryption technology for the web. While properly-configured websites generally upgrade HTTP requests to HTTPS when possible, you can install the [HTTPS Everywhere](https://www.eff.org/https-everywhere) extension (again by the great folks at the EFF) to make sure your browser check if HTTPS is known to be available and uses to it if possible. This not only improves your tracking protection, but also makes your computer significantly more resistant to the [SSLStrip](https://github.com/moxie0/sslstrip) attack.

## Config Settings

### Third Party Cookies
First-party cookies - cookies set and read by the website you're visiting - are immensely useful, allowing you to stay logged in across page loads and keep preferences saved. However, third-party cookies - cookies that are set by another site than the one you are visiting - are basically useless, except for tracking. You can disable them thus:

1. Go to _Preferences_.
1. Go to _Privacy and Security_.
1. Scroll to _History_.
1. Select Firefox will: _Use custom settings for history_.
1. Change "Accept third-party cookies" to _Never_.

### First Party Cookies

You can also configure Firefox to clear all your cookies when you close it. If you're ok with logging in to websites again, this is a good step to take; since I use a password manager, it's not much of a hassle. In the same place as the third-party setting above, change "Keep until" to _I close Firefox_.

### Tracking Protection

Firefox has a list of known trackers, and it can automatically block them, but only when you're using Private Browsing mode. To enable this:

1. Go to _Preferences_.
1. Go to _Privacy and Security_.
1. Scroll to _Tracking Protection_.
1. Check the box entitled _Use Tracking Protection in Private Browsing_.

Combined with Privacy Badger, this effectively covers both your private and non-private use of Firefox for tracker blocking.

### Multi Account Containers

This is a great feature which is not really in the scope of this guide, but I suggest you check it out [here](https://addons.mozilla.org/en-US/firefox/addon/multi-account-containers/). It allows you to segregate some tabs from others, allowing you to have multiple accounts open in a single browser window, and effectively preventing all CSRF attacks.

### Plugins

Plugins, like Adobe Flash and some kinds of DRM video decryption, can contain secret, unauditable code. They also often contain vulnerabilities. Firefox allows you to set these plugins to click-to-run, meaning that they require explicit user permission to execute. This has very little impact on your browsing experience, but a huge impact on security. To set this option:

1. Navigate to "about:addons".
1. Select _Plugins_.
1. Set them all to _Never Activate_, except for the _OpenH264 Video Codec_ and _Widevine Decryption Module_, which should be _Always Activate_, and _Shockwave Flash_, which should be _Ask to Activate_.

### Isolate to First-Party Domains

Firefox has a lot of config options that aren't normally exposed to the user, all accessed via the url "about:config". When you go there, you'll be warned that you might break Firefox. This is true, but not if you follow my instructions, so go ahead and click through.

Search for the setting `privacy.firstparty.isolate` and go ahead and set it to `true`. This will segregate sites from each other much more strongly than usual, making tracking very hard.

## Done!

Congratulations! Your browser is now pretty dang difficult to track. If you have suggestions for more hardening methods, please contact me.
I'd like to give a shoutout to @david_ross@mastodon.social and @gemlog@mastodon.sdf.org for some suggestions I used here.

> Note: This post has been edited to correct errors in the spelling and description of operation of Decentraleyes and HTTPSEverywhere.
