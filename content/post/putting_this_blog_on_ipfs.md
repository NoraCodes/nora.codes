---
title: "Putting This Blog on IPFS"
slug: "putting_this_blog_on_ipfs"
date: 2018-09-19T13:57:47-05:00
---

[IPFS](https://ipfs.io/), the Interplanetary File System, is a global distributed immutable
datastore, an effort to decentralize and distribute the load of hosting websites, which
I [first wrote about](/post/ipfs-the-interplanetary-file-system/) back in 2016. (I suggest
you read that post if you aren't familiar with the technology.) It's a great technology,
and of course that means that
[Cloudflare wants to run a monkey-in-the-middle attack on it](https://blog.cloudflare.com/distributed-web-gateway/).

## Goals

I had two goals in mind when looking at IPFS again, two years down the road from my original
contact with the project. First, I wanted an IPFS gateway I could hook up to my browser
via [IPFS Companion](https://github.com/ipfs-shipyard/ipfs-companion) to make browsing
IPFS content easier. Second, I wanted to host my blog on IPFS, so others could share
the load of serving it (although it's mostly just because I thought it was cool).

The gateway is a pretty simple service; it takes HTTP(S) requests with `/ipfs/` or `/ipns/`
URLs, gets the content from the network, and returns it to the user. Setting up my blog,
though, is a bit more complex.

An `/ipfs/` URL is a totally static, immutable reference to some content, but this is
a blog - I want people to be able to browse is in a friendly and up-to-date way.

Friendliness is solved by the rather excellent directory implementation that IPFS natively
supports. If I have a directory `foo/` with two files, `foo/a.txt` and `foo/b.txt` and I
add `foo/` to IPFS recursively, the two regular files are hashed and published, and the
directory is published as a table mapping the filenames to their hashes. Then, given the
directory hash, IPFS can look up the files by name.

Timeliness is a little more complex. How can I update a blog if its contents are immutable,
represented by static content hashes? Fortunately, the IPNS (InterPlanetary Name System)
provides a way. I can use my node ID to point, mutably, to a hash. In this case, that hash
is the hash of my blog's directory, and I simply update where the IPNS name points every
time I update the blog.

Ok, let's get implementing!

## The Gateway

This website now runs an IPFS node, which you can use to access the network by prefixing
an IPFS or IPNS (the DNS equivalent that allows "mutable" content on the immutable web)
URL with `https://ipfs.leotindall.com`. For instance, the url `/ipfs/QmXoypizjW3WknFiJnKLwHCnL72vedxjQkDDP1mXWo6uco/wiki/`
links to an immutable snapshot of the English Wikipedia; to access it through my IPFS
gateway, you could use [https://ipfs.leotindall.com/ipfs/QmXoypizjW3WknFiJnKLwHCnL72vedxjQkDDP1mXWo6uco/wiki/](https://ipfs.leotindall.com//ipfs/QmXoypizjW3WknFiJnKLwHCnL72vedxjQkDDP1mXWo6uco/wiki/).

Setting this up was immensely easy. I installed it using `snap`:

```
sudo snap install ipfs
```

I then added a simple `systemd` unit:

```
[Unit]
Description=Interplanetary File System Daemon
After=network.target
Requires=snapd.service

[Service]
User=leo
ExecStart=/snap/bin/ipfs daemon
Restart=on-failure

[Install]
WantedBy=default.target
```

The final piece was to configure nginx to forward traffic to the daemon (which runs on
port 8080). I got a LetsEncrypt cert and added the following file to my nginx config:

```nginx
server {
    listen 443 ssl http2;
    listen [::]:443 ssl http2;
    server_name ipfs.leotindall.com;

    access_log /var/log/nginx/ipfs.leotindall.com.access.log;
    error_log /var/log/nginx/ipfs.leotindall.com.error.log;

    location / {
        proxy_pass http://localhost:8080/;
        proxy_set_header Host $host;
        proxy_buffering off;
        proxy_pass_request_headers on;
    }

    ssl_certificate /etc/letsencrypt/live/ipfs.leotindall.com/fullchain.pem; # managed by Certbot
    ssl_certificate_key /etc/letsencrypt/live/ipfs.leotindall.com/privkey.pem; # managed by Certbot
}
```

This is great for a couple reasons, but the primary one is that it terminates the connection
using HTTP2 and SSL, so my interactions with my IPFS gateway are fast and encrypted.

## The Blog

I wanted to make my blog available on IPFS, automatically and rapidly. I already have a
script that syncs my blog from its Git repository:

```bash
#!/bin/bash
set -e
# Enter the correct directory
cd ~/leotindall.com

# Update the Git repo
OLD_ID=$(git rev-parse HEAD)
git pull origin master
NEW_ID=$(git rev-parse HEAD)

# If there's no change, abort with success
if [ $OLD_ID = $NEW_ID ]; then
        echo "No change, execution finished."
        logger "$0 - leotindall.com VCS update found no changes."
        exit 0
fi

# If there was a change, rebuild the site
mkdir -p public_new/
hugo --destination ./public_new/

# Copy the files to their new destination
rm -rf public
mv public_new public
chmod 777 public
logger "$0 - leotindall.com VCS update found changes and rebuilt site."
```

This is pretty simple: grab the latest version from Git, compare the hashes, and rebuild
the website if the hashes don't match, then swap in the new version only if that
succeeded.

After a bit of experimentation, I figured out how to automatically update the site on
IPFS, as well:

```bash
# Get the peer ID, which is what IPNS will name our files
peerid=$(/snap/bin/ipfs id -f"<id>")

# Add the files to IPFS and grab the hash of the directory root
dirhash=$(/snap/bin/ipfs add -r public/ | grep public$ | cut -d" " -f2)

# Publish the directory to our node's IPNS entry
/snap/bin/ipfs name publish $dirhash

# Log to syslog
logger "$0 - leotindall.com ($dirhash) republished to IPFS and IPNS as ($peerid)"
```

Unfortunately, this wasn't the end of my efforts. I'd been using root-relative link URLs
within the blog, things like `/posts/whatever`, which break when used through an IPFS
gateway that prefixes my blog URLs with the gateway's domain and the IPFS URL. Hugo's
`relativeURLs` options fixes that.

Finally, my custom font broke, because it's in static CSS and there's no way to change the
URL per page without using JavaScript, which I don't really want to do.

So, I used a rather ugly hack to "fix" it for nested directories down to 3 levels:

```css
@font-face {
  font-family: BitterRegular;
  src: url(/fonts/bitter.woff);
}

@font-face {
  font-family: BitterRegularFB1;
  src: url(fonts/bitter.woff);
}

@font-face {
  font-family: BitterRegularFB2;
  src: url(../fonts/bitter.woff);
}

@font-face {
  font-family: BitterRegularFB3;
  src: url(../../fonts/bitter.woff);
}

body {
  font-family: BitterRegular, BitterRegularFB1, BitterRegularFB2, BitterRegularFB3, serif, 'Apple Color Emoji', 'Segoe UI Emoji', 'Segoe UI Symbol', 'Symbola';
}
```

I also realized that, since these immutable pages will be available forever (in theory),
I should add some way for people to know what version they're looking at, so I used
a Hugo template to add a "Last update" at the bottom of each page:

```
Last updated {{ now.Format "Jan 01 2006" }}
```

## The Results

All is now ready, and this blog is fully available on IPFS, at `/ipns/Qme48wyZ7LaF9gC5693DZyJBtehgaFhaKycESroemD5fNX/` (which you can [access via my gateway](https://ipfs.leotindall.com/ipns/Qme48wyZ7LaF9gC5693DZyJBtehgaFhaKycESroemD5fNX/)). Even if my server goes down, as long as these files are pinned _somewhere_, 
anyone should be able to use that IPNS name at another gateway and see the blog.

