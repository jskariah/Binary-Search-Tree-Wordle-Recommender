CXX = clang++
# Unused: warn, but annoying to block compilation on
# Sign compare: noisy
# Command line arg: noisy, not relevant to students
CXXFLAGS = \
	-Wall -Wextra -Werror \
	-Wno-error=unused-function \
	-Wno-error=unused-parameter \
	-Wno-error=unused-variable \
	-Wno-error=unused-but-set-variable \
	-Wno-error=unused-value \
	-Wno-sign-compare \
	-Wno-unused-command-line-argument \
	-std=c++2a -I. -g -fno-omit-frame-pointer \
	-fsanitize=address,undefined

ENV_VARS = ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=suppr.txt:print_suppressions=false

# On Ubuntu and WSL, googletest is installed to /usr/include or
# /usr/local/include, which are used by default.

# On Mac, we need to manually include them in our path. Brew installs to
# different locations on Intel/Silicon, so ask brew where things live.
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	GTEST_PREFIX := $(shell brew --prefix googletest)
	LLVM_PREFIX := $(shell brew --prefix llvm)
	CXX := $(LLVM_PREFIX)/bin/clang++
	CXXFLAGS += -I$(GTEST_PREFIX)/include
	CXXFLAGS += -L$(GTEST_PREFIX)/lib
	CXXFLAGS += -L$(LLVM_PREFIX)/lib/c++
	CXXFLAGS += -Wno-character-conversion
endif

################
# BSTSet Tests #
################

build/bstset_tests.o: bstset_tests.cpp bstset.h
	mkdir -p build && $(CXX) $(CXXFLAGS) -c $< -o $@

bstset_tests: build/bstset_tests.o
	$(CXX) $(CXXFLAGS) $^ -lgtest -lgmock -lgtest_main -o $@

test_bst_core: bstset_tests
	$(ENV_VARS) ./$< --gtest_color=yes --gtest_filter="BSTSetCore*"

test_bst_aug: bstset_tests
	$(ENV_VARS) ./$< --gtest_color=yes --gtest_filter="BSTSetAugmented*"

test_bst_erase: bstset_tests
	$(ENV_VARS) ./$< --gtest_color=yes --gtest_filter="BSTSetErase*"

test_bst_all: bstset_tests
	$(ENV_VARS) ./$< --gtest_color=yes

################
# BST Set Main #
################

bstset_main: bstset_main.cpp bstset.h
	$(CXX) $(CXXFLAGS) bstset_main.cpp -lgtest -lgmock -lgtest_main -o $@

run_main: bstset_main
	$(ENV_VARS) ./$<

###############
# Wordle Main #
###############

wordle_main: build/wordle_main.o build/wordle.o
	$(CXX) $(CXXFLAGS) $^ -o $@

build/wordle_main.o: wordle_main.cpp bstset.h | build
	$(CXX) $(CXXFLAGS) -c wordle_main.cpp -o $@

build/wordle.o: wordle.cpp bstset.h | build
	$(CXX) $(CXXFLAGS) -c wordle.cpp -o $@

build: 
	mkdir -p build 

run_wordle: wordle_main
	$(ENV_VARS) ./$<

##################
# Wordle Web App #
##################

build/server_main.o: server/main.cpp bstset.h | build
	$(CXX) $(CXXFLAGS) -c server/main.cpp -o $@

wordle_server: build/server_main.o build/wordle.o
	$(CXX) $(CXXFLAGS) $^ -o $@

run_server: wordle_server
	$(ENV_VARS) ./$<

################
# Wordle Tests #
################

build/wordle_tests.o: wordle_tests.cpp bstset.h | build
	$(CXX) $(CXXFLAGS) -c wordle_tests.cpp -o $@

wordle_tests: build/wordle_tests.o build/wordle.o
	$(CXX) $(CXXFLAGS) $^ -lgtest -lgmock -lgtest_main -o $@

test_wordle: wordle_tests
	$(ENV_VARS) ./$< --gtest_color=yes --gtest_filter="Wordle*"

clean:
	rm -f bstset_tests bstset_main wordle_main wordle_server wordle_tests build/*
	# MacOS symbol cleanup
	rm -rf *.dSYM

.PHONY: clean run_main test_bst_core test_bst_aug test_bst_erase test_bst_all run_server run_wordle
