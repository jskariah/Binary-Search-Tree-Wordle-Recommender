#include <unordered_map>

#include "../bstset.h"

using namespace std;

/**
 * Given a filename that contains a list of words (one word per line),
 * returns a set that contains all the words in the file.
 */
BSTSet<string> load_words(const string& filename);

/**
 * Given a `guess` and the potential `winning_word`, computes the pattern
 * that Wordle would give us, as a string in which each character
 * corresponds to the character position on the original string and the
 * colored feedback is output in numbers as:
 *
 * - 0 - Gray
 * - 1 - Yellow
 * - 2 - Green
 *
 * For example:
 * - `get_pattern("RAISE", "SCORE") == "10012"`
 * - `get_pattern("MERRY", "CORER") == "01210"`
 */
string get_pattern(const string& guess, const string& winning_word);

/**
 * Given the current `remaining_words`, keep only the words that conform
 * to the given `feedback`, based on the given `guess`.
 *
 * For example:
 * If `remaining_words` contains {"BASIC", "SONIC", "TONIC", "STOIC"}, then:
 *
 * - `filter_words(remaining_words, "BASIC", "00122") will erase `"BASIC"` and
 * `"TONIC"` from `remaining_words`.
 *
 * In this case, the `feedback` says that the word should have an 'S' (not in
 * the middle position), therefore we filter out "TONIC". It also says that it
 * doesn't have an 'A' nor a 'B', so we filter out "BASIC". Given the
 * `feedback`, "STOIC" and "SONIC" are still possible winning words that
 * remain in `remaining_words`.
 */
void filter_words(BSTSet<string>& remaining_words, const string& guess,
                  const string& feedback);

/**
 * Given a set of words that are still playable, returns
 * the word that maximizes the entropy.
 */
string recommend(BSTSet<string>& remaining_words);

/**************************************************************************
 *                                                                        *
 * Implementations for the following functions will be given to students  *
 * in wordle.cpp.                                                         *
 *                                                                        *
 **************************************************************************/

/**
 * Computes the entropy of a candidate `guess`, given the
 * `remaining_words` that are still available to play.
 *
 * This will be given to the students.
 */
double entropy(const string& guess, const BSTSet<string>& remaining_words);

/**
 * Given a set of words will continually recommend one based on the
 * principle of optimizing entropy.
 *
 * After each recommendation, user is responsible to enter the feedback
 * they got from playing a Wordle word in a Wordle game engine.
 *
 * Will stop once the user notifies that they won the Wordle game.
 */
void recommender(BSTSet<string>& remaining_words);
