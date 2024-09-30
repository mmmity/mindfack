#include "automata/nfa.hpp"
#include <gtest/gtest.h>
#include <fstream>

class NfaTest : public testing::Test {

  void SetUp() override {
    std::ofstream aut_file_1("aut_file_1.nfa");
    aut_file_1 << "2 2\n";
    aut_file_1 << "0 0 a\n0 1 b\n";
    aut_file_1 << "1";
    aut_file_1.close();

    std::ofstream aut_file_2("aut_file_2.nfa");
    aut_file_2 << "3 4\n";
    aut_file_2 << "0 1 ab\n0 2 ba\n1 2 bb\n2 1 aa\n";
    aut_file_2 << "1 2 \n";
    aut_file_2.close();

    std::ofstream aut_file_3("aut_file_3.nfa");
    aut_file_3 << "3 6\n";
    aut_file_3 << "0 0 a\n0 1 b\n1 0 b\n1 2 a\n2 1 a\n2 2 b\n";
    aut_file_3 << "0\n";
    aut_file_3.close();

    std::ofstream aut_file_4("aut_file_4.nfa");
    aut_file_4 << "3 4\n";
    aut_file_4 << "0 1 a\n1 2 b\n2 0 #\n0 2 #\n";
    aut_file_4 << "2\n";
    aut_file_4.close();

    std::ofstream aut_file_5("aut_file_5.nfa");
    aut_file_5 << "5 9\n";
    aut_file_5
        << "0 1 a\n 0 3 a\n1 2 b\n2 3 b\n3 4 b\n4 2 a\n4 0 #\n0 1 b\n2 4 a\n";
    aut_file_5 << "4\n";
    aut_file_5.close();
  }

  void TearDown() override {
    remove("aut_file_1.nfa");
    remove("aut_file_2.nfa");
    remove("aut_file_3.nfa");
    remove("aut_file_4.nfa");
    remove("aut_file_5.nfa");
  }
};

TEST_F(NfaTest, BasicTests) {
  NFA nfa("aut_file_1.nfa");
  ASSERT_FALSE(nfa.allows("bab"));
  ASSERT_FALSE(nfa.allows("baaa"));
  nfa.add_vertex({{"b", 0}, {"a", 2}}, {{"a", 2}, {"b", 1}}, true);
  ASSERT_TRUE(nfa.allows("bab"));
  ASSERT_TRUE(nfa.allows("baaab"));
  ASSERT_TRUE(nfa.allows("baaa"));

  ASSERT_FALSE(nfa.allows("ababab"));
  nfa.add_vertex({{"ab", 0}, {"ab", 3}}, {{"", 1}, {"ab", 3}}, false);
  ASSERT_TRUE(nfa.allows("ababab"));
  ASSERT_FALSE(nfa.allows("bbb"));
  ASSERT_FALSE(nfa.allows("abba"));

  ASSERT_EQ(nfa.size(), 4);
  ASSERT_TRUE(nfa.has_long_edges());
  nfa.expand_edges();
  ASSERT_FALSE(nfa.has_long_edges());
  ASSERT_EQ(nfa.size(), 6);
  ASSERT_TRUE(nfa.allows("bab"));
  ASSERT_TRUE(nfa.allows("baaab"));
  ASSERT_TRUE(nfa.allows("baaa"));
  ASSERT_TRUE(nfa.allows("ababab"));
  ASSERT_FALSE(nfa.allows("bbb"));
  ASSERT_FALSE(nfa.allows("abba"));

  ASSERT_TRUE(nfa.has_empty_edges());
  nfa.remove_empty_edges();
  ASSERT_FALSE(nfa.has_empty_edges());
  ASSERT_TRUE(nfa.allows("bab"));
  ASSERT_TRUE(nfa.allows("baaab"));
  ASSERT_TRUE(nfa.allows("baaa"));
  ASSERT_TRUE(nfa.allows("ababab"));
  ASSERT_FALSE(nfa.allows("bbb"));
  ASSERT_FALSE(nfa.allows("abba"));
}

