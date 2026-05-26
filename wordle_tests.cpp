#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/resource.h>

#include <iostream>
#include <limits>
#include <random>

#include "bstset.h"
#include "include/worldle.h"

using namespace std;
using namespace testing;

TEST(Wordle, LoadWords) {
  const string filename = "data/tiny_words.txt";

  BSTSet<std::string> words = load_words(filename);

  std::vector<string> expected = {"lousy", "awful", "great", "manic", "rabid"};

  ASSERT_EQ(words.size(), expected.size())
      << "Incorrect number of words loaded from " << filename << ".";

  for (const auto& word : expected) {
    EXPECT_TRUE(words.contains(word)) << "Missing expected word: " << word;
  }
}

TEST(Wordle, GetPattern_AllGreen) {
  EXPECT_THAT(get_pattern("RAISE", "RAISE"), StrEq("22222"))
      << "If the guess equals the winning word, the pattern should be all "
         "greens (22222).";
}

TEST(Wordle, GetPattern_NoMatches) {
  EXPECT_THAT(get_pattern("ABCDE", "FGHIJ"), StrEq("00000"))
      << "If no letters match, the pattern should be all gray (00000).";
}

TEST(Wordle, GetPattern_ExampleFromSpec1) {
  EXPECT_THAT(get_pattern("RAISE", "SCORE"), StrEq("10012"))
      << "Example from specification failed: get_pattern(\"RAISE\", \"SCORE\") "
         "should be 10012.";
}

TEST(Wordle, GetPattern_ExampleFromSpec2) {
  EXPECT_THAT(get_pattern("MERRY", "CORER"), StrEq("01210"))
      << "Example from specification failed: get_pattern(\"MERRY\", \"CORER\") "
         "should be 01210.";
}

TEST(Wordle, GetPattern_DuplicateLettersInGuess) {
  EXPECT_THAT(get_pattern("SHEEP", "PLATE"), StrEq("00101"))
      << "Duplicate letters in guess should only count if available in the "
         "winning word.";
}

TEST(Wordle, GetPattern_DuplicateLettersInGuessAndWinningWord) {
  EXPECT_THAT(get_pattern("SHEEP", "ELATE"), StrEq("00110"))
      << "Duplicate letters in the winning word and guess, but both in wrong "
         "position..";
}

TEST(Wordle, GetPattern_DuplicateLettersInWinningWord) {
  EXPECT_THAT(get_pattern("ALLOT", "TALLY"), StrEq("11201"))
      << "Duplicate letters in the winning word should be matched correctly "
         "without overcounting.";
}

TEST(Wordle, GetPattern_TATTYEdgeCase) {
  EXPECT_THAT(get_pattern("TOTAL", "TATTY"), StrEq("20210"))
      << "Handling multiple repeated letters failed that were correctly "
         "matched in guess.";
}

TEST(Wordle, GetPattern_TooManyDuplicateLetters) {
  EXPECT_THAT(get_pattern("AAAAA", "BANAL"), StrEq("02020"))
      << "Algorithm should not mark more yellow letters than exist in the "
         "winning word.";
}

TEST(Wordle, GetPattern_AllYellow) {
  EXPECT_THAT(get_pattern("ABCDE", "EABCD"), StrEq("11111"))
      << "All letters are present but in the wrong positions, so the pattern "
         "should be 11111.";
}

TEST(Wordle, GetPattern_AllYellow_WordExample) {
  EXPECT_THAT(get_pattern("LAYER", "RELAY"), StrEq("11111"))
      << "All letters appear in the winning word but in different positions.";
}

TEST(Wordle, FilterWords_Basic) {
  string keep_message =
      "If a word in set matches the feedback pattern it should remain after "
      "call to filter_words.";
  string filter_message =
      "If a word in set does not match the feedback patter it should be "
      "removed from the set.";

  BSTSet<string> words;
  words.insert("BASIC");
  words.insert("SONIC");
  words.insert("TONIC");
  words.insert("STOIC");

  filter_words(words, "BASIC", "00122");

  EXPECT_THAT(words.size(), Eq(2));

  EXPECT_THAT(words.contains("SONIC"), Eq(true)) << keep_message;

  EXPECT_THAT(words.contains("STOIC"), Eq(true)) << keep_message;

  EXPECT_THAT(words.contains("BASIC"), Eq(false)) << filter_message;

  EXPECT_THAT(words.contains("TONIC"), Eq(false)) << filter_message;
}

