function f()
    local body = [[
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">

    <!-- Import fonts -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Merriweather:wght@700&family=Quicksand:wght@400;600&display=swap" rel="stylesheet">

    <title>Custom web server</title>
    <link rel="stylesheet" type="text/css" href="/main.css">
</head>
<body>
    <h1>Hello World!</h1>
    <p>This page is generated with a server-side LUA script</p>
</body>
</html>
    ]];
    return body;
end