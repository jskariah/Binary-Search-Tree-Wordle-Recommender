#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>
#include <set>
#include <string>
#include <vector>

#include "bstset.h"

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

// Helper functions
string set_to_string(const set<int>& s) {
  ostringstream ss;
  ss << "{";
  int i = 0;
  for (int value : s) {
    if (i != 0) {
      ss << ", ";
    }
    ss << value;
    i++;
  }
  ss << "}";
  return ss.str();
}

template <typename T>
void shuffle_vec(vector<T>& vec) {
  for (int i = vec.size() - 1; i >= 1; i--) {
    size_t j = Random::randInt(i - 1);
    swap(vec.at(i), vec.at(j));
  }
}

std::mt19937 Random::rng;

TEST(BSTSetCore, DefaultConstructorEmpty) {
  BSTSet<int> bst;
  EXPECT_TRUE(bst.empty());
  EXPECT_THAT(bst.size(), Eq(0u));
}

TEST(BSTSetCore, DefaultConstructorToString) {
  BSTSet<int> bst;
  EXPECT_THAT(bst.to_string(), StrEq("{}"));
}

// Tests for insert, contains and size

TEST(BSTSetCore, InsertSingleElement) {
  BSTSet<int> bst;
  bst.insert(5);
  EXPECT_FALSE(bst.empty());
  EXPECT_THAT(bst.size(), Eq(1u));
  EXPECT_TRUE(bst.contains(5));
  EXPECT_FALSE(bst.contains(3));
}

TEST(BSTSetCore, InsertMultipleElements) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(3);
  bst.insert(7);
  EXPECT_THAT(bst.size(), Eq(3u));
  EXPECT_TRUE(bst.contains(3));
  EXPECT_TRUE(bst.contains(5));
  EXPECT_TRUE(bst.contains(7));
  EXPECT_FALSE(bst.contains(1));
  EXPECT_FALSE(bst.contains(4));
  EXPECT_FALSE(bst.contains(6));
}

TEST(BSTSetCore, InsertDuplicate) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(5);
  EXPECT_THAT(bst.size(), Eq(1u));
  EXPECT_TRUE(bst.contains(5));
}

TEST(BSTSetCore, InsertManyDuplicates) {
  BSTSet<int> bst;
  for (int i = 0; i < 50; i++) {
    bst.insert(10);
  }
  EXPECT_THAT(bst.size(), Eq(1u));
}

TEST(BSTSetCore, InsertIncreasingOrder) {
  BSTSet<int> bst;
  for (int i = 0; i < 20; i++) {
    bst.insert(i);
  }
  EXPECT_THAT(bst.size(), Eq(20u));
  for (int i = 0; i < 20; i++) {
    EXPECT_TRUE(bst.contains(i));
  }
  EXPECT_FALSE(bst.contains(20));
  EXPECT_FALSE(bst.contains(-1));
}

TEST(BSTSetCore, InsertDecreasingOrder) {
  BSTSet<int> bst;
  for (int i = 19; i >= 0; i--) {
    bst.insert(i);
  }
  EXPECT_THAT(bst.size(), Eq(20u));
  for (int i = 0; i < 20; i++) {
    EXPECT_TRUE(bst.contains(i));
  }
}

TEST(BSTSetCore, ContainsOnEmpty) {
  BSTSet<int> bst;
  EXPECT_FALSE(bst.contains(0));
  EXPECT_FALSE(bst.contains(42));
}

// Tests for toString

TEST(BSTSetCore, ToStringBasic) {
  BSTSet<int> bst;
  set<int> ref;
  vector<int> values{3, 1, 4, 2};
  for (int v : values) {
    bst.insert(v);
    ref.insert(v);
  }
  EXPECT_THAT(bst.to_string(), StrEq(set_to_string(ref)));
}

TEST(BSTSetCore, ToStringSingleElement) {
  BSTSet<int> bst;
  bst.insert(42);
  set<int> ref{42};
  EXPECT_THAT(bst.to_string(), StrEq(set_to_string(ref)));
}

TEST(BSTSetCore, ToStringEmpty) {
  BSTSet<int> bst;
  EXPECT_THAT(bst.to_string(), StrEq("{}"));
}

