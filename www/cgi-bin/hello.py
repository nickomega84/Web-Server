#!/usr/bin/env python3
import os
import urllib.parse

print("Content-type: text/html\n")

query = os.environ.get('QUERY_STRING', '')
params = urllib.parse.parse_qs(query)
name = params.get('name', [''])[0]

print(f"<html><body><p>Hello, {name}!</p></body></html>")
