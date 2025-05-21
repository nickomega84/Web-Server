
import time
import requests

BASE_URL = "http://localhost:8080"

tests = [
    # 🔹 Functional Tests
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
    },

    # 🔐 Security Tests
    {
        "name": "Path traversal attempt",
        "method": "GET",
        "path": "/../../etc/passwd",
        "expected_status": 403
    },
    {
        "name": "SQL Injection attempt",
        "method": "POST",
        "path": "/login",
        "data": {"username": "' OR 1=1 --", "password": "pass"},
        "expected_status": 403
    },
    {
        "name": "XSS attempt",
        "method": "GET",
        "path": "/?q=<script>alert(1)</script>",
        "expected_status": 400
    },

    # 🍪 Session & Cookie Tests
    {
        "name": "GET / with session cookie",
        "method": "GET",
        "path": "/",
        "cookies": {"SESSIONID": "PKdhtXMmr18n2L9K"},
        "expected_status": 200
    },
    {
        "name": "POST /secure with invalid session",
        "method": "POST",
        "path": "/secure",
        "cookies": {"SESSIONID": "invalid123"},
        "expected_status": 401
    },

    # 🧪 Custom Header Test
    {
        "name": "GET / with custom header",
        "method": "GET",
        "path": "/",
        "headers": {"X-Custom-Test": "42webserv"},
        "expected_status": 200
    }
]

def run_tests():
    passed = 0
    failed = 0
    print("\n🔍 Running Webserv Tests")
    print("-" * 80)
    for test in tests:
        url = BASE_URL + test["path"]
        method = test["method"]
        headers = test.get("headers", {})
        cookies = test.get("cookies", {})
        data = test.get("data", {})
        print(f"🧪 Test: {test['name']}")
        try:
            start = time.time()
            resp = requests.request(method, url, headers=headers, cookies=cookies, data=data)
            elapsed = round(time.time() - start, 4)
            status_ok = resp.status_code == test["expected_status"]
            content_type_ok = True
            if "expected_content_type" in test:
                content_type_ok = resp.headers.get("Content-Type", "").startswith(test["expected_content_type"])
            if status_ok and content_type_ok:
                print(f"✅ PASSED in {elapsed}s — Status: {resp.status_code}")
                passed += 1
            else:
                print(f"❌ FAILED in {elapsed}s — Status: {resp.status_code}")
                if not status_ok:
                    print(f"   ➤ Expected: {test['expected_status']}, Got: {resp.status_code}")
                if not content_type_ok:
                    print(f"   ➤ Expected Content-Type: {test['expected_content_type']}, Got: {resp.headers.get('Content-Type')}")
                failed += 1
        except Exception as e:
            print(f"❌ ERROR — {e}")
            failed += 1
        print("-" * 80)

    print(f"🎯 Total Passed: {passed}")
    print(f"❌ Total Failed: {failed}")
    print(f"📊 Total Tests: {len(tests)}")

if __name__ == "__main__":
    run_tests()