TEST(BSTSetCore, ToStringWithStrings) {
  BSTSet<string> bst;
  bst.insert("Gwen");
  bst.insert("Jen");
  bst.insert("Ben");
  bst.insert("Sven");
  EXPECT_THAT(bst.to_string(), StrEq("{Ben, Gwen, Jen, Sven}"));
}

TEST(BSTSetCore, ToStringRandom) {
  Random::seed(42);
  BSTSet<int> bst;
  set<int> ref;
  for (int i = 0; i < 100; i++) {
    int key = Random::randInt(200);
    bst.insert(key);
    ref.insert(key);
  }
  EXPECT_THAT(bst.to_string(), StrEq(set_to_string(ref)));
  EXPECT_THAT(bst.size(), Eq(ref.size()));
}

// Tests for clear

TEST(BSTSetCore, ClearEmpty) {
  BSTSet<int> bst;
  bst.clear();
  EXPECT_TRUE(bst.empty());
  EXPECT_THAT(bst.size(), Eq(0u));
}

TEST(BSTSetCore, ClearNonEmpty) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(3);
  bst.insert(7);
  bst.clear();
  EXPECT_TRUE(bst.empty());
  EXPECT_THAT(bst.size(), Eq(0u));
  EXPECT_THAT(bst.to_string(), StrEq("{}"));
  EXPECT_FALSE(bst.contains(5));
}

TEST(BSTSetCore, ClearThenReuse) {
  BSTSet<int> bst;
  bst.insert(1);
  bst.insert(2);
  bst.insert(3);
  bst.clear();

  bst.insert(10);
  bst.insert(20);
  EXPECT_THAT(bst.size(), Eq(2u));
  EXPECT_TRUE(bst.contains(10));
  EXPECT_TRUE(bst.contains(20));
  EXPECT_FALSE(bst.contains(1));
}

TEST(BSTSetCore, DoubleClear) {
  BSTSet<int> bst;
  bst.insert(1);
  bst.clear();
  bst.clear();
  EXPECT_TRUE(bst.empty());
}

// Destructor tests

TEST(BSTSetCore, DestructorLargeTree) {
  BSTSet<int>* bst = new BSTSet<int>();
  for (int i = 0; i < 1000; i++) {
    bst->insert(i);
  }
  delete bst;
}

// Copy constructor tests

TEST(BSTSetCore, CopyConstructorBasic) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(3);
  bst.insert(7);

  BSTSet<int> copy(bst);
  EXPECT_THAT(copy.size(), Eq(3u));
  EXPECT_THAT(copy.to_string(), StrEq(bst.to_string()));
  EXPECT_TRUE(copy.contains(3));
  EXPECT_TRUE(copy.contains(5));
  EXPECT_TRUE(copy.contains(7));
}

TEST(BSTSetCore, CopyConstructorEmpty) {
  BSTSet<int> bst;
  BSTSet<int> copy(bst);
  EXPECT_TRUE(copy.empty());
  EXPECT_THAT(copy.to_string(), StrEq("{}"));
}

TEST(BSTSetCore, CopyConstructorIndependence) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(3);
  bst.insert(7);

  BSTSet<int> copy(bst);

  bst.insert(1);
  bst.insert(10);

  EXPECT_THAT(copy.size(), Eq(3u));
  EXPECT_FALSE(copy.contains(1));
  EXPECT_FALSE(copy.contains(10));

  EXPECT_THAT(bst.size(), Eq(5u));
}

TEST(BSTSetCore, CopyConstructorDeepCopy) {
  BSTSet<int> bst;
  bst.insert(10);
  bst.insert(5);
  bst.insert(15);
  bst.insert(3);
  bst.insert(7);

  BSTSet<int> copy(bst);

  copy.clear();

  EXPECT_THAT(bst.size(), Eq(5u));
  EXPECT_TRUE(bst.contains(10));
  EXPECT_TRUE(bst.contains(5));
}

TEST(BSTSetCore, CopyConstructorLarge) {
  Random::seed(99);
  BSTSet<int> bst;
  set<int> ref;
  for (int i = 0; i < 200; i++) {
    int key = Random::randInt(500);
    bst.insert(key);
    ref.insert(key);
  }

  BSTSet<int> copy(bst);
  EXPECT_THAT(copy.to_string(), StrEq(set_to_string(ref)));
  EXPECT_THAT(copy.size(), Eq(ref.size()));
}

