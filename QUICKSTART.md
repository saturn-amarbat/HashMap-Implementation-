# Quick Start Guide

## Building the Project

```bash
# Build and run all tests
make test_hashmap_all

# Build and run just core tests (constructor, insert, erase, etc.)
make test_hashmap_core

# Build and run augmented tests (equality, iteration)
make test_hashmap_aug

# Build the demo program
make hashmap_main

# Run the demo
./hashmap_main

# Clean up build artifacts
make clean
```

## Using the HashMap

### Basic Usage

```cpp
#include "hashmap.h"
#include <string>

int main() {
  // Create a map: HashMap<KeyType, ValueType>
  HashMap<std::string, int> map;

  // Insert key-value pairs
  map.insert("name", 25);
  map.insert("age", 30);

  // Check if key exists
  if (map.contains("name")) {
    std::cout << "Found!" << std::endl;
  }

  // Access value (throws if not found)
  int value = map.at("age");

  // Modify value
  map.at("age") = 31;

  // Remove a key
  map.erase("name");

  // Check size
  std::cout << "Size: " << map.size() << std::endl;

  return 0;
}
```

### Iteration

```cpp
HashMap<std::string, int> map;
map.insert("a", 1);
map.insert("b", 2);

// Iterate through all entries
map.begin();
std::string key;
int value;
while (map.next(key, value)) {
  std::cout << key << " -> " << value << std::endl;
}
```

### Copying

```cpp
HashMap<int, std::string> original;
original.insert(1, "one");

// Copy constructor
HashMap<int, std::string> copy1(original);

// Assignment operator
HashMap<int, std::string> copy2;
copy2 = original;

// Deep copies: modifications don't affect originals
copy1.insert(2, "two");
// original still has only 1 entry
```

### Equality

```cpp
HashMap<std::string, int> map1, map2;
map1.insert("x", 10);
map2.insert("x", 10);

if (map1 == map2) {
  std::cout << "Maps are equal!" << std::endl;
}
```

## Key Features

| Operation    | Complexity   | Notes                              |
| ------------ | ------------ | ---------------------------------- |
| `insert()`   | O(1) average | O(n) worst case if all collide     |
| `contains()` | O(1) average | Searches chain length              |
| `at()`       | O(1) average | Returns mutable reference          |
| `erase()`    | O(1) average | Throws `out_of_range` if not found |
| `clear()`    | O(n)         | Frees all nodes                    |
| `size()`     | O(1)         | Constant time                      |

## Testing

The test suite includes:

- **Constructor Tests**: Default and custom capacity initialization
- **Basic Operations**: Insert, contains, at, erase, clear
- **Collision Handling**: Multiple keys in same bucket
- **Resizing**: Automatic doubling when load factor > 1.5
- **Memory Safety**: No leaks or buffer overflows (verified with ASan)
- **Copy Semantics**: Deep copying for constructor and assignment
- **Iteration**: begin/next protocol
- **Equality**: Comparing maps regardless of internal structure

Run `make test_hashmap_all` to execute all 53 tests.

## Design Highlights

### Separate Chaining

- Colliding keys are stored in linked lists within each bucket
- Fast insertion at front of chain: O(1)
- Linear search within chain: O(k) where k is chain length

### Dynamic Resizing

- Doubles capacity when `(size + 1) / capacity > 1.5`
- Re-hashes all entries without allocating new nodes
- Maintains O(1) average performance

### Deep Copy Semantics

- Copy constructor and assignment operator create independent copies
- Safe to modify copies without affecting originals
- Proper cleanup via destructor (verified with AddressSanitizer)

### Iterator Protocol

- `begin()` resets internal state
- `next()` returns next key-value pair and advances state
- Handles empty maps, collisions, and resized maps

## Architecture

```
HashMap<KeyT, ValT>
  ├─ ChainNode[] data (array of bucket heads)
  ├─ size_t sz (number of entries)
  ├─ size_t capacity (number of buckets)
  ├─ Iteration state
  │  ├─ curr (current node)
  │  └─ curr_idx (current bucket)
  └─ Methods
     ├─ Core: insert, contains, at, erase, clear
     ├─ Utils: empty, size
     ├─ Semantics: copy constructor, operator=
     ├─ Augmented: operator==, begin, next
     └─ Debug: get_data, get_capacity
```

## Compiler Flags

This project compiles with:

- `-Wall -Wextra -Werror` for strict error checking
- `-std=c++2a` for C++20 features
- `-fsanitize=address,undefined` for memory safety

No warnings or errors in the build.

## Questions?

Refer to:

- [README.md](README.md) - Full documentation
- [hashmap.h](hashmap.h) - Detailed API with complexity analysis
- [hashmap_tests.cpp](hashmap_tests.cpp) - Usage examples via test cases
- [hashmap_main.cpp](hashmap_main.cpp) - Simple demo program
