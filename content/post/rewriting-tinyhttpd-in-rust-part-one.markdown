---
date: 2016-10-21 14:07:09+00:00
slug: rewriting-tinyhttpd-in-rust-part-one
title: Rewriting tinyhttpd in Rust, Part One
categories:
- Programming
- Rust
- Networking
- Open Source
description: The beginning of the process of rewriting an old C webserver in Rust.
---

In 1999, J. David Blackstone, or, as he is known online, [jdavidb](https://slashdot.org/~jdavidb), was taking CSE 4344 (Network Concepts) at UT Arlington. Those were the glory days of Sparc Solaris, and Blackstone wrote, for his college course, a C program called **tinyhttpd**. It is, essentially, a very short version of the immensely complex programs that seem run the world these days: web servers. Unlike the million-line behemoths (think Apache, nginx, et cetera), tinyhttpd is a HTTP 1.1 web server in 532 lines of well-commented C.

HTTP 1.1 is a ubuqitously supported protocol that is useful for a great many applications, and in this modern era of embedded (a.k.a "Internet of Things") computing applications, small web servers have never been more important.

This program is also a small, manageable example of a legacy application - an old program written for an obsolete operating system that still gets the job done, but exposes any organization using it to not only the cost of maintaining ancient operating systems and hardware, but also to the risk of the security vulnerabilities present in tinyhttpd itself and the software it needs to run.

For the purposes of these posts, I'll be looking at tinyhttpd from the perspective of a company that uses it internally, and wants to transition to a more modular, portable, and maintainable design, rather than one which either ships it as a product or buys it as a product from another company and wants to replace it; these situations are similar, but have additional challenges.

The first thing to do is to analyze the existing source. I've gone ahead and created a GitHub repository to host both the old and new source code, and I'll link to specific commits in these posts. For instance, [here](https://github.com/noracodes/rtinyhttpd/tree/af6ea072d72d8f86f0648d98eef9e1fb80782479/legacy) is the commit with nothing but the unmodified source of the legacy app.

The first thing to do is to build the existing app. In order not to clutter the repository with object files, I [created a .gitignore file](https://github.com/noracodes/rtinyhttpd/commit/08c890d28bac42c04988d8d75ad46ac79b6414f1) from GitHub's default C gitignores. Now all I have to do is run `make`, right?

    
    11:32:58: leo [~/Projects/rtinyhttpd/legacy]
    $ make
    gcc -W -Wall -lpthread -o httpd httpd.c
    /tmp/ccbqEOVd.o: In function `main':
    httpd.c:(.text+0x1a85): undefined reference to `pthread_create'
    collect2: error: ld returned 1 exit status
    Makefile:4: recipe for target 'httpd' failed
    make: *** [httpd] Error 1


What's this, it doesn't compile? Well, you'll remember I mentioned it was written for an ancient version of Sparc Solaris - that's the whole reason we're rewriting it. Luckily, the original author anticipated this. Looking at `legacy/httpd.c` (where the error is), I see this comment at the top:

```c  
/* This program compiles for Sparc Solaris 2.6.
 * To compile for Linux:
 * 1) Comment out the #include <pthread.h> line.
 * 2) Comment out the line that defines the variable newthread.
 * 3) Comment out the two lines that run pthread_create().
 * 4) Uncomment the line that runs accept_request().
 * 5) Remove -lsocket from the Makefile.
 */
```

I made a note of this in my analysis folder and [made those changes](https://github.com/noracodes/rtinyhttpd/commit/a1ae783977550ce4752243f34513a7c855f27492) - except that they didn't apply. The makefile didn't have `-lsocket`, and there was only one occurrence of `pthread_create`. They did make the app build, but it didn't work!

In order to figure out what's happening, I looked up `pthread_create` on man7.org. It's part of the POSIX threading API, and it is definitely available on Linux. Furthermore, if we look at the main() function, we can see why commenting out those lines caused a problem - it's an infinite loop that does nothing but accept connections!

```c    
while (1)
{
    client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
    if (client_sock == -1)
    {
        error_die("accept");
    }

    // Commented out in order to build on Linux
    /* if (pthread_create(&newthread , NULL, (void *)accept_request, (void *)&client_sock) != 0)
    {
        perror("pthread_create");
    }
    */
}
```

So, we need to get POSIX threads working to make this app run properly. (Note that this problem isn't an uncommon one when looking at legacy apps; there is often not a good set of build instructions.)

In our case, luckily, this is easy: just revert the commenting and change `-lpthread` in the Makefile to` -pthread`, as mentioned [on the manual page](http://man7.org/linux/man-pages/man3/pthread_create.3.html).

[Doing this](https://github.com/noracodes/rtinyhttpd/commit/8e938ec4943fc859d8bd7eff0dafd13c7a23b168) allows the app to build and run correctly, binding to port 9999. When I open localhost:9999 in my web browser, I get a page back. Success!

Now that we have a compiling and running version of the legacy tinyhttpd, it's time to go through the source code. Luckily for us, tinyhttpd is entirely contained in a single file. Let's start off with the top:

```c
/* J. David's webserver */
/* This is a simple webserver.
 * Created November 1999 by J. David Blackstone.
 * CSE 4344 (Network concepts), Prof. Zeigler
 * University of Texas at Arlington
 */
/* This program compiles for Sparc Solaris 2.6.
 * To compile for Linux:
 * 1) Comment out the #include <pthread.h> line.
 * 2) Comment out the line that defines the variable newthread.
 * 3) Comment out the two lines that run pthread_create().
 * 4) Uncomment the line that runs accept_request().
 * 5) Remove -lsocket from the Makefile.
 */
```

Here is some information which will often be included in legacy programs - some short information about the author and purpose of the program, and some (in this case out of date and inaccurate) information about building and running the program. [Removing the misleading lines](https://github.com/noracodes/rtinyhttpd/commit/185a968f0f1307968ada452e86f17f1e23b72225) makes this section a lot more concise and is probably a good idea.

Skipping the `#include`s, which aren't very helpful in this case, we find two `#define` statements:

```c    
#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"
```

The `SERVER_STRING` definition is pretty straightforward; it's an identifier of the software, which will be sent to clients. In our version, I would prefer to not include the `\r\n` terminator in the definition itself. As to the `ISspace` definition, though, I'm not immediately sure. A quick search of the source shows no definition of a function `isspace` taking an integer, so it's probably coming from one of the includes.

If this program had multiple files, I'd search through them next; but, as there are none, I'm going straight to the Internet. [Turns out](http://www.tutorialspoint.com/c_standard_library/c_function_isspace.htm), it does just what you'd expect - it checks if a given integer represents whitespace or not. This definition simply allows calling it directly on `char` values without writing out an explicit cast every time. I've [made a note of this](https://github.com/noracodes/rtinyhttpd/commit/7c9fe4d96e1735fa3e12c941f01f0da968ac4e66) in my analysis documents.

After the head macros, we can see explicit definitions of all the functions used in the program.

```    
void accept_request(void *);
void bad_request(int);
void cat(int, FILE *);
void cannot_execute(int);
void error_die(const char *);
void execute_cgi(int, const char *, const char *, const char *);
int get_line(int, char *, int);
void headers(int, const char *);
void not_found(int);
void serve_file(int, const char *);
int startup(u_short *);
void unimplemented(int);
```

Because they have no comments, these definitions are not particularly useful, so let's go down to the bottom of the page and look at which functions are called in the program's entry function, `main()`.

```c    
int main(void) {
    int server_sock = -1;
    int client_sock = -1;
    u_short port = 9999;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);
    pthread_t newthread;

    signal(SIGPIPE, SIG_IGN);

    server_sock = startup(&port);
    printf("httpd running on port %d\n", port);

    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
        if (client_sock == -1)
        {
            error_die("accept");
        }

        if (pthread_create(&newthread , NULL, (void *)accept_request, (void *)&client_sock) != 0)
        {
            perror("pthread_create");
        }

    }

    close(server_sock);
    return(0);
}
```

Let's break this down further. This function takes void, meaning that the program has no arguments or command line options. This probably means it's not very customizable, something I'd like to change in the rewritten version.

After the function signature come the definitions of some local variables: `server_sock` and `client_sock`, `port`, `client_name`, `client_name_len`, and `newthread`. `server_sock` and `client_sock` are just `int`s, but they represent file handles, as we'll see in a moment. `port` is clearly a port number. `client_name` is the address of the client, and `client_name_len` is its length.

Below that, the program uses `signal()` to ignore `SIGPIPE`, the signal that programs receive when they write to a file handle which has been closed. It seems to me that this should be handled more appropriately in the rewrite.

Immediately afterward, the `server_sock` variable is filled by the result of the function `startup`, which is given a pointer to the port number. This seems odd to me - why does it need a reference and not just the value? - so I look at that function's definition. It is commented with:

```c
/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port. If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
/**********************************************************************/
```

That makes more sense now - it allows dynamically generating a port number. That's useful, but the functionality isn't exposed through the command line interface, which is annoying. In our program, I'd like to expose that, and I'd also like to move away from the C convention of modifying inputs. In the rewrite, I think I'll return a tuple. Since this is a fairly complex idea, I'll take this time to [write some notes down](https://github.com/noracodes/rtinyhttpd/commit/c76164487b6f82403e9f179f1a8092fdf3ab9232).

That's enough to understand a bit more about `main`. After a simple status message, the program moves on to the main loop:

```c
while (1)
{
    client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
    if (client_sock == -1)
    {
        error_die("accept");
    }

    if (pthread_create(&newthread , NULL, (void *)accept_request, (void *)&client_sock) != 0)
    {
        perror("pthread_create");
    }
}
```

This is an infinite loop which accepts a connection, as can be seen if we look up `accept()`, which is where `client_sock` gets its value. It returns a file handle representing the socket. It returns -1 if it fails for some reason, and the next few lines check for that eventuality. This is another suboptimal design imposed by C's lack of algebraic data types - in Rust, this idea can be represented with an `Option` or a `Result`.

The next few lines try (and handle errors for) spawning a new thread that runs `accept_request`. Looking at the comments here is not quite as illuminating as one might hope:

```c
/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return. Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
```

I'm not really sure what processing the request "appropriately" entails. For now, though, it's enough to know that this is the main function for dealing with incoming requests.

The only code after this is cleanup code we won't need in the rewrite, so we have enough info to [write a short pseudocode summary](https://github.com/noracodes/rtinyhttpd/commit/58d96df64f7d015d80853952c8f77cf851d1eee5) of the server:

```   
Open and configure a server socket

Until the process is terminated:
     Wait for a client to request a connection
     Try to open a connection with that client
     Open a new thread to deal with that connection's request
```

That's a lot simpler than one might have imagined from the length of this post, and it doesn't tell us much about the actual functionality of the server, but it gives you a good idea of the process one often has to go through to understand legacy code.

Now that we have examined the basic structure of the server's execution, I'm going to dive into the actual functionality and logic of the server, which is encapsulated primarily in the function `accept_request`, whose signature is `void accept_request(void *arg)`. This is a signature that is totally unrevealing, and which in Rust would require an unsafe block; this function takes a raw pointer with no type information at all. We'll have to do quite a bit of work to understand what the function actually does.

First of all, are there any clues about what the argument might represent? Well, we can look back at how the function is called:

`pthread_create(&newthread , NULL, (void *)accept_request, (void *)&client_sock)`

This is a little complicated, but essentially a new thread is being spawned which will execute `accept_request(&client_sock)`. This is the only place this function is called, so the argument is presumably expected to be only a pointer to an integer file descriptor to a socket - but the compiler knows none of that! That's a lot of unchecked assumptions and unsafe memory access. Rust, and more importantly the Rust standard library, has better invariant checking, which will make the re-implementation a great deal safer and thus easier to extend.

Moving on to the body of the function, we see the creation of a lot of local variables which I'll go into as they're used. It is important to note, though, that there is a group of buffers created with absolute lengths. These appear, at first glance, to be possible introduction points for overflow vulnerabilities - something that is mitigated by the Rust idiom of defaulting to using `Vec`s instead of arrays.

One of these buffers, of length 1024, is populated using the function `get_line`, which, according to the comments above its definition, reads a line into a buffer and null-terminates it, with length checking, and returns the number of bytes stored. That buffer is printed and dissected over the course of the next 90 lines or so.

Now that it's clear how I dissect each line of code, I'm going to move a bit faster, translating the entire program into pseudocode function by function. What we currently have is this:

```
Open and configure a server socket

Until the process is terminated:
 Wait for a client to request a connection
 Try to open a connection with that client
 Open a new thread to deal with that connection's request
```

And we're examining the idea of "dealing with the client". This is all done in the `accept_connection` function, whose pseudocode looks a bit like this:

```
accept_request takes a socket connecting to the client
     read a line from the client
     log (to stdout) the received request
     
     copy what is assumed to be the method into another buffer
     if the method isn't GET or POST:
          handle an unimplemented method somehow
     if the method is POST:
          make a note that this request will require executing a CGI script

     copy what is assumed to be the URL into another buffer
     if the method is GET and the url has a ? in it:
          make a note that this request will require executing a CGI script
     construct the path to the requested resource by prepending "htdocs" to the url
          (note - I'd like to make this customizable in the rewrite)

     if the URL is /:
          add 'index.html' to the path

     if the resource being requested doesn't exist:
          handle a not found error somehow
     if the resource is a directory:
          append "/index.html" to the file path
          (note- the existence of THIS file isn't checked!)
     if the file is executable:
          make a note that this request will require executing a CGI script
     
     if this request requires executing a CGI script:
          handle executing a CGI script
     otherwise:
          handle serving a static file
```

This analysis is pretty revealing: essentially all this function does is determine some properties of a request and then pass it off to be handled appropriately by other functions.

This particular function should be fairly easy to translate into more efficient Rust code, especially if we look at using Rust's more advanced type system. In particular, rather than having a large number of buffers, I'd like to use slices and ADTs. For example, I might create an enum `HTTPMethod`:

```rust
enum HTTPMethod {
     Get,
     Post,
     Other
}
```

Then I could use a `match` expression to appropriately dispatch the request, whether to the static server, CGI handler, or error response.

In the next post, I'll take a look at the handler functions and how they handle the various conditions and actions a request can trigger - unimplemented method, resource not found, static file serving, and CGI execution. I'll also discuss the Rust idioms that can be used to better model the intended behavior and internal structure of this server.
