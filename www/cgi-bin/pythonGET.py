#!/usr/bin/python3

import os
import sys

print("Content-Type: text/html\n")
print("<html>")
print("<head><title>Python GET</title></head>")
print("<body>")
print("<h1>GET Request Processed by Python CGI</h1>")

query_string = os.environ.get("QUERY_STRING", "")
if query_string:
    print(f"<p>Query String: {query_string}</p>")
    params = query_string.split('&')
    print("<ul>")
    for param in params:
        if '=' in param:
            key, value = param.split('=', 1)
            print(f"<li>{key}: {value}</li>")
        else:
            print(f"<li>{param}</li>")
    print("</ul>")
else:
    print("<p>No query string provided.</p>")

print("</body>")
print("</html>")