// Assignement operator tests

TEST(BSTSetCore, AssignmentBasic) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(3);
  bst.insert(7);

  BSTSet<int> other;
  other.insert(100);

  other = bst;
  EXPECT_THAT(other.size(), Eq(3u));
  EXPECT_THAT(other.to_string(), StrEq(bst.to_string()));
  EXPECT_FALSE(other.contains(100));
}

TEST(BSTSetCore, AssignmentSelf) {
  BSTSet<int> bst;
  bst.insert(1);
  bst.insert(2);
  bst.insert(3);

  BSTSet<int>& ref = bst;
  ref = bst;

  EXPECT_THAT(bst.size(), Eq(3u));
  EXPECT_TRUE(bst.contains(1));
  EXPECT_TRUE(bst.contains(2));
  EXPECT_TRUE(bst.contains(3));
}

TEST(BSTSetCore, AssignmentIndependence) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(3);

  BSTSet<int> other;
  other = bst;

  bst.insert(99);
  EXPECT_FALSE(other.contains(99));
  EXPECT_THAT(other.size(), Eq(2u));
}

TEST(BSTSetCore, AssignmentFromEmpty) {
  BSTSet<int> bst;
  bst.insert(1);
  bst.insert(2);

  BSTSet<int> empty;
  bst = empty;
  EXPECT_TRUE(bst.empty());
  EXPECT_THAT(bst.to_string(), StrEq("{}"));
}

TEST(BSTSetCore, AssignmentToEmpty) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(10);

  BSTSet<int> empty;
  empty = bst;
  EXPECT_THAT(empty.size(), Eq(2u));
  EXPECT_TRUE(empty.contains(5));
  EXPECT_TRUE(empty.contains(10));
}

TEST(BSTSetCore, AssignmentChain) {
  BSTSet<int> a, b, c;
  a.insert(1);
  a.insert(2);
  a.insert(3);

  c = b = a;
  EXPECT_THAT(b.size(), Eq(3u));
  EXPECT_THAT(c.size(), Eq(3u));
  EXPECT_THAT(b.to_string(), StrEq(a.to_string()));
  EXPECT_THAT(c.to_string(), StrEq(a.to_string()));
}

// Large test (stress test)

TEST(BSTSetCore, RandomInsertContainsStress) {
  Random::seed(123);
  BSTSet<int> bst;
  set<int> ref;

  for (int i = 0; i < 500; i++) {
    int key = Random::randInt(300);
    bst.insert(key);
    ref.insert(key);
  }

  EXPECT_THAT(bst.size(), Eq(ref.size()));
  EXPECT_THAT(bst.to_string(), StrEq(set_to_string(ref)));

  for (int i = 0; i <= 300; i++) {
    EXPECT_THAT(bst.contains(i), Eq(ref.count(i) > 0));
  }
}

// Tests for Augmented
// Remove min

TEST(BSTSetAugmented, RemoveMinThrowsOnEmpty) {
  BSTSet<int> bst;
  EXPECT_THROW(bst.remove_min(), runtime_error);
}

TEST(BSTSetAugmented, RemoveMinSingleElement) {
  BSTSet<int> bst;
  bst.insert(5);
  EXPECT_THAT(bst.remove_min(), Eq(5));
  EXPECT_TRUE(bst.empty());
  EXPECT_THAT(bst.size(), Eq(0u));
}

TEST(BSTSetAugmented, RemoveMinUpdatesSize) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(3);
  bst.insert(7);
  bst.remove_min();
  EXPECT_THAT(bst.size(), Eq(2u));
  bst.remove_min();
  EXPECT_THAT(bst.size(), Eq(1u));
}

TEST(BSTSetAugmented, RemoveMinReturnsCorrectValue) {
  BSTSet<int> bst;
  bst.insert(10);
  bst.insert(5);
  bst.insert(15);
  bst.insert(3);
  EXPECT_THAT(bst.remove_min(), Eq(3));
  EXPECT_THAT(bst.remove_min(), Eq(5));
  EXPECT_THAT(bst.remove_min(), Eq(10));
  EXPECT_THAT(bst.remove_min(), Eq(15));
}

