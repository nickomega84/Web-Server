# #!/bin/bash

# echo "Content-Type: text/html"
# echo ""
# echo "<html>"
# echo "<head><title>Shell GET</title></head>"
# echo "<body>"
# echo "<h1>GET Request Processed by Shell CGI</h1>"

# if [ -n "$QUERY_STRING" ]; then
#     echo "<p>Query String: $QUERY_STRING</p>"
#     echo "<ul>"
#     IFS='&' read -ra ADDR <<< "$QUERY_STRING"
#     for i in "${ADDR[@]}"; do
#         echo "<li>$i</li>"
#     done
#     echo "</ul>"
# else
#     echo "<p>No query string provided.</p>"
# fi

# echo "</body>"
# echo "</html>"

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


#curl "http://localhost:8080/cgi-bin/shellGET.sh?id=123&categoria=electronica"
#http://localhost:8080/cgi-bin/shellGET.sh?busqueda=web&page=2