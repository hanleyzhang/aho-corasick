#ifndef AHOCORASICK_HPP
#define AHOCORASICK_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <set>
#include <queue>
#include <sstream>

/**
 * Represent a state in the state machine
 * @tparam EntryType
 * @tparam OutputType
 */
template<typename EntryType, typename OutputType>
class State {

  struct Edge {
    EntryType entry;
    std::weak_ptr<State<EntryType, OutputType>> dest;
  };

private:
  std::vector<Edge> edges;
  std::vector<OutputType> outputs;
  std::weak_ptr<State> fail;

public:

  State() : fail(std::weak_ptr<State>()) {}

  /**
   * The fail function
   */
  std::weak_ptr<State> get_fail() {
    return fail;
  }

  /**
   * Set the fail function
   */
  void set_fail(std::weak_ptr<State>& f) {
    fail = f;
  }

  /**
   * Set of output of this state, can be empty
   */
  std::vector<OutputType> get_outputs() {
    return outputs;
  }

  /**
   * The list of entries going out of this state
   */
  std::set<EntryType> get_entries() {
    std::set<EntryType> ls;
    for (const Edge& e: edges) {
      ls.insert(e.entry);
    }
    return ls;
  }

  /**
   * Add a child to this state
   */
  int add_child(const EntryType &entry, std::weak_ptr<State<EntryType, OutputType>> state) {
    if (!go(entry).expired()) {
      // already contain this entry
      return -1;
    }

    Edge e;
    e.entry = entry;
    e.dest = state;
    edges.push_back(e);
    return 0;
  }

  /**
   * Add an output symbol to this state
   */
  void add_output(const OutputType out) {
    outputs.push_back(out);
  }

  /**
   * The go function
   */
  std::weak_ptr<State> go(const EntryType &entry) {
    for (const Edge &e: edges) {
      if (e.entry == entry) {
        return e.dest;
      }
    }
    return std::weak_ptr<State>();
  }

};

/**
 * Implement the Aho-Corasick algorithm
 */
class AhoCorasick {

  typedef State<char, unsigned long> StateType;

  std::vector<std::shared_ptr<StateType>> states;
  std::set<char> vocabulary;

  /**
   * Preprocess the given string: remove duplicated spaces, and update vocabulary if needed
   * @param s
   * @param update_vocab How to deal with characters that are not in the vocabulary:
   *    true to update vocabulary, false to eliminate it from the string
   */
  std::string preprocess_string(const std::string& s, bool update_vocab) {
    std::stringbuf out;
    char last_char = tolower(s[0]);
    for (const char& c: s) {
      char l = tolower(c);

      // not in vocabulary and not update vocab -> ignore
      if (vocabulary.find(l) == vocabulary.end() and !update_vocab) {
        last_char = l;
        continue;
      }

      // multiple spaces -> ignore
      if (last_char == ' ' and last_char == l) {
        last_char = l;
        continue;
      }

      vocabulary.insert(l);
      out.sputc(l);
      last_char = l;
    }
    return out.str();
  }

public:

  AhoCorasick(std::set<char> &vocab) {
    for (const char& c: vocab) {
      vocabulary.insert(std::tolower(c));
    }
    states.push_back(std::shared_ptr<StateType>(std::make_shared<StateType>()));
  }

  /**
   * The root of the trie
   */
  std::weak_ptr<StateType> root() const {
    return states.at(0);
  }

  /**
   * Initialize the state machine
   */
  int initialize(std::vector<std::string> &dict) {
    if (states.size() != 1) {
      return -1;
    }

    // build the state machine
    for (std::size_t i = 0; i < dict.size(); ++i) {
      std::weak_ptr<StateType> p = root();
      std::string s = preprocess_string(dict[i], true);

      for (char &c: s) {
        std::shared_ptr<StateType> locked_p(p);
        std::weak_ptr<StateType> child = locked_p->go(c);

        if (child.expired()) {
          std::shared_ptr<StateType> new_state(std::make_shared<StateType>());
          locked_p->add_child(c, new_state);
          states.push_back(new_state);
          p = new_state;
        } else {
          p = child;
        }
      }
      std::shared_ptr<StateType>(p)->add_output(i);
    }

    std::weak_ptr<StateType> root_weak = root();
    std::shared_ptr<StateType> root(root_weak);

    // add loops into the root node
    std::set<char> root_entries = root->get_entries();
    for (const char& c: vocabulary) {
      if (root_entries.find(c) == root_entries.end()) {
        // c is not in root entries, add loop
        root->add_child(c, root_weak);
      }
    }

    // compute the fail() function
    std::queue<std::weak_ptr<StateType>> state_queue;
    for (const char& c: vocabulary) {
      std::weak_ptr<StateType> q = root->go(c);
      std::shared_ptr<StateType> locked_q(q);
      if (locked_q != root_weak.lock()) {
        locked_q->set_fail(root_weak);
        state_queue.push(q);
      }
    }

    while (!state_queue.empty()) {
      const std::weak_ptr<StateType>& r = state_queue.front();
      state_queue.pop();
      std::shared_ptr<StateType> locked_r(r);

      for (const char& c: locked_r->get_entries()) {
        std::weak_ptr<StateType> u = locked_r->go(c);
        state_queue.push(u);

        // follow the fail link until we can keep going with the current input c
        std::shared_ptr<StateType> v(locked_r->get_fail());
        while((v->go(c)).expired()) {
          v = std::shared_ptr<StateType>(v->get_fail());
        }

        // update the fail function and outputs of u
        std::weak_ptr<StateType> fu = v->go(c);
        std::shared_ptr<StateType>locked_u(u);

        locked_u->set_fail(fu);
        for (const long& l: std::shared_ptr<StateType>(fu)->get_outputs()) {
          locked_u->add_output(l);
        }
      }
    }

    return 0;
  }

  /**
   *
   * @param text
   * @return
   */
  std::vector<unsigned long> parse(const std::string &text) {
    // replace spaces and punctuations
    std::string s = preprocess_string(text, false);
    std::shared_ptr<StateType> q(root());
    std::vector<unsigned long> result;

    for (const char& c: s) {
      while (q->go(c).expired()) {
        q = std::shared_ptr<StateType>(q->get_fail());
      }
      q = std::shared_ptr<StateType>(q->go(c));
      std::vector<unsigned long> outs = q->get_outputs();
      result.insert(result.end(), outs.begin(), outs.end());
    }
    return result;
  }

};

#endif  // AHOCORASICK_HPP