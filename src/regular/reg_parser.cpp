#include "regular/reg_parser.hpp"
#include <memory>
#include <stdexcept>

const std::unordered_map<char, size_t> RegParser::priority = {{'*', 3}, {'+', 3}, {'&', 2}, {'|', 1}};

NFA RegParser::CharNode::get_nfa() {
  NFA out;
  std::string by = (c == '#' ? "" : std::string(1, c));
  out.add_vertex({{by, 0}}, {}, true);
  return out;
}

NFA RegParser::AddNode::get_nfa() {
  NFA out = left->get_nfa();
  out.parallel(right->get_nfa());
  return out;
}

NFA RegParser::ConcatNode::get_nfa() {
  NFA out = left->get_nfa();
  out.consecutive(right->get_nfa());
  return out;
}

NFA RegParser::AsteriskNode::get_nfa() {
  NFA out = operand->get_nfa();
  out.klini_asterisk();
  return out;
}

NFA RegParser::PlusNode::get_nfa() {
  NFA out = operand->get_nfa();
  out.klini_plus();
  return out;
}

std::shared_ptr<RegParser::BaseNode> RegParser::parse(const std::string& regex) {
  if (regex.empty()) throw std::runtime_error("regex cannot be empty");
  std::string with_operations;
  for (size_t i = 1; i < regex.size(); ++i) {
    with_operations.push_back(regex[i - 1]);
    if (regex[i] != '|' && regex[i] != ')' && regex[i] != '*' && regex[i] != '+' && regex[i - 1] != '|' && regex[i - 1] != '(') {
      with_operations.push_back('&');
    }
  }
  with_operations.push_back(regex.back());
  return parse_helper(with_operations);
}

std::shared_ptr<RegParser::BaseNode> RegParser::parse_helper(const std::string& regex) {
  if (regex.empty()) throw std::runtime_error("regex cannot be empty");
  if (regex.size() == 1) {
    if (regex[0] == '+' || regex[0] == '&' || regex[0] == '*' || regex[0] == '(' || regex[0] == ')' || regex[0] == '|') {
      throw std::runtime_error("malformed regexp");
    }
    return std::make_shared<CharNode>(regex[0]);
  }

  int balance = 0;
  int last_operation_position = -1;
  for (size_t i = 0; i < regex.size(); ++i) {
    if (regex[i] == '(') ++balance;
    if (regex[i] == ')') --balance;
    if (balance < 0) throw std::runtime_error("malformed regexp: wrong brace sequence");
    if (regex[i] == '|' || regex[i] == '+' || regex[i] == '*' || regex[i] == '&') {
      if (balance == 0) {
        if (last_operation_position == -1) last_operation_position = i;
        else if (priority.at(regex[i]) <= priority.at(regex[last_operation_position])) last_operation_position = i;
      }
    }
  }

  if (balance != 0) throw std::runtime_error("malformed regexp: wrong brace sequence");
  if (last_operation_position == -1) {
    if (regex[0] == '(' && regex.back() == ')') {
      return parse_helper(regex.substr(1, regex.size() - 2));
    }
    throw std::runtime_error("malformed regexp");
  }

  if (regex[last_operation_position] == '|' || regex[last_operation_position] == '&') {
    std::string op1 = regex.substr(0, last_operation_position);
    std::string op2 = regex.substr(last_operation_position + 1);
    if (regex[last_operation_position] == '|') return std::make_shared<AddNode>(parse_helper(op1), parse_helper(op2));
    else return std::make_shared<ConcatNode>(parse_helper(op1), parse_helper(op2));
  } else {
    std::string op = regex.substr(0, last_operation_position);
    if (regex[last_operation_position] == '*') return std::make_shared<AsteriskNode>(parse_helper(op));
    else return std::make_shared<PlusNode>(parse_helper(op));
  }
}