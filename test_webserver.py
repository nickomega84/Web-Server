import requests

BASE_URL = "http://localhost:8080"

tests = [
    {
        "name": "GET /",
        "method": "GET",
        "path": "/",
        "expected_status": 200,
        "expected_content_type": "text/html"
    },
    {
        "name": "GET /no-existe",
        "method": "GET",
        "path": "/no-existe",
        "expected_status": 404,
        "expected_content_type": "text/html"
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
        "expected_status": 405
    },
    {
        "name": "Path traversal attempt",
        "method": "GET",
        "path": "/../../etc/passwd",
        "expected_status": 403
    },
    {
        "name": "GET /style.css (MIME test)",
        "method": "GET",
        "path": "/style.css",
        "expected_status": 200,
        "expected_content_type": "text/css"
    },
    {
        "name": "GET /logo.png (MIME test)",
        "method": "GET",
        "path": "/logo.png",
        "expected_status": 200,
        "expected_content_type": "image/png"
    }
]

def run_tests():
    print("🔍 Running Webserv Tests\n" + "-"*60)
    passed = 0
    failed = 0
    for test in tests:
        url = BASE_URL + test["path"]
        try:
            resp = None
            method = test["method"]
            if method == "GET":
                resp = requests.get(url)
            elif method == "POST":
                resp = requests.post(url, data=test.get("data", {}))
            elif method == "DELETE":
                resp = requests.delete(url)
            elif method == "PUT":
                resp = requests.put(url)
            else:
                print(f"❌ Unsupported method in {test['name']}")
                failed += 1
                continue

            code_ok = resp.status_code == test["expected_status"]
            mime_ok = True
            if "expected_content_type" in test:
                mime_ok = resp.headers.get("Content-Type", "").startswith(test["expected_content_type"])

            if code_ok and mime_ok:
                print(f"✅ PASSED: {test['name']} (status {resp.status_code})")
                passed += 1
            else:
                print(f"❌ FAILED: {test['name']} (status {resp.status_code})")
                if not code_ok:
                    print(f"   ➤ Expected status: {test['expected_status']}, got: {resp.status_code}")
                if not mime_ok:
                    print(f"   ➤ Expected Content-Type: {test['expected_content_type']}, got: {resp.headers.get('Content-Type')}")
                failed += 1

        except Exception as e:
            print(f"❌ ERROR in {test['name']}: {e}")
            failed += 1

    print("-" * 60)
    print(f"✅ Total Passed: {passed}")
    print(f"❌ Total Failed: {failed}")
    print("-" * 60)

if __name__ == "__main__":
    run_tests()
