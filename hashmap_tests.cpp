/**
 * HashMap Unit Tests
 *
 * Comprehensive test suite for the HashMap template class, covering:
 *  - Core operations: constructor, empty, size, insert, contains, at, clear
 *  - Destruction and cleanup (memory safety verified with AddressSanitizer)
 *  - Collision handling and chain management
 *  - Dynamic resizing behavior
 *  - Copy semantics: copy constructor and assignment operator
 *  - Advanced features: equality comparison and iterator (begin/next) protocol
 *
 * Tests use car brand/model data for readable, intuitive test cases.
 * All tests are compiled with AddressSanitizer and UndefinedBehaviorSanitizer
 * to ensure memory safety and correctness.
 *
 * @file hashmap_tests.cpp
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>
#include <vector>

#include "hashmap.h"

using namespace std;
using namespace testing;

namespace {

class Random {
 private:
  static mt19937 rng;

 public:
  static void seed(int s) {
    Random::rng.seed(s);
  }

  /**
   * Generate a random integer in the range 0 (inclusive) to `max` (exclusive)
   */
  static int randInt(int max) {
    // Even though mt19937 is standardized, STL distributions aren't!
    // So, unfortunately, even though this is biased, we're forced to
    // do something like this. Technically uint32_fast_t isn't...
    // totally consistent across platforms? But within reason it works.
    return rng() % (max + 1);
  }
};

std::mt19937 Random::rng;

// ============== HashMapCore Tests ==============
// tasks start below, i thought of maps as car garage and sstoring various cars
// in it since it was good analogy for me to understand

// Constructor Tests
TEST(HashMapCore, DefaultConstructorCreatesEmptyMap) {
  HashMap<string, string> garage;
  EXPECT_TRUE(garage.empty());
  EXPECT_EQ(garage.size(), 0);
}

TEST(HashMapCore, CapacityConstructorCreatesEmptyMap) {
  HashMap<string, string> dealership(20);
  EXPECT_TRUE(dealership.empty());
  EXPECT_EQ(dealership.size(), 0);
}

// Empty and Size Tests
TEST(HashMapCore, EmptyReturnsTrueForEmptyMap) {
  HashMap<string, string> cars;
  EXPECT_TRUE(cars.empty());
}

TEST(HashMapCore, EmptyReturnsFalseAfterInsert) {
  HashMap<string, string> cars;
  cars.insert("Toyota", "Camry");
  EXPECT_FALSE(cars.empty());
}

TEST(HashMapCore, SizeIncreasesWithInserts) {
  HashMap<string, string> inventory;
  EXPECT_EQ(inventory.size(), 0);
  inventory.insert("Honda", "Civic");
  EXPECT_EQ(inventory.size(), 1);
  inventory.insert("Ford", "Mustang");
  EXPECT_EQ(inventory.size(), 2);
  inventory.insert("Tesla", "Model3");
  EXPECT_EQ(inventory.size(), 3);
}

// Insert Tests
TEST(HashMapCore, InsertAddsMapping) {
  HashMap<string, string> cars;
  cars.insert("BMW", "M3");
  EXPECT_TRUE(cars.contains("BMW"));

  EXPECT_EQ(cars.at("BMW"), "M3");
}

TEST(HashMapCore, InsertDoesNotOverwriteExistingKey) {
  HashMap<string, string> cars;
  cars.insert("Mazda", "MX5");
  cars.insert("Mazda", "CX5");  // should NOT overwrite
  EXPECT_EQ(cars.at("Mazda"), "MX5");
  EXPECT_EQ(cars.size(), 1);
}

TEST(HashMapCore, InsertMultipleValues) {
  HashMap<string, string> showroom;

  showroom.insert("Audi", "A4");
  showroom.insert("Mercedes", "C300");

  showroom.insert("Lexus", "IS300");
  EXPECT_EQ(showroom.size(), 3);

  EXPECT_TRUE(showroom.contains("Audi"));
  EXPECT_TRUE(showroom.contains("Mercedes"));
  EXPECT_TRUE(showroom.contains("Lexus"));
}

TEST(HashMapCore, InsertHandlesCollisions) {
  // Using integer keys where hash(i) = i to control bucket placement
  HashMap<int, string> cars(10);
  cars.insert(5, "Porsche911");
  cars.insert(15, "FerrariF40");   // collides with 5 (both % 10 == 5)
  cars.insert(25, "Lamborghini");  // also collides
  EXPECT_EQ(cars.size(), 3);

  EXPECT_EQ(cars.at(5), "Porsche911");
  EXPECT_EQ(cars.at(15), "FerrariF40");
  EXPECT_EQ(cars.at(25), "Lamborghini");
}

