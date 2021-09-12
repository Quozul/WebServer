# WebServer

Web server made in C++.

## Features
- HTTP over SSL
- Server-side scripting using LUA language
- Parallel request handling using threads

## Config file

Default config file:
> The config file does not support comments!

```sh
cert=/path/to/cert/file
key=/path/to/key/file
server=/path/to/server/folder
port=443  # Port on which the server is running

# The following options are not yet implemented
nolua=false  # Prevent the server from responding with the raw .lua file
mimefile=/path/to/mime.types  # mime.types can be found here https://raw.githubusercontent.com/apache/httpd/trunk/docs/conf/mime.types
memcache=1024  # Maximum amount of memory the server can use as a file cache, 0 for none
threads=8  # Maximum amount of threads to use for request handling
```