TEST(Wordle, FilterWords_AllGray) {
  string keep_message =
      "If a word in set matches the feedback pattern it should remain after "
      "call to filter_words.";
  string filter_message =
      "If a word in set does not match the feedback patter it should be "
      "removed from the set.";

  BSTSet<string> words;
  words.insert("CRANE");
  words.insert("PLUMB");
  words.insert("SWEPT");

  filter_words(words, "CRANE", "00000");

  EXPECT_THAT(words.size(), Eq(1));

  EXPECT_THAT(words.contains("PLUMB"), Eq(true)) << keep_message;

  EXPECT_THAT(words.contains("CRANE"), Eq(false)) << filter_message;
  EXPECT_THAT(words.contains("SWEPT"), Eq(false)) << filter_message;
}

TEST(Wordle, FilterWords_AllGreen) {
  BSTSet<string> words;
  words.insert("RAISE");
  words.insert("ARISE");
  words.insert("RAZED");

  filter_words(words, "RAISE", "22222");

  EXPECT_THAT(words.size(), Eq(1)) << "All green feedback should leave exactly "
                                      "one word if the guess is on the set.";

  EXPECT_THAT(words.contains("RAISE"), Eq(true))
      << "The winning word must remain in the set if it was in the set.";

  EXPECT_THAT(words.contains("ARISE"), Eq(false))
      << "All green feedback should leave only the guess.";
  EXPECT_THAT(words.contains("RAZED"), Eq(false))
      << "All green feedback should leave only the guess.";
}

TEST(Wordle, FilterWords_YellowPositionConstraint) {
  string keep_message =
      "If a word in set matches the feedback pattern it should remain after "
      "call to filter_words.";
  string filter_message =
      "If a word in set does not match the feedback patter it should be "
      "removed from the set.";

  BSTSet<string> words;
  words.insert("CRANE");
  words.insert("MULCH");
  words.insert("REACT");

  filter_words(words, "CRANE", "10000");

  EXPECT_TRUE(words.contains("MULCH")) << keep_message;

  EXPECT_FALSE(words.contains("CRANE")) << filter_message;

  EXPECT_FALSE(words.contains("REACT")) << filter_message;
}

TEST(Wordle, FilterWords_DuplicateLetters) {
  string keep_message =
      "If a word in set matches the feedback pattern it should remain after "
      "call to filter_words.";
  string filter_message =
      "If a word in set does not match the feedback patter it should be "
      "removed from the set.";

  BSTSet<string> words;
  words.insert("TATTY");
  words.insert("TASTY");
  words.insert("TAFFY");

  filter_words(words, "TOTAL", "20210");

  EXPECT_TRUE(words.contains("TATTY")) << keep_message;

  EXPECT_FALSE(words.contains("TASTY")) << filter_message;

  EXPECT_FALSE(words.contains("TAFFY")) << filter_message;
}

TEST(Wordle, FilterWords_NoMatchesRemain) {
  BSTSet<string> words;
  words.insert("CRANE");
  words.insert("SLATE");

  filter_words(words, "CRANE", "00000");

  EXPECT_THAT(words.size(), Eq(0));
}

