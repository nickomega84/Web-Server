#!/usr/bin/env bash
###############################################################################
# Smoke-tests para verificar:
#  1. Ruta válida          → 200
#  2. Path-traversal       → 403/404
#  3. Symlink escapista    → 403
###############################################################################

BIN="./webserv"                 # binario
CONF="./server.conf"            # tu .conf
PORT=8081
ROOT="./www"
PASS=0
FAIL=0

# ---------- colores ----------
green()  { printf "\e[32m%s\e[0m\n" "$*"; }
red()    { printf "\e[31m%s\e[0m\n" "$*"; }

# ---------- helpers ----------
expect_code () {   # $1=desc  $2=url  $3=expected list (espaciado)
    local code
    code=$(curl -s -o /dev/null -w '%{http_code}' "http://127.0.0.1:$PORT$2")
    for exp in $3; do
        [[ "$code" == "$exp" ]] && { green "[PASS] $1 ($code)"; ((PASS++)); return; }
    done
    red   "[FAIL] $1 (got $code, expected $3)"
    ((FAIL++))
}

cleanup () {
    [[ -n "$WS_PID" ]] && kill "$WS_PID" 2>/dev/null
    rm -f "$ROOT/leak"                     # borra symlink
}
trap cleanup EXIT INT TERM

# ---------- arrancar servidor ----------
"$BIN" "$CONF" & WS_PID=$!
sleep 0.3   # tiempo a que escuche

# ---------- preparar entorno ----------
ln -s /etc "$ROOT/leak"   # symlink malicioso → /etc

# ---------- TESTS ----------
expect_code "static OK"           "/index.html"         "200"
expect_code "path-traversal"      "/../../etc/passwd"   "403 404"
expect_code "symlink escape"      "/leak/passwd"        "403"

# ---------- resumen ----------
echo
[[ $FAIL -eq 0 ]] && green "✔ Todos los tests pasaron ($PASS)" \
                  || red   "✖ $FAIL tests fallaron / $PASS pasaron"
exit $FAIL

