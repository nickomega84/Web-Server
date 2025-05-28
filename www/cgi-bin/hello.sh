#!/bin/bash
echo "Content-type: text/html"
echo

# Extrae el parámetro "name" del QUERY_STRING
name=$(echo "$QUERY_STRING" | sed -n 's/^.*name=\([^&]*\).*$/\1/p' | sed 's/+/ /g' | sed 's/%20/ /g')

echo "<html><body>"
echo "<p>Hello, $name!</p>"
echo "</body></html>"
