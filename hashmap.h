/**
 * HashMap: A Generic Hash Table with Chaining
 *
 * A template-based implementation of a hash table using separate chaining
 * for collision resolution. This data structure provides O(1) average-case
 * performance for insert, lookup, and erase operations.
 *
 * Features:
 *  - Generic key/value types with template parameters
 *  - Separate chaining collision resolution using singly-linked lists
 *  - Dynamic resizing when load factor exceeds 1.5
 *  - Complete iterator support via begin/next methods
 *  - Copy constructor and assignment operator for deep copying
 *  - Equality comparison between maps
 *
 * @author Portfolio Implementation
 * @date January 2026
 */

#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

template <typename KeyT, typename ValT>
class HashMap {
 private:
  struct ChainNode {
    const KeyT key;
    ValT value;
    ChainNode* next;

    ChainNode(KeyT key, ValT value) : key(key), value(value), next(nullptr) {
    }

    ChainNode(KeyT key, ValT value, ChainNode* next)
        : key(key), value(value), next(next) {
    }
  };

  ChainNode** data;
  size_t sz;
  size_t capacity;

  // Utility members for begin/next iteration
  ChainNode* curr;  // Current node in iteration
  size_t curr_idx;  // Current bucket index in iteration

 public:
  /**
   * Creates an empty `HashMap` with 10 buckets.
   */
  HashMap() {
    capacity = 10;
    sz = 0;
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
    }

