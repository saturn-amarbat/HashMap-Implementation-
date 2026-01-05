/**
 * HashMap Demo and Testing Playground
 *
 * Simple demonstration of HashMap usage with basic operations.
 * Compile and run to experiment with the HashMap implementation.
 *
 * @file hashmap_main.cpp
 */

#include <iostream>
#include <string>

#include "hashmap.h"

using namespace std;

int main() {
  // Example usage: Simple key-value store
  HashMap<string, int> inventory(5);

  // Insert some mappings
  inventory.insert("apples", 42);
  inventory.insert("oranges", 37);
  inventory.insert("bananas", 28);

  // Check contents
  cout << "Inventory size: " << inventory.size() << endl;
  cout << "Contains 'apples': " << (inventory.contains("apples") ? "yes" : "no")
       << endl;
  cout << "Apples in stock: " << inventory.at("apples") << endl;

  // Iterate through all items
  cout << "\nAll items:" << endl;
  inventory.begin();
  string product;
  int count;
  while (inventory.next(product, count)) {
    cout << "  " << product << ": " << count << endl;
  }

  return 0;
}
