#!/usr/bin/python3

import cgi, cgitb

form = cgi.FieldStorage()
Username = form.getvalue('Username')
email = form.getvalue('email')

print("Content-type:text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title>It's working</title>")
print("</head>")
print("<body>")
print("<h1>New python script</h1>")
print(Username)
print(email)
print("</body>")
print("</html>")