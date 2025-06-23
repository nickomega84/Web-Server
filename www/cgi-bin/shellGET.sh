#!/bin/sh
echo "Content-type: text/html"
echo ""

echo "<html><body><h1>CGI ejecutado correctamente</h1>"

if [ -n "$QUERY_STRING" ]; then
    echo "<p>Query String: $QUERY_STRING</p>"
    echo "<ul>"
    OLDIFS=$IFS
    IFS='&'
    for i in $QUERY_STRING; do
        echo "<li>$i</li>"
    done
    IFS=$OLDIFS
    echo "</ul>"
else
    echo "<p>No query string provided.</p>"
fi

echo "</body></html>"

#curl "http://localhost:8081/cgi-bin/shellGET.sh?id=123&categoria=electronica"
#http://localhost:8081/cgi-bin/shellGET.sh?busqueda=web&page=2