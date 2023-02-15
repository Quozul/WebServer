-- Function called by the server
function GET(request, response)
    -- Set a custom header
    setResponseHeader(response, "Content-Type", "application/json");

    -- Request's parameters are not parsed by default,
    -- you have to request them manually
    local params = getRequestParams(request.p);
    -- Do something with the params...

    -- Create a JSON containing the request's path
    local body = "{\"path\":\"" .. request.path .. "\",\"param\":\"" .. params.hello .. "\"}";

    -- Return what will be sent to the client
    return body;
end