TEST_F(NfaTest, TestAllows) {
  NFA nfa1("aut_file_1.nfa");
  NFA nfa2("aut_file_2.nfa");
  NFA nfa3("aut_file_3.nfa");
  NFA nfa4("aut_file_4.nfa");
  NFA nfa5("aut_file_5.nfa");

  std::string w1("aaaab");
  std::string w2("ab");
  std::string w3("baaabb");
  std::string w4("baaaa");
  std::string w5("");
  std::string w6("bba");
  std::string w7("abab");

  ASSERT_TRUE(nfa1.allows(w1));
  ASSERT_TRUE(nfa1.allows(w2));
  ASSERT_FALSE(nfa1.allows(w3));
  ASSERT_FALSE(nfa1.allows(w4));
  ASSERT_FALSE(nfa1.allows(w5));
  ASSERT_FALSE(nfa1.allows(w6));
  ASSERT_FALSE(nfa1.allows(w7));

  ASSERT_FALSE(nfa2.allows(w1));
  ASSERT_TRUE(nfa2.allows(w2));
  ASSERT_TRUE(nfa2.allows(w3));
  ASSERT_FALSE(nfa2.allows(w4));
  ASSERT_FALSE(nfa2.allows(w5));
  ASSERT_FALSE(nfa2.allows(w6));
  ASSERT_FALSE(nfa2.allows(w7));

  ASSERT_FALSE(nfa3.allows(w1));
  ASSERT_FALSE(nfa3.allows(w2));
  ASSERT_FALSE(nfa3.allows(w3));
  ASSERT_FALSE(nfa3.allows(w4));
  ASSERT_TRUE(nfa3.allows(w5));
  ASSERT_TRUE(nfa3.allows(w6));
  ASSERT_FALSE(nfa3.allows(w7));

  ASSERT_FALSE(nfa4.allows(w1));
  ASSERT_TRUE(nfa4.allows(w2));
  ASSERT_FALSE(nfa4.allows(w3));
  ASSERT_FALSE(nfa4.allows(w4));
  ASSERT_TRUE(nfa4.allows(w5));
  ASSERT_FALSE(nfa4.allows(w6));
  ASSERT_TRUE(nfa4.allows(w7));

  ASSERT_FALSE(nfa5.allows(w1));
  ASSERT_TRUE(nfa5.allows(w2));
  ASSERT_FALSE(nfa5.allows(w3));
  ASSERT_FALSE(nfa5.allows(w4));
  ASSERT_FALSE(nfa5.allows(w5));
  ASSERT_TRUE(nfa5.allows(w6));
  ASSERT_TRUE(nfa5.allows(w7));
}

TEST_F(NfaTest, TestEdgeExpansion) {
  NFA nfa;
  nfa.add_vertex({{"abcde", 0}}, {}, true);
  ASSERT_EQ(2, nfa.size());
  ASSERT_TRUE(nfa.has_long_edges());
  nfa.expand_edges();
  ASSERT_EQ(6, nfa.size());
  ASSERT_FALSE(nfa.has_long_edges());
  nfa.add_vertex({{"abcde", 3}}, {}, false);
  ASSERT_TRUE(nfa.has_long_edges());
  nfa.expand_edges();
  ASSERT_EQ(11, nfa.size());
  nfa.add_vertex({{"", 5}, {"a", 7}}, {{"ac", 9}}, false);
  nfa.expand_edges();
  ASSERT_EQ(13, nfa.size());
}

TEST_F(NfaTest, TestEmptyEdgeRemoval) {
  NFA nfa;
  nfa.add_vertex({{"", 0}}, {}, true);
  ASSERT_TRUE(nfa.allows(""));
  ASSERT_EQ(1, nfa.edges());
  nfa.remove_empty_edges();
  ASSERT_TRUE(nfa.allows(""));
  ASSERT_EQ(0, nfa.edges());

  NFA nfa1("aut_file_4.nfa");
  ASSERT_EQ(nfa1.edges(), 4);
  ASSERT_TRUE(nfa1.allows("") && nfa1.allows("abababab"));
  nfa1.remove_empty_edges();
  ASSERT_EQ(nfa1.edges(), 3);
  ASSERT_TRUE(nfa1.allows("") && nfa1.allows("abababab"));
}