TEST(BSTSetAugmented, RemoveMinRootIsMin) {
  BSTSet<int> bst;
  bst.insert(1);
  bst.insert(5);
  bst.insert(10);
  EXPECT_THAT(bst.remove_min(), Eq(1));
  EXPECT_THAT(bst.size(), Eq(2u));
  EXPECT_TRUE(bst.contains(5));
  EXPECT_TRUE(bst.contains(10));
}

TEST(BSTSetAugmented, RemoveMinPreservesRightChild) {
  BSTSet<int> bst;
  bst.insert(10);
  bst.insert(5);
  bst.insert(15);
  bst.insert(7);
  EXPECT_THAT(bst.remove_min(), Eq(5));
  EXPECT_TRUE(bst.contains(7));
  EXPECT_TRUE(bst.contains(10));
  EXPECT_TRUE(bst.contains(15));
  EXPECT_THAT(bst.size(), Eq(3u));
}

TEST(BSTSetAugmented, RemoveMinRootWithRightChild) {
  BSTSet<int> bst;
  bst.insert(1);
  bst.insert(5);
  bst.insert(3);
  bst.insert(7);
  EXPECT_THAT(bst.remove_min(), Eq(1));
  EXPECT_THAT(bst.size(), Eq(3u));
  EXPECT_TRUE(bst.contains(3));
  EXPECT_TRUE(bst.contains(5));
  EXPECT_TRUE(bst.contains(7));
}

TEST(BSTSetAugmented, RemoveMinAllElements) {
  Random::seed(42);
  BSTSet<int> bst;
  set<int> ref;
  for (int i = 0; i < 20; i++) {
    int key = Random::randInt(100);
    bst.insert(key);
    ref.insert(key);
  }

  while (!ref.empty()) {
    int expected = *ref.begin();
    EXPECT_THAT(bst.remove_min(), Eq(expected));
    ref.erase(ref.begin());
    EXPECT_THAT(bst.size(), Eq(ref.size()));
  }
  EXPECT_TRUE(bst.empty());
}

// Tests for begin / next

TEST(BSTSetAugmented, BeginNextEmpty) {
  BSTSet<int> bst;
  bst.begin();
  int val;
  EXPECT_FALSE(bst.next(val));
}

TEST(BSTSetAugmented, BeginNextSingleElement) {
  BSTSet<int> bst;
  bst.insert(42);
  bst.begin();
  int val;
  EXPECT_TRUE(bst.next(val));
  EXPECT_THAT(val, Eq(42));
  EXPECT_FALSE(bst.next(val));
}

TEST(BSTSetAugmented, BeginNextInOrder) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(3);
  bst.insert(7);
  bst.insert(1);
  bst.insert(4);

  vector<int> result;
  bst.begin();
  int val;
  while (bst.next(val)) {
    result.push_back(val);
  }

  EXPECT_THAT(result, ElementsAre(1, 3, 4, 5, 7));
}

TEST(BSTSetAugmented, BeginNextReturnsFalseAfterDone) {
  BSTSet<int> bst;
  bst.insert(1);
  bst.insert(2);

  bst.begin();
  int val;
  while (bst.next(val)) {
  }

  EXPECT_FALSE(bst.next(val));
  EXPECT_FALSE(bst.next(val));
}

TEST(BSTSetAugmented, BeginNextLeftOnly) {
  BSTSet<int> bst;
  bst.insert(5);
  bst.insert(4);
  bst.insert(3);
  bst.insert(2);
  bst.insert(1);

  vector<int> result;
  bst.begin();
  int val;
  while (bst.next(val)) {
    result.push_back(val);
  }

  EXPECT_THAT(result, ElementsAre(1, 2, 3, 4, 5));
}

TEST(BSTSetAugmented, BeginNextRightOnlyIncreasing) {
  BSTSet<int> bst;
  bst.insert(1);
  bst.insert(2);
  bst.insert(3);
  bst.insert(4);
  bst.insert(5);

  vector<int> result;
  bst.begin();
  int val;
  while (bst.next(val)) {
    result.push_back(val);
  }

  EXPECT_THAT(result, ElementsAre(1, 2, 3, 4, 5));
}

