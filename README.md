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

## LUA example

The server can read and run .lua files for server-side scripting, here's an example.

```lua
-- Function called by the server
function F(request, response)
    -- Set a custom header
    setResponseHeader(response, "Content-Type", "application/json");
    
    -- Request's parameters are not parsed by default,
    -- you have to request them manually
    local params = getRequestParams(request.p);
    -- Do something with the params...
    
    -- Create a JSON containing the request's path
    local body = "{\"path\":\"" .. request.path .. "\"}";
    
    -- Return what will be sent to the client
    return body;
end
```

### The F function

The F function can receive 2 tables as parameters: `request` and `response`.

Request contains the following values:
- `method` - Request's method
- `http_version` - HTTP version used by the client
- `path` - Path
- `body` - Content of the request
- `p` - Pointer to the Request instance

Response is only a pointer to the Response instance.

### Response functions

#### setResponseHeader
```
-- Prototype:
void setResponseHeader(Response *response, string key, string value);

-- Example:
setResponseHeader(response, "Content-Type", "application/json");
```

#### sendResponseHeaders
Manually sends the response's headers.
```
-- Prototype:
void sendResponseHeaders(Response *response);

-- Example:
sendResponseHeaders(response);
```

#### sendResponseBody
Manually sends response's body.
> This function MUST be called after the sendResponseHeaders was called
```
-- Prototype:
void sendResponseBody(Response *response, string content);

-- Example:
sendResponseBody(response, "Hello World!");
```

### Request functions

#### getRequestHeaders
```
-- Prototype:
table getRequestHeaders(Request *request);

-- Example:
local headers = getRequestHeaders(request.p);
local ua = headers["User-Agent"];
```

#### getRequestParams
```
-- Prototype:
table getRequestParams(Request *request);

-- Example:
local params = getRequestParams(request.p);
local var = params["var"];
```

