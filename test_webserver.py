import requests

BASE_URL = "http://localhost:8080"

tests = [
    {
        "name": "GET /",
        "method": "GET",
        "path": "/",
        "expected_status": 200
    },
    {
        "name": "GET /no-existe",
        "method": "GET",
        "path": "/no-existe",
        "expected_status": 404
    },
    {
        "name": "POST /upload",
        "method": "POST",
        "path": "/upload",
        "data": {"field": "value"},
        "expected_status": 200
    },
    {
        "name": "DELETE /index.html",
        "method": "DELETE",
        "path": "/index.html",
        "expected_status": 200
    },
    {
        "name": "PUT /index.html",
        "method": "PUT",
        "path": "/index.html",
        "expected_status": 405  # Método no permitido
    },
    {
        "name": "Path traversal attempt",
        "method": "GET",
        "path": "/../../etc/passwd",
        "expected_status": 403  # O 404 si lo bloqueas correctamente
    }
]

def run_tests():
    print("🔍 Running Webserv Tests\n" + "-"*40)
    for test in tests:
        url = BASE_URL + test["path"]
        try:
            if test["method"] == "GET":
                resp = requests.get(url)
            elif test["method"] == "POST":
                resp = requests.post(url, data=test.get("data", {}))
            elif test["method"] == "DELETE":
                resp = requests.delete(url)
            elif test["method"] == "PUT":
                resp = requests.put(url)
            else:
                print(f"❌ Unsupported method in {test['name']}")
                continue

            result = "✅ PASSED" if resp.status_code == test["expected_status"] else "❌ FAILED"
            print(f"{result}: {test['name']} (got {resp.status_code}, expected {test['expected_status']})")

        except Exception as e:
            print(f"❌ ERROR in {test['name']}: {e}")
    print("-"*40 + "\n✅ Testing completed.")

if __name__ == "__main__":
    run_tests()
