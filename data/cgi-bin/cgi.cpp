#include <iostream>

int main() {
    std::cout << "Content-type:text/html\r\n\r\n" << std::endl;
    std::cout << "<html>\n" << std::endl;
    std::cout << "<head>\n" << std::endl;
    std::cout << "<title>Hello World" << std::endl;
    std::cout << "</head>" << std::endl;
    std::cout << "<body>" << std::endl;
    std::cout << "<h1>Helloooo</h1>" << std::endl;
    std::cout << "</body>" << std::endl;
    std::cout << "</html>";
    std::cout << std::endl;
    return 0;
}