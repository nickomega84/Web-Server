#!/usr/bin/env python3
import sys
import os
import urllib.parse

print("Content-type: text/html\n")

length = int(os.environ.get('CONTENT_LENGTH', 0))
post_data = sys.stdin.read(length)
params = urllib.parse.parse_qs(post_data)
name = params.get('name', [''])[0]

print(f"<html><body><p>Recibido: {name}</p></body></html>")
