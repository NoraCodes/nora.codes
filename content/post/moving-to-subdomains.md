---
title: "Moving to Subdomains"
date: 2018-05-13T16:26:58-05:00
categories:
- Networking
- System Administration
- Open Source
- Linux
---

I just finished moving my EtherPad Lite instance and my Gogs Git VCS instance to subdomains, rather than subdirectories.
This involved two sources of pain:

1. a lot of _waiting_ for the DNS to propagate. First I had to wait to get my new NS settings set, then to actually update the domain names allowing the [git.leotindall.com](https://git.leotindall.com) and [pad.leotindall.com](https://pad.leotindall.com) domains to point to this server.

1. a lot of config file updating in multiple places. I did eventually unify my Nginx config, which will save me some grief in the future, but I still had to update the Gogs and Etherpad Lite configs to be aware of their new locations.

My new Nginx config is in a few pieces:

* `leotindall.com` config. This holds the config for this blog, some other static content, my Keybase proof, and most importantly a server block which serves on port 80, on both IPv4 and IPv6, and issues 301 redirects to the same page at port 443 over HTTPS.
* `vid.leotindall.com` config for PeerTube. This has to do a little bit of magic to do DNS resolution (for lookup for webtorrent hosts) and websockets.
* `pad.leotindall.com` config for EtherPad Lite. It's relatively simple, but does have to do some websockets config.
* `git.leotindall.com` config for Gogs. The simplest of the configs, it just points to the Gogs webserver.
* `longview` config for the Linode Longview server.

Overall, it's a much less convoluted system that I had in the past, and it means I can trivially seperate these services into multiple servers in the future.

I also recently installed Cockpit, which has been... a bit underwhelming. I'm hoping that will pick up, but either way it'll be useful if I do end up leasing additional servers, since it can chart CPU and memory usage from more than one at a time, but it doesn't offer a way to monitor Nginx or other webservers, or PostgreSQL.