TEST(HashMapCore, InsertTriggersResize) {
  HashMap<string, string> cars(2);
  cars.insert("Subaru", "WRX");
  cars.insert("Nissan", "GTR");
  cars.insert("Mitsubishi", "Evo");    // load factor = 3/2 = 1.5
  cars.insert("Dodge", "Challenger");  // Should trigger resize (> 1.5)
  EXPECT_EQ(cars.size(), 4);
  EXPECT_TRUE(cars.contains("Subaru"));
  EXPECT_TRUE(cars.contains("Nissan"));
  EXPECT_TRUE(cars.contains("Mitsubishi"));

  EXPECT_TRUE(cars.contains("Dodge"));
}

TEST(HashMapCore, InsertAfterResizeMaintainsAllValues) {
  HashMap<string, string> fleet(2);
  fleet.insert("Toyota", "Corolla");
  fleet.insert("Honda", "Accord");
  fleet.insert("Mazda", "3");
  fleet.insert("Subaru", "Impreza");
  fleet.insert("Ford", "Focus");
  fleet.insert("Chevy", "Cruze");
  fleet.insert("Nissan", "Sentra");
  fleet.insert("Hyundai", "Elantra");
  EXPECT_EQ(fleet.size(), 8);

  EXPECT_TRUE(fleet.contains("Toyota"));
  EXPECT_EQ(fleet.at("Mazda"), "3");
  EXPECT_TRUE(fleet.contains("Hyundai"));
}

// Contains Tests
TEST(HashMapCore, ContainsReturnsFalseForEmptyMap) {
  HashMap<string, string> cars;
  EXPECT_FALSE(cars.contains("BMW"));
}

TEST(HashMapCore, ContainsReturnsTrueForPresentKey) {
  HashMap<string, string> cars;

  cars.insert("Acura", "NSX");
  EXPECT_TRUE(cars.contains("Acura"));
}

TEST(HashMapCore, ContainsReturnsFalseForAbsentKey) {
  HashMap<string, string> cars;
  cars.insert("Volkswagen", "Golf");
  EXPECT_FALSE(cars.contains("Audi"));
}

TEST(HashMapCore, ContainsWorksWithCollisions) {
  HashMap<int, string> cars(10);
  cars.insert(3, "McLaren");
  cars.insert(13, "Bugatti");  // same bucket
  EXPECT_TRUE(cars.contains(3));
  EXPECT_TRUE(cars.contains(13));
  EXPECT_FALSE(cars.contains(23));
}

// At Tests
TEST(HashMapCore, AtReturnsCorrectValue) {
  HashMap<string, string> cars;
  cars.insert("Genesis", "G70");
  EXPECT_EQ(cars.at("Genesis"), "G70");
}

TEST(HashMapCore, AtThrowsForAbsentKey) {
  HashMap<string, string> cars;
  EXPECT_THROW(cars.at("Ferrari"), out_of_range);
}

TEST(HashMapCore, AtReturnsCorrectValueWithCollisions) {
  HashMap<int, string> cars(10);
  cars.insert(7, "AstonMartin");
  cars.insert(17, "Bentley");
  cars.insert(27, "RollsRoyce");
  EXPECT_EQ(cars.at(7), "AstonMartin");
  EXPECT_EQ(cars.at(17), "Bentley");
  EXPECT_EQ(cars.at(27), "RollsRoyce");
}

TEST(HashMapCore, AtAllowsModification) {
  HashMap<string, string> cars;
  cars.insert("Jeep", "Wrangler");
  cars.at("Jeep") = "Cherokee";
  EXPECT_EQ(cars.at("Jeep"), "Cherokee");
}

TEST(HashMapCore, AtThrowsForKeyInSameBucket) {
  HashMap<int, string> cars(10);
  cars.insert(4, "Corvette");
  cars.insert(14, "Viper");
  EXPECT_THROW(cars.at(24), out_of_range);  // 24 not present but same bucket
}

// Clear Tests
TEST(HashMapCore, ClearEmptiesMap) {
  HashMap<string, string> cars;
  cars.insert("Kia", "Stinger");
  cars.insert("Lincoln", "Continental");
  cars.clear();
  EXPECT_TRUE(cars.empty());
  EXPECT_EQ(cars.size(), 0);
}