TEST(Wordle, FilterWords_MultipleCalls) {
  string keep_message =
      "If a word in set matches the feedback pattern it should remain after "
      "call to filter_words.";
  string filter_message =
      "If a word in set does not match the feedback patter it should be "
      "removed from the set.";
  BSTSet<string> words;
  words.insert("CREEP");
  words.insert("EERIE");
  words.insert("BEECH");
  words.insert("CREPE");
  words.insert("EVADE");
  words.insert("EMCEE");
  words.insert("LEECH");

  filter_words(words, "BEECH", "01210");

  EXPECT_THAT(words.contains("CREEP"), Eq(true)) << keep_message;
  EXPECT_THAT(words.contains("CREPE"), Eq(true)) << keep_message;
  EXPECT_THAT(words.contains("EERIE"), Eq(false)) << filter_message;
  EXPECT_THAT(words.contains("BEECH"), Eq(false)) << filter_message;
  EXPECT_THAT(words.contains("EVADE"), Eq(false)) << filter_message;
  EXPECT_THAT(words.contains("EMCEE"), Eq(false)) << filter_message;
  EXPECT_THAT(words.contains("LEECH"), Eq(false)) << filter_message;

  EXPECT_THAT(words.size(), Eq(2));

  filter_words(words, "PERCH", "11110");
  EXPECT_THAT(words.contains("CREEP"), Eq(true)) << keep_message;
  EXPECT_THAT(words.contains("CREPE"), Eq(true)) << keep_message;
  EXPECT_THAT(words.size(), Eq(2));

  filter_words(words, "CAMPY", "20020");
  EXPECT_THAT(words.contains("CREEP"), Eq(false)) << filter_message;
  EXPECT_THAT(words.contains("CREPE"), Eq(true)) << keep_message;
  EXPECT_THAT(words.size(), Eq(1));
}

TEST(Wordle, RecommendSingleWord) {
  BSTSet<string> words;
  words.insert("APPLE");

  string rec = recommend(words);

  EXPECT_THAT(rec, StrEq("APPLE"));
}

TEST(Wordle, RecommendTwoWords) {
  BSTSet<string> words;
  words.insert("APPLE");
  words.insert("GRAPE");

  string rec = recommend(words);

  EXPECT_THAT(rec == "APPLE" || rec == "GRAPE", Eq(true));
}

TEST(Wordle, RecommendReturnsPlayableWord) {
  BSTSet<string> words;
  words.insert("BATCH");
  words.insert("CATCH");
  words.insert("HATCH");
  words.insert("LATCH");
  words.insert("PATCH");

  string rec = recommend(words);

  bool found = false;
  string value;
  words.begin();
  while (words.next(value)) {
    if (value == rec) {
      found = true;
    }
  }

  EXPECT_THAT(found, Eq(true));
}

TEST(Wordle, RecommendLargeFile) {
  cout << "This test may take a little longer while loading large file."
       << endl;
  BSTSet<string> og_words = load_words("words.txt");

  BSTSet<string> words = og_words;

  string rec = recommend(words);
  EXPECT_THAT(rec, StrEq("raise"))
      << "The recommendation should maximize entropy. ";

  filter_words(words, "raise", "00011");

  rec = recommend(words);
  EXPECT_THAT(rec, StrEq("spelt"));

  filter_words(words, "spelt", "21200");

  rec = recommend(words);
  EXPECT_THAT(rec, StrEq("sheep"));
  filter_words(words, "sheep", "22222");
  EXPECT_THAT(words.size(), Eq(1));

  words = og_words;
  rec = recommend(words);
  EXPECT_THAT(rec, StrEq("raise"))
      << "The recommendation should maximize entropy. ";
  filter_words(words, "raise", "11000");

  rec = recommend(words);
  EXPECT_THAT(rec, StrEq("adorn"));
  filter_words(words, "adorn", "10010");

  rec = recommend(words);
  EXPECT_THAT(rec, StrEq("tract"));
  filter_words(words, "tract", "02200");

  rec = recommend(words);
  EXPECT_THAT(rec, StrEq("graph"));
  filter_words(words, "graph", "02200");

  rec = recommend(words);
  EXPECT_THAT(rec, StrEq("brawl"));

  // Try where winning word is not in set ("trees").
  words = og_words;
  rec = recommend(words);
  EXPECT_THAT(rec, StrEq("raise"))
      << "The recommendation should maximize entropy. ";
  filter_words(words, "raise", "10011");

  rec = recommend(words);
  EXPECT_THAT(rec, StrEq("sheer"))
      << "The recommendation should maximize entropy. ";
  filter_words(words, "sheer", "10221");

  // No more words left in the dataset.
  EXPECT_THAT(words.size(), Eq(0));
}