#include "include/worldle.h"

using namespace std;

int main() {
  // Program for running command line interface of the Wordle Recommender.
  // This won't work until all your functions have been implemented correctly in wordle.cpp.

  string filename = "words.txt";
  BSTSet<string> words = load_words(filename);
  recommender(words);
}