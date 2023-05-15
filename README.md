# Webserv

:warning: **Please be advised that the intended operating system for this project is Linux as it utilizes epoll.**

## Introduction

The Hypertext Transfer Protocol (HTTP) is an application protocol for distributed, collaborative, hypermedia information systems.

HTTP is the foundation of data communication for the World Wide Web, where hypertext documents include hyperlinks to other resources that the user can easily access. For example, by a mouse click or by tapping the screen in a web browser.

HTTP was developed to facilitate hypertext and the World Wide Web.

The primary function of a web server is to store, process, and deliver web pages to clients. The communication between client and server takes place using the Hypertext Transfer Protocol (HTTP).

Pages delivered are most frequently HTML documents, which may include images, style sheets, and scripts in addition to the text content.

Multiple web servers may be used for a high-traffic website.

A user agent, commonly a web browser or web crawler, initiates communication by requesting a specific resource using HTTP and the server responds with the content of that resource or an error message if unable to do so. The resource is typically a real file on the server’s secondary storage, but this is not necessarily the case and depends on how the webserver is implemented.

While the primary function is to serve content, full implementation of HTTP also includes ways of receiving content from clients. This feature is used for submitting web forms, including the uploading of files.

## Features
- Configuration file that can be created or edited
- Non-blocking
- Error handling
- Persistent server
- GET, POST, DELETE methods
- Upload files
- PHP CGI
- Multiple ports

## Usage
```git clone https://github.com/Adebureaux/webserv.git```\
```make```\
```./webserv```\

You can edit webserv.conf file to edit your server configuration. You can also create your own configuration file and use it with: ```./webserv myconf.conf```

## Rules
- Use explicit variables and functions names
- Use explicit messages for commits
- Working on different branches, then merge changes on the master branch
- Provide documentation in DOCS.md

## Ressources
[DOCS.md](DOCS.md)
