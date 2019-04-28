---
date: 2016-12-24 05:04:24+00:00
slug: socketserver-the-python-networking-module-you-didnt-know-you-needed
title: 'socketserver: the Python networking module you didn''t know you needed'
categories:
- Programming
- Networking
- Python
description: The Python 3 socketserver module makes network servers much easier and more readable than similar code using raw sockets.
---

I occasionally spend time randomly surfing the Python standard library docs; there is a lot of useful functionality included in the language's standard distribution, such as, for instance, the [`socketserver`](https://docs.python.org/3/library/socketserver.html) module, which I didn't know about until this evening and which is one of the most useful I've seen in a while. As ever, the docs are straightforward in their self-description:


<blockquote>The `socketserver` module simplifies the task of writing network servers.</blockquote>


This is something of an understatement. To demonstrate this, here is a simple CaaS (capitalization as a service) server written with `socketserver` and one with `socket`:

<pre><code class="python3">
    import socket
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('', 50006))
        s.listen(1)
        conn, addr = s.accept()
        with conn:
            while True:
                data = conn.recv(1024)
                if not data: break
                conn.sendall(data.upper())
</code></pre>


And here is the same functionality with `socketserver`:

<pre><code class="python3">
    import socketserver
    
    class CaaSHandler(socketserver.StreamRequestHandler):
        def handle(self):
            data = self.rfile.readline()
            self.wfile.write(data.upper())
    
    if __name__ == "__main__":
        server = socketserver.TCPServer(('', 50007), CaaSHandler)
        server.serve_forever()
</code></pre>

Both of these take connections synchronously and sequentially, capitalize the data they recieve, and return it. The main difference is that the `socketserver` version can accept as much data as there is memory, while the `socket` version can accept only a limited amount (1024 bytes in this example).

This is because `socketserver`'s `StreamRequestHandler` provides the file-like objects `rfile` and `wfile` which expose all the normal luxuries of Python 3 files, like `readline` and `read`. The parent class of the handler you write will deal with setting the buffer size, looping until a newline or EOF is encountered, and dealing with client-first and server-first protocols. We could just as easily add a welcome message/prompt to the program; just make the `CaaSHandler` class look like this:

<pre><code class="python3">
    class CaaSHandler(socketserver.StreamRequestHandler):
        def handle(self):
            self.wfile.write(b"Enter some data to be capitalized:\n")
            data = self.rfile.readline()
            self.wfile.write(data.upper())
</code></pre>

without any changes to the client's behavior. Adding that functionality in the `socket` version is somewhat nontrivial; how, for instance, one would handle both clients that expect to send data first and clients that expect to receive it first is less than obvious.

The second useful facility that `socketserver` provides is the `xxxServer` classes. I use `TCPServer` here, to which I passed a tuple of `(hostname, portnumber)` and the name of my handler class,` CaaSHandler.` I could also have used `UDPServer` for datagrams or `UnixStreamServer`/`UnixDatagramServer `for [Unix sockets](https://en.wikipedia.org/wiki/Unix_domain_socket).

The `socketserver` module also provides mixins for threading and forking servers, which makes writing asynchronous network services much less painful than using `socket` and `threading` or even `asyncio`.
