# BSTSet and Wordle Recommender

## Overview

This project is a C++ data structures project that implements a custom templated binary search tree set, `BSTSet`, and applies it to a Wordle-style word recommender.

The project has two main parts:

1. A custom `BSTSet<T>` class implemented using a binary search tree
2. A Wordle recommender that uses `BSTSet<string>` to store, filter, and recommend possible words

The main implementation work was completed in:

- `bstset.h`
- `wordle.cpp`

## Features

### BSTSet

The `BSTSet` class is a custom set implementation backed by a binary search tree.

It supports:

- Inserting unique values
- Checking whether a value exists
- Returning the size of the set
- Checking whether the set is empty
- Clearing all values
- Copy construction
- Assignment operator support
- In-order string conversion
- Removing the minimum value
- In-order traversal using `begin()` and `next()`
- Erasing values from the tree

### Wordle Recommender

The Wordle portion of the project uses the custom `BSTSet` to manage possible Wordle words.

It supports:

- Loading words from a file
- Computing Wordle feedback patterns
- Filtering possible words based on feedback
- Recommending a word based on entropy scoring

## Technologies Used

- C++
- Templates
- Classes
- Pointers
- Dynamic memory allocation
- Binary search trees
- Recursion
- File input/output
- GoogleTest
  

## Project Structure

```text
BSTSet/
├── bstset.h
├── bstset_main.cpp
├── bstset_tests.cpp
├── wordle.cpp
├── wordle_main.cpp
├── wordle_tests.cpp
├── words.txt
├── suppr.txt
├── .gitattributes
├── .gitignore
└── README.md
```

## File Descriptions

| File | Description |
|---|---|
| `bstset.h` | Main implementation of the templated binary search tree set |
| `bstset_main.cpp` | Small runner file for manually testing `BSTSet` |
| `bstset_tests.cpp` | Test file for the `BSTSet` implementation |
| `wordle.cpp` | Wordle helper and recommender implementation |
| `wordle_main.cpp` | Runs the command-line Wordle recommender |
| `wordle_tests.cpp` | Test file for the Wordle recommender functions |
| `words.txt` | Word list used by the Wordle recommender |
| `suppr.txt` | AddressSanitizer suppression file |
| `.gitattributes` | Git line-ending configuration |

## BSTSet Implementation

`BSTSet<T>` is implemented using a binary search tree where each node stores:

- A value
- A parent pointer
- A left child pointer
- A right child pointer

The set stores unique values, meaning duplicate insertions do not increase the size of the set.

Core operations include:

```cpp
insert(value);
contains(value);
size();
empty();
clear();
to_string();
erase(value);
remove_min();
begin();
next(value);
```

## Wordle Recommender Implementation

The Wordle recommender uses the custom `BSTSet<string>` class to store possible five-letter words.

The main Wordle functions include:

```cpp
load_words(filename);
get_pattern(guess, winning_word);
filter_words(remaining_words, guess, feedback);
recommend(remaining_words);
```

### Wordle Feedback Pattern

The program represents Wordle feedback using numbers:

| Number | Meaning |
|---|---|
| `0` | Gray: letter is not in the word |
| `1` | Yellow: letter is in the word but in the wrong position |
| `2` | Green: letter is in the correct position |

Example:

```text
Guess: RAISE
Winning Word: SCORE
Pattern: 10012
```

## What I Learned

Through this project, I practiced:

- Implementing a custom templated data structure
- Using binary search tree properties for insertion, search, traversal, and deletion
- Managing dynamic memory safely
- Working with parent, left, and right node pointers
- Writing recursive helper functions
- Implementing copy constructors and assignment operators
- Performing in-order traversal
- Applying a data structure to a real application
- Modeling Wordle feedback logic
- Filtering possible solutions based on user feedback
- Using entropy to recommend strong Wordle guesses
- Testing and debugging a larger C++ project

## Notes

This repository is intended to showcase the implementation and functionality of the BSTSet and Wordle recommender project.

Generated build files, compiled executables, editor-specific files, zip files, and course handouts are intentionally excluded from the repository.

## Future Improvements

Potential future improvements include:

- Adding tree visualization
- Adding balancing logic such as AVL or Red-Black Tree rotations
- Improving Wordle recommendation speed
- Supporting different word lengths
- Adding a graphical Wordle helper interface
- Displaying remaining possible words after each filter step
- Adding more detailed performance analysis

## Author

Created by Justin Skariah.

Primary implementation work completed in `bstset.h` and `wordle.cpp`.
