#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/worldle.h"
#include "httplib.h"

using namespace std;
using namespace httplib;

/* ============================================================
   Global Solver State
   ============================================================ */

// static BSTSet<string> remaining_words;
static BSTSet<string> remaining_words;
static BSTSet<string> og_remaining_words;

static string dictionary_file = "./words.txt";

/* ============================================================
   Guess history (for Wordle-style board)
   ============================================================ */

struct GuessResult {
  string guess;
  string feedback;  // original G/Y/B form
};

static vector<GuessResult> history;

/* ============================================================
   Helper: load game
   ============================================================ */

void load_game() {
  og_remaining_words = load_words(dictionary_file);
  remaining_words = og_remaining_words;
  history.clear();
}

/* ============================================================
   Helper: reset game
   ============================================================ */

void reset_game() {
  remaining_words = og_remaining_words;
  history.clear();
}

/* ============================================================
   Helper: convert UI feedback to numeric form

   UI sends:    G Y B
   Solver wants:2 1 0
   ============================================================ */

string convert_feedback_to_digits(const string& fb) {
  string out;
  out.reserve(fb.size());

  for (char c : fb) {
    switch (toupper(c)) {
      case 'G':
        out.push_back('2');
        break;
      case 'Y':
        out.push_back('1');
        break;
      case 'B':
        out.push_back('0');
        break;
      default:
        out.push_back('0');
        break;  // safe fallback
    }
  }
  return out;
}

/* ============================================================
   Build history string for UI
   ============================================================ */

string build_history_string() {
  ostringstream oss;

  for (const auto& entry : history) {
    oss << entry.guess << " " << entry.feedback << "\n";
  }

  return oss.str();
}

/* ============================================================
   Main
   ============================================================ */

int main() {
  cout << "Loading dictionary...\n";
  load_game();
  reset_game();
  cout << "Dictionary loaded... read " << remaining_words.size()
       << " total words." << endl;

  Server svr;

  /* ---------------- GET /next ---------------- */

  svr.Get("/next", [](const Request&, Response& res) {
    try {
      if (remaining_words.size() == 0) {
        res.status = 400;
        res.set_content("ERROR: no candidates remaining", "text/plain");
        return;
      }

      string guess = recommend(remaining_words);
      res.set_content(guess, "text/plain");

    } catch (const exception& e) {
      res.status = 500;
      res.set_content(string("ERROR: ") + e.what(), "text/plain");
    }
  });

  /* ---------------- GET /history ---------------- */

  svr.Get("/history", [](const Request&, Response& res) {
    res.set_content(build_history_string(), "text/plain");
  });

  /* ---------------- POST /feedback ---------------- */
  // Expected body from UI: "raise GYBBY"

  svr.Post("/feedback", [](const Request& req, Response& res) {
    try {
      istringstream iss(req.body);
      string guess, feedback;
      iss >> guess >> feedback;

      if (guess.empty() || feedback.size() != guess.size()) {
        res.status = 400;
        res.set_content("ERROR: invalid feedback format", "text/plain");
        return;
      }

      // store BEFORE filtering
      history.push_back({guess, feedback});

      // Convert to numeric format for student code
      string numeric_feedback = convert_feedback_to_digits(feedback);

      std::transform(guess.begin(), guess.end(), guess.begin(),
                     [](unsigned char c) { return std::tolower(c); });

      filter_words(remaining_words, guess, numeric_feedback);
      res.set_content("ok", "text/plain");

    } catch (const exception& e) {
      res.status = 500;
      res.set_content(string("ERROR: ") + e.what(), "text/plain");
    }
  });

  /* ---------------- POST /reset ---------------- */

  svr.Post("/reset", [](const Request&, Response& res) {
    reset_game();
    res.set_content("ok", "text/plain");
  });

  /* ---------------- Static UI Hosting ---------------- */

  // Serves files from ./web (index.html, etc.)
  svr.set_mount_point("/", "./web");

  cout << "Server running at http://localhost:8080\n";
  svr.listen("localhost", 8080);
}