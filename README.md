# HashMap Implementation

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Tests Passing](https://img.shields.io/badge/tests-53%2F53%20passing-brightgreen)](hashmap_tests.cpp)
![C++](https://img.shields.io/badge/C%2B%2B-20-blue)
[![Memory Safe](https://img.shields.io/badge/memory-safe-informational?logo=c%2B%2B)](hashmap.h)

A high-performance, generic hash table implementation in C++ with separate chaining for collision resolution.

## Overview

This project demonstrates a production-quality implementation of a hash table (hash map) data structure using C++ templates. The implementation features dynamic resizing, collision handling via chaining, and comprehensive support for iteration and comparison operations.

### Key Features

- **Generic Templates**: Works with any key/value types that support hashing and equality comparison
- **Separate Chaining**: Uses singly-linked lists to resolve hash collisions
- **Dynamic Resizing**: Automatically doubles capacity when load factor exceeds 1.5
- **Complete API**:
  - Core operations: `insert()`, `contains()`, `at()`, `erase()`, `clear()`
  - Utilities: `empty()`, `size()`
  - Copy semantics: Copy constructor and assignment operator
  - Iteration: `begin()` and `next()` protocol
  - Comparison: `operator==()` for equality testing
- **Memory Safety**: Compiled with AddressSanitizer and UndefinedBehaviorSanitizer to catch memory errors
- **Comprehensive Testing**: 53 test cases covering all operations and edge cases

## Architecture

### Data Structure

The hash table is implemented as an array of pointers to linked list nodes:

```
data -> [ chain0 ] -> [ chain1 ] -> ... -> [ chain9 ]
          node node    nullptr              node node node
```

Each `ChainNode` stores:

- `const KeyT key`: The key (immutable once inserted)
- `ValT value`: The associated value (mutable via `at()`)
- `ChainNode* next`: Pointer to the next node in the chain

### Core Operations

| Operation           | Time Complexity | Notes                                          |
| ------------------- | --------------- | ---------------------------------------------- |
| `insert(key, val)`  | O(L)            | L = chain length; O(1) average case            |
| `contains(key)`     | O(L)            | L = chain length; O(1) average case            |
| `at(key)`           | O(L)            | Returns mutable reference; throws if not found |
| `erase(key)`        | O(L)            | Throws if key not found                        |
| `clear()`           | O(N+B)          | N = entries, B = buckets                       |
| `size()`, `empty()` | O(1)            | Constant time                                  |

### Resizing Strategy

When `(size + 1) / capacity > 1.5`, the hash table:

1. Allocates a new bucket array with double the capacity
2. Re-hashes all existing entries into the new buckets
3. **Does not allocate new nodes** — reuses and re-chains existing nodes
4. Deallocates the old bucket array

## Building and Testing

### Prerequisites

- C++20 compatible compiler (clang++ or g++)
- GoogleTest and GoogleMock libraries
- Make build tool
- AddressSanitizer support (optional, but recommended)

### Build Commands

```bash
# Build the test suite
make hashmap_tests

# Run core tests only
make test_hashmap_core

# Run augmented feature tests (operator==, iterator)
make test_hashmap_aug

# Run all tests
make test_hashmap_all

# Build and run the demo
make hashmap_main
make run_main

# Clean build artifacts
make clean
```

## Usage Example

```cpp
#include "hashmap.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
  // Create a hash table with initial capacity of 10
  HashMap<string, int> stock;

  // Insert key-value pairs
  stock.insert("apples", 42);
  stock.insert("oranges", 37);
  stock.insert("bananas", 28);

  // Check if key exists
  if (stock.contains("apples")) {
    cout << "Apples in stock: " << stock.at("apples") << endl;
  }

  // Modify values
  stock.at("apples") = 45;

  // Iterate through all entries
  stock.begin();
  string product;
  int count;
  while (stock.next(product, count)) {
    cout << product << ": " << count << endl;
  }

  // Remove an entry
  int removed = stock.erase("bananas");

  // Clear all entries
  stock.clear();

  return 0;
}
```

## Test Coverage

The implementation includes **53 comprehensive tests** organized into two categories:

### Core Tests (42 tests)

- **Constructors**: Default and capacity-specified initialization
- **Size & Empty**: Tracking and querying element count
- **Insert**: Basic insertion, collision handling, overwrite prevention, and resizing
- **Contains & At**: Key lookup and value retrieval with mutation support
- **Erase**: Removing entries from various positions in chains
- **Clear**: Bulk deletion and memory cleanup
- **Destructors**: Memory leak detection with ASan
- **Copy Operations**: Copy constructor and assignment operator with deep copying

### Augmented Tests (11 tests)

- **Equality**: Comparing maps regardless of internal bucket arrangement
- **Iteration**: Traversing all entries with `begin()` and `next()`
- **Post-Resize Iteration**: Correct iteration after dynamic resizing

All tests use readable, domain-specific test data (car brands and models) and compile with memory sanitization flags.

## Implementation Highlights

### Collision Resolution

The `insert()` method uses separate chaining:

```cpp
void insert(KeyT key, ValT value) {
  if (contains(key)) return;  // No duplicates

  // Check load factor and resize if needed
  if ((sz + 1) / capacity > 1.5) {
    // Resize and rehash...
  }

  // Insert at front of chain
  size_t index = hash<KeyT>{}(key) % capacity;
  ChainNode* new_node = new ChainNode(key, value, data[index]);
  data[index] = new_node;
  sz++;
}
```

### Deep Copy Semantics

Both the copy constructor and assignment operator perform deep copies:

```cpp
HashMap(const HashMap& other) {
  capacity = other.capacity;
  sz = other.sz;
  data = new ChainNode*[capacity];

  // Copy every node
  for (size_t i = 0; i < capacity; ++i) {
    ChainNode* src = other.data[i];
    ChainNode** dst_ptr = &data[i];
    while (src != nullptr) {
      *dst_ptr = new ChainNode(src->key, src->value);
      dst_ptr = &(*dst_ptr)->next;
      src = src->next;
    }
  }
}
```

### Iterator Protocol

The `begin()` and `next()` methods provide a simple iteration interface:

```cpp
void begin() {
  curr = nullptr;
  curr_idx = 0;
}

bool next(KeyT& key, ValT& value) {
  // Advance to next non-empty bucket if needed
  while (curr == nullptr && curr_idx < capacity) {
    curr = data[curr_idx++];
  }

  if (curr == nullptr) return false;

  key = curr->key;
  value = curr->value;
  curr = curr->next;
  return true;
}
```

## Code Quality

- **Memory Safety**: Compiled with AddressSanitizer (`-fsanitize=address,undefined`)
- **Strict Warnings**: `-Wall -Wextra -Werror` for compile-time safety
- **Modern C++**: Uses C++20 features
- **Documentation**: Comprehensive file and function headers for portfolio readability
- **Formatting**: Consistent indentation and naming conventions

## Project Structure

```
.
├── hashmap.h              # Main implementation (442 lines)
├── hashmap_tests.cpp      # Comprehensive test suite (588 lines)
├── hashmap_main.cpp       # Demo and usage examples
├── Makefile               # Build configuration
├── README.md             # This file
└── suppr.txt             # AddressSanitizer suppressions
```

## Performance Notes

- **Average Case**: O(1) for insert, lookup, and erase with good hash distribution
- **Worst Case**: O(N) if all keys hash to the same bucket (rare with good hash functions)
- **Resizing**: O(N+B) amortized cost when load factor is exceeded
- **Memory**: O(N+B) where N is number of entries and B is bucket count

## Compilation Flags

```makefile
-Wall -Wextra -Werror          # Strict error checking
-std=c++2a                      # C++20 standard
-fsanitize=address,undefined    # Memory and UB detection
-g -fno-omit-frame-pointer      # Debug symbols with full stack traces
```

## Future Enhancements

- Open addressing as an alternative collision resolution strategy
- Customizable hash functions via template parameter
- Iterators with proper C++ iterator traits
- Move semantics for C++11 efficiency
- Heterogeneous lookup support

## License

This is a portfolio project demonstrating data structure implementation in modern C++.

---

**Implementation Date**: January 2026  
**Status**: Complete and tested  
**Test Results**: 53/53 tests passing ✓