TEST(HashMapCore, ClearRemovesAllMappings) {
  HashMap<string, string> cars;
  cars.insert("Cadillac", "CTS");
  cars.insert("Buick", "Regal");
  cars.clear();
  EXPECT_FALSE(cars.contains("Cadillac"));
  EXPECT_FALSE(cars.contains("Buick"));
}

TEST(HashMapCore, ClearOnEmptyMap) {
  HashMap<string, string> cars;
  cars.clear();
  EXPECT_TRUE(cars.empty());
  EXPECT_EQ(cars.size(), 0);
}

TEST(HashMapCore, InsertAfterClear) {
  HashMap<string, string> cars;
  cars.insert("Alfa Romeo", "Giulia");
  cars.clear();
  cars.insert("Maserati", "Ghibli");
  EXPECT_EQ(cars.size(), 1);
  EXPECT_TRUE(cars.contains("Maserati"));
  EXPECT_FALSE(cars.contains("Alfa Romeo"));
}

// Destructor Tests (memory leak tests with AddressSanitizer)
TEST(HashMapCore, DestructorCleansUpMemory) {
  HashMap<string, string>* cars = new HashMap<string, string>();
  cars->insert("Volvo", "S60");
  cars->insert("Saab", "93");
  cars->insert("Polestar", "2");
  delete cars;  // Should not leak
}

TEST(HashMapCore, DestructorCleansUpEmptyMap) {
  HashMap<string, string>* cars = new HashMap<string, string>();
  delete cars;  // Should not leak
}

TEST(HashMapCore, DestructorCleansUpAfterResize) {
  HashMap<string, string>* cars = new HashMap<string, string>(2);
  cars->insert("Mini", "Cooper");
  cars->insert("Fiat", "500");
  cars->insert("Smart", "ForTwo");
  cars->insert("Scion", "tC");
  cars->insert("Saturn", "Ion");
  delete cars;  // Should not leak
}

// Erase Tests
TEST(HashMapCore, EraseRemovesMapping) {
  HashMap<string, string> cars;
  cars.insert("Pontiac", "GTO");
  string val = cars.erase("Pontiac");
  EXPECT_EQ(val, "GTO");
  EXPECT_FALSE(cars.contains("Pontiac"));
  EXPECT_EQ(cars.size(), 0);
}

TEST(HashMapCore, EraseThrowsForAbsentKey) {
  HashMap<string, string> cars;
  EXPECT_THROW(cars.erase("DeLorean"), out_of_range);
}

TEST(HashMapCore, EraseFromChain) {
  HashMap<int, string> cars(10);
  cars.insert(8, "Pagani");
  cars.insert(18, "Koenigsegg");
  cars.insert(28, "Zenvo");
  string val = cars.erase(18);
  EXPECT_EQ(val, "Koenigsegg");
  EXPECT_FALSE(cars.contains(18));
  EXPECT_TRUE(cars.contains(8));
  EXPECT_TRUE(cars.contains(28));
  EXPECT_EQ(cars.size(), 2);
}

TEST(HashMapCore, EraseFirstInChain) {
  HashMap<int, string> cars(10);
  cars.insert(2, "Lotus");
  cars.insert(12, "Caterham");
  cars.erase(2);
  EXPECT_FALSE(cars.contains(2));
  EXPECT_TRUE(cars.contains(12));
}

TEST(HashMapCore, EraseLastInChain) {
  HashMap<int, string> cars(10);
  cars.insert(6, "Jaguar");
  cars.insert(16, "LandRover");
  cars.erase(16);
  EXPECT_TRUE(cars.contains(6));
  EXPECT_FALSE(cars.contains(16));
}

TEST(HashMapCore, EraseThrowsForKeyInSameBucket) {
  HashMap<int, string> cars(10);
  cars.insert(9, "Morgan");
  cars.insert(19, "TVR");
  EXPECT_THROW(cars.erase(29), out_of_range);
}

// Copy Constructor Tests
TEST(HashMapCore, CopyConstructorCopiesEmptyMap) {
  HashMap<string, string> garage1;
  HashMap<string, string> garage2(garage1);
  EXPECT_TRUE(garage2.empty());
  EXPECT_EQ(garage2.size(), 0);
}

TEST(HashMapCore, CopyConstructorCopiesMappings) {
  HashMap<string, string> original;
  original.insert("Infiniti", "Q50");
  original.insert("Acura", "TLX");
  original.insert("Lexus", "GS350");
  HashMap<string, string> copy(original);
  EXPECT_EQ(copy.size(), 3);
  EXPECT_EQ(copy.at("Infiniti"), "Q50");
  EXPECT_EQ(copy.at("Acura"), "TLX");
  EXPECT_EQ(copy.at("Lexus"), "GS350");
}

