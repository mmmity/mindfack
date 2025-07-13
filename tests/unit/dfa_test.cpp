#include "automata/dfa.hpp"
#include <gtest/gtest.h>
#include <fstream>

class DfaTest : public testing::Test {
  void SetUp() override {
    std::ofstream nfa_file("aut_file.nfa");
    nfa_file << "5 9\n";
    nfa_file
        << "0 1 0\n 0 3 0\n1 2 1\n2 3 1\n3 4 1\n4 2 0\n4 0 #\n0 1 1\n2 4 0\n";
    nfa_file << "4\n";
    nfa_file.close();

    std::ofstream dfa_file_1("aut_file_1.dfa");
    dfa_file_1 << "3 6\n";
    dfa_file_1 << "0 0 0\n0 1 1\n1 2 0\n1 0 1\n2 1 0\n2 2 1\n";
    dfa_file_1 << "0\n";
    dfa_file_1.close();

    std::ofstream dfa_file_2("aut_file_2.dfa");
    dfa_file_2 << "5 8\n";
    dfa_file_2 << "0 1 0\n0 2 1\n1 3 1\n2 4 0\n3 1 0\n3 2 1\n4 1 0\n4 2 1\n";
    dfa_file_2 << "0 1 3 4\n";
    dfa_file_2.close();

    std::ofstream dfa_file_3("aut_file_3.dfa");
    dfa_file_3 << "3 4\n";
    dfa_file_3 << "0 1 0\n0 2 1\n1 0 1\n2 0 0\n";
    dfa_file_3 << "0 1\n";
    dfa_file_3.close();

    std::ofstream dfa_file_4("aut_file_4.dfa");
    dfa_file_4 << "9 18\n";
    dfa_file_4
        << "0 1 0\n0 2 1\n1 1 0\n1 3 1\n2 1 0\n2 2 1\n3 1 0\n3 4 1\n4 5 0\n4 6 "
           "1\n5 5 0\n5 7 1\n6 5 0\n6 6 1\n7 5 0\n7 8 1\n8 5 0\n8 6 1\n";
    dfa_file_4 << "4 5 6 7 8\n";
    dfa_file_4.close();

    std::ofstream dfa_file_5("aut_file_5.dfa");
    dfa_file_5 << "4 8\n";
    dfa_file_5 << "0 1 0\n0 0 1\n1 1 0\n1 2 1\n2 1 0\n2 3 1\n3 3 0\n3 3 1\n";
    dfa_file_5 << "3\n";
    dfa_file_5.close();
  }

  void TearDown() override {
    remove("aut_file.nfa");
    remove("aut_file_1.dfa");
    remove("aut_file_2.dfa");
    remove("aut_file_3.dfa");
    remove("aut_file_4.dfa");
    remove("aut_file_5.dfa");
  }
};

TEST_F(DfaTest, BasicTests) {
  std::vector<std::string> some_strings = {
      "10",       "00",     "01",     "11",        "0",    "1",    "",
      "101",      "1101",   "000001", "000101101", "1001", "0111", "0011",
      "01001001", "010010", "01010",  "110010",    "011"};
  NFA nfa("aut_file.nfa");
  DFA dfa(nfa);
  for (const std::string& s : some_strings) {
    ASSERT_EQ(nfa.allows(s), dfa.allows(s));
  }
  ASSERT_FALSE(dfa.is_full());
  dfa.make_full();
  ASSERT_TRUE(dfa.is_full());
  ASSERT_EQ(dfa.edges(), 2 * dfa.size());
  size_t sz = dfa.size();
  dfa.minimize();
  ASSERT_TRUE(sz >= dfa.size());
  for (const std::string& s : some_strings) {
    ASSERT_EQ(nfa.allows(s), dfa.allows(s));
  }
  dfa.make_complement();
  for (const std::string& s : some_strings) {
    ASSERT_NE(nfa.allows(s), dfa.allows(s));
  }
}

TEST_F(DfaTest, TestMinimize) {
  DFA dfa1("aut_file_2.dfa");
  DFA dfa2("aut_file_3.dfa");
  std::vector<std::string> some_strings = {
      "10",       "00",     "01",     "11",        "0",    "1",    "",
      "101",      "1101",   "000001", "000101101", "1001", "0111", "0011",
      "01001001", "010010", "01010",  "110010",    "011"};
  for (const std::string& s : some_strings) {
    ASSERT_EQ(dfa1.allows(s), dfa2.allows(s));
  }

  dfa1.minimize();
  dfa2.make_full();
  ASSERT_EQ(dfa1.size(), dfa2.size());
  ASSERT_EQ(dfa1.edges(), dfa2.edges());
  for (const std::string& s : some_strings) {
    ASSERT_EQ(dfa1.allows(s), dfa2.allows(s));
  }

  DFA dfa3("aut_file_4.dfa");
  DFA dfa4("aut_file_5.dfa");
  for (const std::string& s : some_strings) {
    ASSERT_EQ(dfa3.allows(s), dfa4.allows(s));
  }

  dfa3.minimize();
  ASSERT_EQ(dfa3.size(), dfa4.size());
  ASSERT_EQ(dfa3.edges(), dfa4.edges());
  for (const std::string& s : some_strings) {
    ASSERT_EQ(dfa3.allows(s), dfa4.allows(s));
  }
}