TEST(BSTSetAugmented, BeginNextNoSkipsOrDuplicates) {
  Random::seed(55);
  BSTSet<int> bst;
  set<int> ref;
  for (int i = 0; i < 10; i++) {
    int key = Random::randInt(50);
    bst.insert(key);
    ref.insert(key);
  }

  vector<int> result;
  bst.begin();
  int val;
  while (bst.next(val)) {
    result.push_back(val);
  }

  vector<int> expected(ref.begin(), ref.end());
  EXPECT_THAT(result, Eq(expected));
}

TEST(BSTSetAugmented, BeginNextMultipleSeeds) {
  for (int seed = 0; seed < 5; seed++) {
    Random::seed(seed);
    BSTSet<int> bst;
    set<int> ref;
    for (int i = 0; i < 10; i++) {
      int key = Random::randInt(30);
      bst.insert(key);
      ref.insert(key);
    }

    vector<int> result;
    bst.begin();
    int val;
    while (bst.next(val)) {
      result.push_back(val);
    }

    vector<int> expected(ref.begin(), ref.end());
    EXPECT_THAT(result, Eq(expected));
  }
}

TEST(BSTSetAugmented, BeginResetsTraversal) {
  BSTSet<int> bst;
  bst.insert(3);
  bst.insert(1);
  bst.insert(5);

  bst.begin();
  int val;
  bst.next(val);

  bst.begin();
  vector<int> result;
  while (bst.next(val)) {
    result.push_back(val);
  }

  EXPECT_THAT(result, ElementsAre(1, 3, 5));
}

// Tests for erase

TEST(BSTSetErase, ThrowsWhenNotFound) {
  BSTSet<int> bst;
  EXPECT_THROW(bst.erase(5), out_of_range);

  bst.insert(10);
  EXPECT_THROW(bst.erase(5), out_of_range);
}

TEST(BSTSetErase, EraseLeaf) {
  BSTSet<int> bst;
  bst.insert(10);
  bst.insert(5);
  bst.insert(15);
  EXPECT_THAT(bst.erase(5), Eq(5));
  EXPECT_THAT(bst.size(), Eq(2u));
  EXPECT_FALSE(bst.contains(5));
  EXPECT_TRUE(bst.contains(10));
  EXPECT_TRUE(bst.contains(15));
}

TEST(BSTSetErase, EraseRootNoChildren) {
  BSTSet<int> bst;
  bst.insert(10);
  EXPECT_THAT(bst.erase(10), Eq(10));
  EXPECT_TRUE(bst.empty());
  EXPECT_THAT(bst.to_string(), StrEq("{}"));
}

TEST(BSTSetErase, EraseNodeOneChildLeft) {
  BSTSet<int> bst;
  bst.insert(10);
  bst.insert(5);
  bst.insert(3);
  EXPECT_THAT(bst.erase(5), Eq(5));
  EXPECT_THAT(bst.size(), Eq(2u));
  EXPECT_TRUE(bst.contains(3));
  EXPECT_TRUE(bst.contains(10));
}

TEST(BSTSetErase, EraseNodeOneChildRight) {
  BSTSet<int> bst;
  bst.insert(10);
  bst.insert(5);
  bst.insert(7);
  EXPECT_THAT(bst.erase(5), Eq(5));
  EXPECT_THAT(bst.size(), Eq(2u));
  EXPECT_TRUE(bst.contains(7));
  EXPECT_TRUE(bst.contains(10));
}

TEST(BSTSetErase, EraseNodeTwoChildren) {
  BSTSet<int> bst;
  bst.insert(10);
  bst.insert(5);
  bst.insert(15);
  bst.insert(3);
  bst.insert(7);
  EXPECT_THAT(bst.erase(5), Eq(5));
  EXPECT_THAT(bst.size(), Eq(4u));
  EXPECT_TRUE(bst.contains(3));
  EXPECT_TRUE(bst.contains(7));
  EXPECT_TRUE(bst.contains(10));
  EXPECT_TRUE(bst.contains(15));
}

