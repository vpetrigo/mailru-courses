#include <iostream>
#include <stack>
#include <string>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include "Deque.hpp"

bool is_appropriate_brackets(const char left_bracket, const char right_bracket) {
  // because we have only '()', '[]' and '{}' brackets which have ASCII code
  // [40, 41], [91, 92], [123, 125] here is the fast solution
  const auto diff = right_bracket - left_bracket;

  return (diff == 1) || (diff == 2) ? true : false;
}

std::string check_brackets(const std::string& bracket_seq) {
  // map to compare open and close brackets
  const std::unordered_map<char, char> brackets {{'(', ')'}, {'[', ']'}, {'{', '}'},
                                                 {')', '('}, {']', '['}, {'}', '{'}};
  constexpr auto close_brackets = ")]}";
  constexpr auto string_begining = 0;
  std::string answer;
  std::stack<char, Deque<char>> stack;
  char *close_br_occurence = nullptr;

  for (const auto& bracket : bracket_seq) {
    if ((close_br_occurence = std::strchr(close_brackets, bracket)) != nullptr) {
      if (stack.empty()) {
        // Add open bracket to the left
        answer.insert(string_begining, 1, brackets.at(bracket));
        answer.push_back(bracket);
      }
      else {
        // Check whether previous bracket is an appropriate open bracket
        const auto& left_bracket = stack.top();
        stack.pop();

        if (is_appropriate_brackets(left_bracket, bracket)) {
          answer.push_back(bracket);
        }
        else {
          // empty string
          return {};
        }
      }
    }
    else {
      // Handle open bracket
      answer.push_back(bracket);
      stack.push(bracket);
    }
  }

  while (!stack.empty()) {
    const auto& open_br = stack.top();
    stack.pop();

    answer.push_back(brackets.at(open_br));
  }

  return answer;
}

int main() {
  std::stack<char, Deque<char>> st;
  std::string bracket_sequence;

  std::getline(std::cin, bracket_sequence);

  const auto&& answer = check_brackets(bracket_sequence);

  if (!answer.empty()) {
    std::cout << answer;
  }
  else {
    std::cout << "IMPOSSIBLE";
  }
  std::cout << std::endl;

  return 0;
}
