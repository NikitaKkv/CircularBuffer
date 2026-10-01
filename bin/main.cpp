#include <circular_buffer.h>
#include <iostream>
#include <algorithm>

int main() {
    std::cout << "=== Circular Buffer Demo ===" << std::endl;

    // Fixed-size circular buffer with capacity 5
    circular_buffer<int> cb(5);
    cb.push_back(10);
    cb.push_back(20);
    cb.push_back(30);

    std::cout << "Elements:";
    for (int x : cb) {
        std::cout << " " << x;
    }
    std::cout << "\n";

    // Push more elements (overwriting old ones in fixed mode)
    cb.push_back(40);
    cb.push_back(50);
    cb.push_back(60);

    std::cout << "After overwriting:";
    for (int x : cb) {
        std::cout << " " << x;
    }
    std::cout << "\n";

    // STL-compatibility: random access and algorithms
    std::cout << "Front: " << cb.front() << ", Back: " << cb.back() << "\n";
    std::sort(cb.begin(), cb.end());
    std::cout << "After std::sort:";
    for (int x : cb) {
        std::cout << " " << x;
    }
    std::cout << "\n";

    // Extendable circular buffer
    std::cout << "\n=== Extendable Circular Buffer Demo ===" << std::endl;
    circular_buffer<std::string, true> ext_cb(2);
    ext_cb.push_back("Hello");
    ext_cb.push_back("Modern");
    ext_cb.push_back("C++"); // Automatically expands capacity

    std::cout << "Size: " << ext_cb.size() << ", Capacity: " << ext_cb.capacity() << "\n";
    for (const auto& s : ext_cb) {
        std::cout << s << " ";
    }
    std::cout << "\n";

    return 0;
}