TEST(BSTSetErase, EraseRootTwoChildren) {
  BSTSet<int> bst;
  bst.insert(10);
  bst.insert(5);
  bst.insert(15);
  EXPECT_THAT(bst.erase(10), Eq(10));
  EXPECT_THAT(bst.size(), Eq(2u));
  EXPECT_TRUE(bst.contains(5));
  EXPECT_TRUE(bst.contains(15));
}

TEST(BSTSetErase, ErasePreservesDescendants) {
  BSTSet<int> bst;
  bst.insert(20);
  bst.insert(10);
  bst.insert(30);
  bst.insert(5);
  bst.insert(15);
  bst.insert(25);
  bst.insert(35);
  EXPECT_THAT(bst.erase(10), Eq(10));
  EXPECT_THAT(bst.size(), Eq(6u));
  EXPECT_TRUE(bst.contains(5));
  EXPECT_TRUE(bst.contains(15));
  EXPECT_TRUE(bst.contains(20));
  EXPECT_TRUE(bst.contains(25));
  EXPECT_TRUE(bst.contains(30));
  EXPECT_TRUE(bst.contains(35));
}

TEST(BSTSetErase, EraseSuccessorHasRightChild) {
  BSTSet<int> bst;
  bst.insert(20);
  bst.insert(10);
  bst.insert(30);
  bst.insert(5);
  bst.insert(15);
  bst.insert(12);
  bst.insert(18);
  bst.insert(13);
  EXPECT_THAT(bst.erase(10), Eq(10));
  EXPECT_THAT(bst.size(), Eq(7u));
  EXPECT_TRUE(bst.contains(5));
  EXPECT_TRUE(bst.contains(12));
  EXPECT_TRUE(bst.contains(13));
  EXPECT_TRUE(bst.contains(15));
  EXPECT_TRUE(bst.contains(18));
  EXPECT_TRUE(bst.contains(20));
  EXPECT_TRUE(bst.contains(30));
}

TEST(BSTSetErase, EraseSuccessorNotRightChild) {
  BSTSet<int> bst;
  bst.insert(20);
  bst.insert(10);
  bst.insert(30);
  bst.insert(5);
  bst.insert(25);
  bst.insert(35);
  bst.insert(22);
  bst.insert(27);
  EXPECT_THAT(bst.erase(20), Eq(20));
  EXPECT_THAT(bst.size(), Eq(7u));
  EXPECT_TRUE(bst.contains(5));
  EXPECT_TRUE(bst.contains(10));
  EXPECT_TRUE(bst.contains(22));
  EXPECT_TRUE(bst.contains(25));
  EXPECT_TRUE(bst.contains(27));
  EXPECT_TRUE(bst.contains(30));
  EXPECT_TRUE(bst.contains(35));
}

TEST(BSTSetErase, EraseAllRandom) {
  Random::seed(77);
  BSTSet<int> bst;
  set<int> ref;
  for (int i = 0; i < 20; i++) {
    int key = Random::randInt(100);
    bst.insert(key);
    ref.insert(key);
  }

  vector<int> toRemove(ref.begin(), ref.end());
  shuffle_vec(toRemove);

  for (int key : toRemove) {
    EXPECT_THAT(bst.erase(key), Eq(key));
    ref.erase(key);
    EXPECT_THAT(bst.size(), Eq(ref.size()));
    EXPECT_FALSE(bst.contains(key));

    for (int remaining : ref) {
      EXPECT_TRUE(bst.contains(remaining));
    }
  }
  EXPECT_TRUE(bst.empty());
}

TEST(BSTSetErase, EraseMultipleSeeds) {
  for (int seed = 0; seed < 5; seed++) {
    Random::seed(seed);
    BSTSet<int> bst;
    set<int> ref;
    for (int i = 0; i < 15; i++) {
      int key = Random::randInt(50);
      bst.insert(key);
      ref.insert(key);
    }

    vector<int> toRemove(ref.begin(), ref.end());
    shuffle_vec(toRemove);

    for (int key : toRemove) {
      bst.erase(key);
      ref.erase(key);
      EXPECT_THAT(bst.size(), Eq(ref.size()));
    }
    EXPECT_TRUE(bst.empty());
  }
}

}  // namespace