    curr = nullptr;
    curr_idx = 0;
  }

  /**
   * Creates an empty `HashMap` with `capacity` buckets.
   */
  HashMap(size_t capacity) {
    this->capacity = capacity;

    this->sz = 0;

    this->data = new ChainNode*[this->capacity];

    for (size_t i = 0; i < this->capacity; i++) {
      this->data[i] = nullptr;
    }

    this->curr = nullptr;
    this->curr_idx = 0;
  }

  /**
   * Checks if the `HashMap` is empty. Runs in O(1).
   */
  bool empty() const {
    return sz == 0;
  }

  /**
   * Returns the number of mappings in the `HashMap`. Runs in O(1).
   */
  size_t size() const {
    return sz;
  }

  /**
   * Adds the mapping `{key -> value}` to the `HashMap`. If the key already
   * exists, does not update the mapping (like the C++ STL map).
   *
   * Uses the hash value of the key to determine the location in the
   * underlying hash table. Creates exactly one new node; resizes by doubling
   * when the load factor exceeds 1.5.
   *
   * On resize, doesn't create new nodes, but rearranges existing ones.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  void insert(KeyT key, ValT value) {
    // avoid duplicate keys; do not update existing mapping
    if (contains(key)) {
      return;
    }
    // need to resize?
    double load_factor =
        static_cast<double>(sz + 1) / static_cast<double>(capacity);
    if (load_factor > 1.5) {
      // double the capacity
      size_t new_capacity = capacity * 2;
      ChainNode** new_data = new ChainNode*[new_capacity];
      for (size_t i = 0; i < new_capacity; i++) {
        new_data[i] = nullptr;
      }

      // rehash everything
      for (size_t i = 0; i < capacity; i++) {
        ChainNode* curr = data[i];
        while (curr != nullptr) {
          ChainNode* next = curr->next;
          size_t new_index = hash<KeyT>{}(curr->key) % new_capacity;
          curr->next = new_data[new_index];
          new_data[new_index] = curr;
          curr = next;
        }
      }

      delete[] data;
      data = new_data;
      capacity = new_capacity;
    }

    // insert new node at front of chain
    size_t index = hash<KeyT>{}(key) % capacity;
    ChainNode* new_node = new ChainNode(key, value, data[index]);
    data[index] = new_node;
    sz++;
  }

  /**
   * Return a reference to the value stored for `key` in the map.
   *
   * If key is not present in the map, throw `out_of_range` exception.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT& at(const KeyT& key) const {
    size_t index = hash<KeyT>{}(key) % capacity;
    ChainNode* curr = data[index];
    while (curr != nullptr) {
      if (curr->key == key) {
        return curr->value;
      }
      curr = curr->next;
    }
    throw out_of_range("Key not found");
  }

  /**
   * Returns `true` if the key is present in the map, and false otherwise.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  bool contains(const KeyT& key) const {
    size_t index = hash<KeyT>{}(key) % capacity;
    ChainNode* node = data[index];
    while (node != nullptr) {
      if (node->key == key) {
        return true;
      }
      node = node->next;
    }
    return false;
  }

  /**
   * Empties the `HashMap`, freeing all nodes. The bucket array may be left
   * alone.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  void clear() {
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* curr = data[i];
      while (curr != nullptr) {
        ChainNode* next = curr->next;
        delete curr;
        curr = next;
      }
      data[i] = nullptr;
    }
    // reset size tracker too
    sz = 0;
  }

  /**
   * Destructor, cleans up the `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  ~HashMap() {
    clear();
    delete[] data;
  }

  /**
   * Removes the mapping for the given key from the `HashMap`, and returns the
   * value.
   *
   * Throws `out_of_range` if the key is not present in the map. Creates no new
   * nodes, and does not update the key or value of any existing nodes.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT erase(const KeyT& key) {
    size_t index = hash<KeyT>{}(key) % capacity;
    ChainNode* curr = data[index];
    ChainNode* prev = nullptr;

    while (curr != nullptr) {
      if (curr->key == key) {
        ValT val = curr->value;
        if (prev == nullptr) {
          // first node
          data[index] = curr->next;

        } else {
          prev->next = curr->next;
        }
        delete curr;
        sz--;
        return val;
      }
      prev = curr;
      curr = curr->next;
    }
    throw out_of_range("Key not found");
  }

  /**
   * Copy constructor.
   *
   * Copies the mappings from the provided `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings in `other`, and B is the
   * number of buckets.
   */
  HashMap(const HashMap& other) {
    capacity = other.capacity;
    sz = other.sz;

    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
    }

    // copy all the nodes
    for (size_t i = 0; i < other.capacity; i++) {
      ChainNode* other_curr = other.data[i];
      ChainNode** my_curr_ptr = &(data[i]);

      while (other_curr != nullptr) {
        *my_curr_ptr = new ChainNode(other_curr->key, other_curr->value);
        my_curr_ptr = &((*my_curr_ptr)->next);
        other_curr = other_curr->next;
      }
    }

    curr = nullptr;
    curr_idx = 0;
  }

  /**
   * Assignment operator; `operator=`.
   *
   * Clears this table, and copies the mappings from the provided `HashMap`.
   *
   * Runs in O((N1+B1) + (N2+B2)), where N1 and B1 are the number of mappings
   * and buckets in `this`, and N2 and B2 are the number of mappings and buckets
   * in `other`.
   */
  HashMap& operator=(const HashMap& other) {
    if (this == &other) {
      return *this;  // self-assign: nothing to do
    }
    // clear old stuff
    clear();
    delete[] data;

    // copy from other
    capacity = other.capacity;
    sz = other.sz;
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
    }

    // copy nodes
    for (size_t i = 0; i < other.capacity; i++) {
      ChainNode* other_curr = other.data[i];
      ChainNode** my_curr_ptr = &(data[i]);
      while (other_curr != nullptr) {
        *my_curr_ptr = new ChainNode(other_curr->key, other_curr->value);
        my_curr_ptr = &((*my_curr_ptr)->next);

        other_curr = other_curr->next;
      }
    }

    curr = nullptr;
    curr_idx = 0;

    return *this;
  }

  // =====================
  /**
   *
   * Checks if the contents of `this` and `other` are equivalent.
   *
   * Two `HashMap` objects are equivalent if they contain the same
   * mappings from key to value. To be equivalent, the two `HashMap`
   * objects need not have the elements saved in the same order
   * inside of the buckets.
   *
   * Runs in worst-case O(B*L) time, where B is the maximum number
   * of buckets in either of the `HashMap` objects and L is the length
   * of the  largest chain on any of the buckets.
   *
   */
  bool operator==(const HashMap& other) const {
    if (this->sz != other.sz) {
      return false;
    }

    // For each key in this, ensure other has same key/value
    for (size_t i = 0; i < this->capacity; ++i) {
      ChainNode* node = this->data[i];

      while (node != nullptr) {
        // if key missing or value mismatch => not equal
        if (!other.contains(node->key)) return false;
        if (other.at(node->key) != node->value) return false;
        node = node->next;
      }
    }
    return true;
  }

  /**
   * Resets internal state for an iterative traversal.
   *
   * See `next` for usage details. Modifies nothing except for `curr` and
   * `curr_idx`.
   *
   * Runs in worst-case O(B), where B is the number of buckets.
   */

  void begin() {
    // reset internal cursors; find nothing yet
    curr = nullptr;
    curr_idx = 0;
  }

  /**
   * Uses the internal state to return the "next" key and val
   * by reference, and advances the internal state. Returns `true` if the
   * reference parameters were set, and `false` other
   *
   * Example usage:
   *
   * ```c++
   * HashMap<string, int> hm;
   * hm.begin();
   * string key;
   * int value;
   * while (hm.next(key, val)) {
   *   cout << key << ": " << val << endl;
   * }
   * ```
   *
   *
   * Does not visit the mappings in any defined order.
   *
   * Modifies nothing except for `curr` and `curr_idx`.
   *
   * Runs in worst-case O(B) where B is the number of buckets.
   */

  bool next(KeyT& key, ValT& value) {
    // if we're between buckets or at start, advance to next non-empty bucket
    while (curr == nullptr && curr_idx < capacity) {
      curr = data[curr_idx];
      curr_idx += 1;  // move past this bucket for next time
    }

    if (curr == nullptr) {
      return false;  // no more elements
    }

    // yield the current node, then advance within the bucket
    key = curr->key;
    value = curr->value;
    curr = curr->next;

    return true;
  }

  // ===============================================

  /**
   * Returns a pointer to the underlying memory managed by the `HashMap`.
   * For autograder testing purposes only.
   */

  void* get_data() const {
    return data;
  }

  /**
   * Returns the capacity of the underlying memory managed by the `HashMap`. For
   * autograder testing purposes only.
   */

  size_t get_capacity() {
    return capacity;
  }
};
