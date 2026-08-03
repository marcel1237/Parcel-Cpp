#include <iostream>
#include <memory>

class Resource {
public:
    Resource() { std::cout << "Resource Created\n"; }
    ~Resource() { std::cout << "Resource Destroyed\n"; }
};

int main() {
    {
        std::unique_ptr<Resource> res = std::make_unique<Resource>();
    } // Auto destroyed here
    return 0;
}
