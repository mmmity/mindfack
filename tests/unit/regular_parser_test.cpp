#include <gtest/gtest.h>

#include "regular/reg_parser.hpp"

TEST(RegParserTest, GoodRegexps) {
  std::string regexp1 = "(a|b)*(b|c)+";
  std::string regexp2 = "(#|a|ab|abc)";
  std::string regexp3 = "(aaaa|ab)";
  std::string regexp4 = "((a|ab)(c|cd)+(e|ef)*)";
  std::string regexp5 = "b*a";

  NFA nfa1 = RegParser::parse(regexp1)->get_nfa();
  NFA nfa2 = RegParser::parse(regexp2)->get_nfa();
  NFA nfa3 = RegParser::parse(regexp3)->get_nfa();
  NFA nfa4 = RegParser::parse(regexp4)->get_nfa();
  NFA nfa5 = RegParser::parse(regexp5)->get_nfa();

  std::vector<std::pair<std::string, bool>> exp1 = {{"", false}, {"b", true}, {"c", true}, {"a", false}, {"ab", true}, {"abc", true}, {"abbab", true}, {"aaaa", false}, {"bbbbbbbbb", true}, {"abaca", false}, {"ababababbcbcbcbc", true}};
  std::vector<std::pair<std::string, bool>> exp2 = {{"", true}, {"b", false}, {"c", false}, {"a", true}, {"ab", true}, {"abc", true}, {"abbab", false}, {"aaaa", false}, {"bbbbbbbbb", false}, {"abaca", false}, {"ababababbcbcbcbc", false}};
  std::vector<std::pair<std::string, bool>> exp3 = {{"", false}, {"b", false}, {"c", false}, {"a", false}, {"ab", true}, {"abc", false}, {"abbab", false}, {"aaaa", true}, {"bbbbbbbbb", false}, {"abaca", false}, {"ababababbcbcbcbc", false}};
  std::vector<std::pair<std::string, bool>> exp4 = {{"", false}, {"b", false}, {"c", false}, {"a", false}, {"ab", false}, {"abc", true}, {"abbab", false}, {"aaaa", false}, {"bbbbbbbbb", false}, {"abaca", false}, {"ababababbcbcbcbc", false}, {"accde", true}, {"abccd", true}, {"abccdeefefe", true}, {"accdeff", false}};
  std::vector<std::pair<std::string, bool>> exp5 = {{"", false}, {"b", false}, {"bbbbbbba", true}, {"bbbbb", false}, {"a", true}, {"bbab", false}};

  for (auto [s, res] : exp1) ASSERT_EQ(res, nfa1.allows(s));
  for (auto [s, res] : exp2) ASSERT_EQ(res, nfa2.allows(s));
  for (auto [s, res] : exp3) ASSERT_EQ(res, nfa3.allows(s));
  for (auto [s, res] : exp4) ASSERT_EQ(res, nfa4.allows(s));
  for (auto [s, res] : exp5) ASSERT_EQ(res, nfa5.allows(s));
}

TEST(RegParserTest, BadRegexps) {
  std::string regexp1 = "";
  std::string regexp2 = "|a";
  std::string regexp3 = "a|";
  std::string regexp4 = "((ab)|bc))";
  std::string regexp5 = "((|a)|b)";
  std::string regexp6 = "((((()))))";
  std::string regexp7 = "(ab|cd)|((ab)**)|(|a)";
  std::string regexp8 = "(*a)";

  ASSERT_THROW(RegParser::parse(regexp1), std::runtime_error);
  ASSERT_THROW(RegParser::parse(regexp2), std::runtime_error);
  ASSERT_THROW(RegParser::parse(regexp3), std::runtime_error);
  ASSERT_THROW(RegParser::parse(regexp4), std::runtime_error);
  ASSERT_THROW(RegParser::parse(regexp5), std::runtime_error);
  ASSERT_THROW(RegParser::parse(regexp6), std::runtime_error);
  ASSERT_THROW(RegParser::parse(regexp7), std::runtime_error);
  ASSERT_THROW(RegParser::parse(regexp8), std::runtime_error);
}