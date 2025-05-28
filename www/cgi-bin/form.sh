#!/bin/bash
echo "Content-type: text/html"
echo

# Lee los datos del cuerpo
read -n "$CONTENT_LENGTH" POST_DATA

# Extrae el valor de "name"
name=$(echo "$POST_DATA" | sed -n 's/^.*name=\([^&]*\).*$/\1/p' | sed 's/+/ /g' | sed 's/%20/ /g')

echo "<html><body>"
echo "<p>Recibido: $name</p>"
echo "</body></html>"
