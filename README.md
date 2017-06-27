# Aho-Corasick algorithm

Implementation of the Aho-Corasick algorithm for exact matching 
against a set of words.

See [wikipedia](https://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_algorithm) 
or [slides](https://www.cs.uku.fi/~kilpelai/BSA05/lectures/slides04.pdf) for more information.

### Compile

```bash
cd /path/to/aho-corasick
./compile.sh
```

This will give two executables named `ahocorasick` and `ahocorasick-test`.

`ahocorasick-test` contains some very simple tests:

```bash
./ahocorasick-test 
PASSED
```

`ahocorasick` is the main program, and can be executed in multiple ways:

```

./ahocorasick
Usage: ./ahocorasick dictionary-file [quoted-test-string]

# parse the given string (in quotes)
./ahocorasick problem-dict.txt "Which restaurants do West Indian food???"
Read 14 keywords from problem-dict.txt
Parse: Which restaurants do West Indian food???
West Indian
Indian


./ahocorasick problem-dict.txt "What      is the    weather like today?."
Read 14 keywords from problem-dict.txt
Parse: What      is the    weather like today?.
none

./ahocorasick problem-dict.txt "Which    restaurants do West     indian food???"
Read 14 keywords from problem-dict.txt
Parse: Which    restaurants do West     indian food???
West Indian
Indian

# parse interactively
./ahocorasick problem-dict.txt 
Read 14 keywords from problem-dict.txt
Press Ctrl+C when you are bored.
Input string: Which    restaurants do West     indian food???
West Indian
Indian
Input string: Find me a place that does tapas
none
Input string: ^C
```

### Complexity

- Initialization: the initialization of the (enhanced) trie has `O(m)` complexity where `m` is 
the number of keywords.

- Parsing: `O(n+z)` where `n` is the length of the input string, and `z` is the number of matches. 
This is because the algorithm follows the `fail` links and does not need 
to try every single character in the input string.

### Assumptions

- Only works with latin alphabet for now, but support for other language
is provided as long as the input file is in UTF-8.

