#pragma once

#include "automata/nfa.hpp"
#include <memory>
#include <unordered_map>

class RegParser {
  struct BaseNode {
    virtual NFA get_nfa() {return NFA();};
  };

  struct CharNode : BaseNode {
    char c;
    CharNode(char c_) : c(c_) {}
    NFA get_nfa() override;
  };

  struct AddNode : BaseNode {
    std::shared_ptr<BaseNode> left, right;
    AddNode(std::shared_ptr<BaseNode> left_, std::shared_ptr<BaseNode> right_) : left(left_), right(right_) {}
    NFA get_nfa() override;
  };

  struct ConcatNode : BaseNode {
    std::shared_ptr<BaseNode> left, right;
    ConcatNode(std::shared_ptr<BaseNode> left_, std::shared_ptr<BaseNode> right_) : left(left_), right(right_) {}
    NFA get_nfa() override;
  };

  struct AsteriskNode : BaseNode {
    std::shared_ptr<BaseNode> operand;
    AsteriskNode(std::shared_ptr<BaseNode> operand_) : operand(operand_) {}
    NFA get_nfa() override;
  };

  struct PlusNode : BaseNode {
    std::shared_ptr<BaseNode> operand;
    PlusNode(std::shared_ptr<BaseNode> operand_) : operand(operand_) {}
    NFA get_nfa() override;
  };

  static std::shared_ptr<BaseNode> parse_helper(const std::string& regexp);
  static const std::unordered_map<char, size_t> priority;
 public:
  static std::shared_ptr<BaseNode> parse(const std::string& regexp);
};