# CircularBuffer

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![CMake](https://img.shields.io/badge/build-CMake-brightgreen.svg)](https://cmake.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A high-performance, **STL-compliant Circular Buffer (Ring Buffer)** container template implemented in modern C++ (C++23). It supports custom allocators, random-access iterators, strong exception safety guarantees, and both fixed-capacity and auto-extending modes.

---

## Features

- **STL Named Requirements Compliance**:
  - [Container](https://en.cppreference.com/w/cpp/named_req/Container)
  - [SequenceContainer](https://en.cppreference.com/w/cpp/named_req/SequenceContainer) (`push_back`, `pop_back`, `push_front`, `pop_front`, `insert`, `erase`, `clear`)
  - [ReversibleContainer](https://en.cppreference.com/w/cpp/named_req/ReversibleContainer) (`rbegin()`, `rend()`, `crbegin()`, `crend()`)
  - [AllocatorAwareContainer](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer) (supports custom allocators via `std::allocator_traits`)
  - [RandomAccessIterator](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator) with $O(1)$ indexing, pointer arithmetic, and compatibility with STL algorithms (`std::sort`, `std::find`, `std::binary_search`).
- **Dual Operating Modes**:
  - **Fixed Capacity** (`Extendable = false`, default): Overwrites the oldest elements once the buffer is full.
  - **Dynamic Growth** (`Extendable = true`): Automatically doubles capacity when full (vector-like reallocation).
- **Strong Exception Safety**: Rollback semantics for insertions and range operations; guarantees no memory or resource leaks if element copy/move constructors throw.
- **Thorough Test Suite**: Comprehensive GoogleTest coverage with 60 test suites testing allocators, iterators, exceptions, and boundary conditions.

---

## Performance & Complexity

| Operation | Time Complexity |
|-----------|-----------------|
| `push_back(const T&)` / `push_front(const T&)` | $\mathcal{O}(1)$ amortized |
| `pop_back()` / `pop_front()` | $\mathcal{O}(1)$ |
| `operator[](size_type)` / `at(size_type)` | $\mathcal{O}(1)$ |
| `front()` / `back()` | $\mathcal{O}(1)$ |
| `insert(iterator, const T&)` | $\mathcal{O}(N)$ |
| `erase(iterator)` | $\mathcal{O}(N)$ |
| `clear()` | $\mathcal{O}(N)$ |

---

## Code Example

```cpp
#include <circular_buffer.h>
#include <iostream>
#include <algorithm>

int main() {
    // 1. Fixed-capacity circular buffer
    circular_buffer<int> cb(3);
    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);
    cb.push_back(4); // Overwrites oldest element (1)

    // Supports range-based for loops
    for (int x : cb) {
        std::cout << x << " "; // Output: 4 2 3
    }

    // Compatible with std algorithms
    std::sort(cb.begin(), cb.end());

    // 2. Extendable circular buffer
    circular_buffer<std::string, true> ext_buf(2);
    ext_buf.push_back("Data");
    ext_buf.push_back("Structures");
    ext_buf.push_back("C++"); // Automatically expands capacity to 4
}
```

---

## Build Instructions

### Prerequisites
- C++23 capable compiler (`g++ >= 13` or `clang++ >= 17`)
- CMake `>= 3.12`

### Building & Running Demo

```bash
# Clone repository
git clone https://github.com/NikitaKkv/CircularBuffer.git
cd CircularBuffer

# Configure and compile
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Run demo application
./build/bin/circular_buffer
```

### Running Test Suite

```bash
ctest --test-dir build --output-on-failure
```

---

## License

This project is licensed under the MIT License.
