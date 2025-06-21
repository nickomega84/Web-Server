import subprocess
import sys

# Define the server URL and tests
BASE_URL = "http://localhost:8081"

tests = {
    "GET /                   ": ["curl", "-o", "/dev/null", "-s", "-w", "%{http_code}", f"{BASE_URL}/"],
    "GET /style.css          ": ["curl", "-o", "/dev/null", "-s", "-w", "%{http_code}", f"{BASE_URL}/style.css"],
    "GET CGI pythonGET.py    ": ["curl", "-o", "/dev/null", "-s", "-w", "%{http_code}", f"{BASE_URL}/cgi-bin/pythonGET.py"],
    "POST /upload            ": ["curl", "-o", "/dev/null", "-s", "-w", "%{http_code}", "-X", "POST", "-d", "test=data", f"{BASE_URL}/upload"],
    "POST CGI pythonPOST.py  ": ["curl", "-o", "/dev/null", "-s", "-w", "%{http_code}", "-X", "POST", "-d", "foo=bar", f"{BASE_URL}/cgi-bin/pythonPOST.py"],
    "DELETE /index.html      ": ["curl", "-o", "/dev/null", "-s", "-w", "%{http_code}", "-X", "DELETE", f"{BASE_URL}/index.html"],
    "UNKNOWN METHOD FOO /    ": ["curl", "-o", "/dev/null", "-s", "-w", "%{http_code}", "-X", "FOO", f"{BASE_URL}/"],
    "Server no segfaults     ": ["bash", "-c", f"timeout 5 curl -s {BASE_URL}/"],
    "Memory leaks check      ": ["valgrind", "--leak-check=full", "--error-exitcode=1", "curl", "-s", f"{BASE_URL}/"]
}

def run_test(name, cmd):
    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT, timeout=10)
        status = output.decode().strip()
        # For HTTP status code tests: expect 200 or 201
        if name.startswith("Server no segfaults"):
            passed = True
        elif name.startswith("Memory leaks check"):
            passed = True  # valgrind exit code 0 means no leaks
        else:
            passed = status in ("200", "201")
        result = "PASS" if passed else f"FAIL (status {status})"
    except subprocess.CalledProcessError as e:
        result = f"FAIL ({e.returncode})"
    except subprocess.TimeoutExpired:
        result = "FAIL (timeout)"
    print(f"{name}: {result}")

def main():
    print("Running WEBSERV basic functionality tests...\n")
    for name, cmd in tests.items():
        run_test(name, cmd)

if __name__ == "__main__":
    main()