TEST(HashMapCore, CopyConstructorCreatesIndependentCopy) {
  HashMap<string, string> garage1;
  garage1.insert("Chrysler", "300");
  HashMap<string, string> garage2(garage1);
  garage1.insert("Dodge", "Charger");
  garage2.insert("RAM", "1500");
  EXPECT_EQ(garage1.size(), 2);
  EXPECT_EQ(garage2.size(), 2);
  EXPECT_FALSE(garage1.contains("RAM"));
  EXPECT_FALSE(garage2.contains("Dodge"));
}

TEST(HashMapCore, CopyConstructorCopiesAfterResize) {
  HashMap<string, string> lot1(2);
  lot1.insert("GMC", "Sierra");
  lot1.insert("Chevrolet", "Silverado");
  lot1.insert("Ford", "F150");
  lot1.insert("Toyota", "Tundra");
  lot1.insert("Nissan", "Titan");
  HashMap<string, string> lot2(lot1);
  EXPECT_EQ(lot2.size(), 5);
  EXPECT_EQ(lot2.at("GMC"), "Sierra");
  EXPECT_TRUE(lot2.contains("Ford"));
}

// Assignment Operator Tests
TEST(HashMapCore, AssignmentOperatorCopiesEmptyMap) {
  HashMap<string, string> showroom1;
  HashMap<string, string> showroom2;
  showroom2.insert("Porsche", "Cayman");
  showroom2 = showroom1;
  EXPECT_TRUE(showroom2.empty());
  EXPECT_EQ(showroom2.size(), 0);
}

TEST(HashMapCore, AssignmentOperatorCopiesMappings) {
  HashMap<string, string> dealer1;
  dealer1.insert("Audi", "A6");
  dealer1.insert("BMW", "5Series");
  HashMap<string, string> dealer2;
  dealer2 = dealer1;
  EXPECT_EQ(dealer2.size(), 2);
  EXPECT_EQ(dealer2.at("Audi"), "A6");
  EXPECT_EQ(dealer2.at("BMW"), "5Series");
}

TEST(HashMapCore, AssignmentOperatorClearsExistingData) {
  HashMap<string, string> lot1;
  lot1.insert("Tesla", "ModelS");
  HashMap<string, string> lot2;
  lot2.insert("Rivian", "R1T");
  lot2.insert("Lucid", "Air");
  lot2 = lot1;
  EXPECT_EQ(lot2.size(), 1);
  EXPECT_TRUE(lot2.contains("Tesla"));
  EXPECT_FALSE(lot2.contains("Rivian"));
}

TEST(HashMapCore, AssignmentOperatorHandlesSelfAssignment) {
  HashMap<string, string> garage;
  garage.insert("Ferrari", "488");
  garage.insert("Lamborghini", "Huracan");
  HashMap<string, string>& ref = garage;
  garage = ref;  // self-assignment through reference
  EXPECT_EQ(garage.size(), 2);
  EXPECT_EQ(garage.at("Ferrari"), "488");
  EXPECT_EQ(garage.at("Lamborghini"), "Huracan");
}

TEST(HashMapCore, AssignmentOperatorCreatesIndependentCopy) {
  HashMap<string, string> collection1;
  collection1.insert("McLaren", "720S");
  HashMap<string, string> collection2;
  collection2 = collection1;
  collection1.insert("AstonMartin", "DB11");

  EXPECT_EQ(collection1.size(), 2);
  EXPECT_EQ(collection2.size(), 1);
  EXPECT_FALSE(collection2.contains("AstonMartin"));
}

// ============== HashMapAugmented Tests ==============
// these might need work

// operator== Tests
TEST(HashMapAugmented, EqualityTrueForEmptyMaps) {
  HashMap<string, string> lot1;
  HashMap<string, string> lot2;
  EXPECT_TRUE(lot1 == lot2);
}

TEST(HashMapAugmented, EqualityTrueForSameMappings) {
  HashMap<string, string> garage1;
  garage1.insert("Honda", "Civic");
  garage1.insert("Toyota", "Corolla");
  HashMap<string, string> garage2;
  garage2.insert("Toyota", "Corolla");
  garage2.insert("Honda", "Civic");  // different order, same content
  EXPECT_TRUE(garage1 == garage2);
}

