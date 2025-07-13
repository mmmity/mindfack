#include <gtest/gtest.h>

#include "automata/rfa.hpp"
#include "regular/reg_parser.hpp"

TEST(RfaTest, BasicTests) {
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

  RFA rfa1 = nfa1;
  RFA rfa2 = nfa2;
  RFA rfa3 = nfa3;
  RFA rfa4 = nfa4;
  RFA rfa5 = nfa5;

  std::vector<std::string> checking = {"", "b","c", "a", "ab", "abc", "abbab", "aaaa", "bbbbbbbbb", "abaca", "ababababbcbcbcbc", "accde", "abccd", "abccdeefefe", "accdeff"};

  std::string regexp_new1 = rfa1.minimize();
  std::string regexp_new2 = rfa2.minimize();
  std::string regexp_new3 = rfa3.minimize();
  std::string regexp_new4 = rfa4.minimize();
  std::string regexp_new5 = rfa5.minimize();

  NFA nfa1_new = RegParser::parse(regexp_new1)->get_nfa();
  NFA nfa2_new = RegParser::parse(regexp_new2)->get_nfa();
  NFA nfa3_new = RegParser::parse(regexp_new3)->get_nfa();
  NFA nfa4_new = RegParser::parse(regexp_new4)->get_nfa();
  NFA nfa5_new = RegParser::parse(regexp_new5)->get_nfa();

  for (const std::string& s : checking) {
    ASSERT_EQ(nfa1.allows(s), nfa1_new.allows(s));
    ASSERT_EQ(nfa2.allows(s), nfa2_new.allows(s));
    ASSERT_EQ(nfa3.allows(s), nfa3_new.allows(s));
    ASSERT_EQ(nfa4.allows(s), nfa4_new.allows(s));
    ASSERT_EQ(nfa5.allows(s), nfa5_new.allows(s));
  }
}