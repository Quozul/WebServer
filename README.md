# Quozul.Web

Web server made in C++.

## Compiling

> This program is designed to compile x64 bit systems running Linux and Windows (using msvc_x64).  
> For Windows you might need to download `lua-5.3.3_Win64_dll14_lib.zip` from [LuaBinaries](https://sourceforge.net/projects/luabinaries).

1. Install dependencies
```sh
apt install build-essential liblua5.3-dev libssl-dev cmake
```
2. Install submodules
```sh
git submodule update --init --recursive
```
3. Run cmake

## Features
- HTTP over SSL
- Server-side scripting using LUA language
- Parallel request handling using threads

## Config file

Default config file:
> The configuration file might change in the future with new features being added.  
> No default file is not automatically created if it does not exist.  
> Some parameters might not be used currently.

```json
{
  "cert": "/path/to/cert/file",
  "key": "/path/to/key/file",
  "server": "/path/to/server/folder",
  "port": 8443, // Port on which the server is running
  "threads": 8, // Maximum amount of threads to use for request handling
  "mime_types": "/etc/mime.types", // mime.types can be found here https://raw.githubusercontent.com/apache/httpd/trunk/docs/conf/mime.types
  "serve_lua": true, // Defines if raw .lua files can be served, can be a security issue
  "partial_download": true // Defines if the server allow partial downloads by default
}
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

### The LUA function

> Upcoming versions will feature `<method>()` functions (such as `GET()`) that the server will use to respond the request's method.

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