TEST(HashMapAugmented, EqualityFalseForDifferentSizes) {
  HashMap<string, string> showroom1;
  showroom1.insert("Mazda", "Miata");
  HashMap<string, string> showroom2;
  showroom2.insert("Mazda", "Miata");
  showroom2.insert("Nissan", "370Z");
  EXPECT_FALSE(showroom1 == showroom2);
}

TEST(HashMapAugmented, EqualityFalseForDifferentKeys) {
  HashMap<string, string> dealer1;
  dealer1.insert("BMW", "M4");
  HashMap<string, string> dealer2;
  dealer2.insert("Audi", "M4");  // same value, different key
  EXPECT_FALSE(dealer1 == dealer2);
}

TEST(HashMapAugmented, EqualityFalseForDifferentValues) {
  HashMap<string, string> inventory1;
  inventory1.insert("Ford", "Mustang");
  HashMap<string, string> inventory2;
  inventory2.insert("Ford", "Fiesta");  // same key, different value
  EXPECT_FALSE(inventory1 == inventory2);
}

TEST(HashMapAugmented, EqualityTrueAfterResize) {
  HashMap<string, string> fleet1(2);
  HashMap<string, string> fleet2(10);
  // add enough to trigger resize in fleet1
  fleet1.insert("Subaru", "Outback");
  fleet1.insert("Volkswagen", "Jetta");
  fleet1.insert("Kia", "Forte");
  fleet1.insert("Hyundai", "Accent");
  fleet2.insert("Subaru", "Outback");
  fleet2.insert("Volkswagen", "Jetta");
  fleet2.insert("Kia", "Forte");
  fleet2.insert("Hyundai", "Accent");
  EXPECT_TRUE(fleet1 == fleet2);
}

// begin/next Tests
TEST(HashMapAugmented, BeginNextOnEmptyMap) {
  HashMap<string, string> cars;
  cars.begin();
  string key, value;
  EXPECT_FALSE(cars.next(key, value));
}

TEST(HashMapAugmented, BeginNextIteratesAllElements) {
  HashMap<string, string> collection;
  collection.insert("Porsche", "911");

  collection.insert("Ferrari", "F8");
  collection.insert("Lamborghini", "Aventador");

  vector<pair<string, string>> data;
  collection.begin();
  string key, value;
  while (collection.next(key, value)) {
    data.push_back({key, value});
  }

  EXPECT_EQ(data.size(), 3);
  vector<pair<string, string>> expected = {
      {"Porsche", "911"}, {"Ferrari", "F8"}, {"Lamborghini", "Aventador"}};
  EXPECT_THAT(data, UnorderedElementsAreArray(expected));
}

TEST(HashMapAugmented, BeginNextHandlesCollisions) {
  // using ints where hash(i) = i to force collisions
  HashMap<int, string> hm(10);
  hm.insert(3, "McLaren");

  hm.insert(13, "Pagani");  // same bucket

  hm.insert(23, "Koenigsegg");  // same bucket

  vector<pair<int, string>> data;
  hm.begin();
  int key;
  string value;
  while (hm.next(key, value)) {
    data.push_back({key, value});
  }

  EXPECT_EQ(data.size(), 3);
  vector<pair<int, string>> expected = {
      {3, "McLaren"}, {13, "Pagani"}, {23, "Koenigsegg"}};
  EXPECT_THAT(data, UnorderedElementsAreArray(expected));
}

TEST(HashMapAugmented, BeginNextAfterResize) {
  HashMap<string, string> hm(2);

  hm.insert("Chevrolet", "Malibu");

  hm.insert("Buick", "Encore");
  hm.insert("Cadillac", "Escalade");
  hm.insert("GMC", "Yukon");
  hm.insert("Saturn", "Vue");
  hm.insert("Pontiac", "Firebird");

  vector<pair<string, string>> data;

  hm.begin();
  string key, value;
  while (hm.next(key, value)) {
    data.push_back({key, value});
  }

  EXPECT_EQ(data.size(), 6);
}

TEST(HashMapAugmented, BeginNextCanBeCalledMultipleTimes) {
  HashMap<string, string> garage;

  garage.insert("Tesla", "Model3");

  garage.insert("Rivian", "R1S");

  garage.begin();
  string key, value;
  int count1 = 0;
  while (garage.next(key, value)) {
    count1++;
  }

  garage.begin();  // reset iteration

  int count2 = 0;
  while (garage.next(key, value)) {
    count2++;
  }

  EXPECT_EQ(count1, 2);
  EXPECT_EQ(count2, 2);
}

}  // namespace
