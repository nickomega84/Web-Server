#!/usr/bin/python

import cgi, cgitb

form = cgi.FieldStorage()

username = form["username"].value
email = form["email"].value

print("Content-type:text/html\r\n\r\n")

print("<html>")
print("<head>")
print("<title>Funcionaa</title>")
print("</head>")
print("<body>")
print("<h1>Bienvenidoo al mundo de Skylanders</h1>")
print(username)
print(email)
print("</body>")
print("</html>")