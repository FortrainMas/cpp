
#include <iostream>
#include <gtest/gtest.h>

#include <regex/nfa.hpp>
#include <regex/parser.hpp>
#include <regex/regex.hpp>



TEST(SimpleRegexTests, MatchPlain) {
    std::string str("accb");
    Parser parser(str);
    Regex regex(str);

    EXPECT_EQ(regex.match(str), true);
}

TEST(SimpleRegexTests, UnmatchSimple) {
    std::string reg("abc");
    Regex regex(reg);
    std::string str("aaasdafsdfewa");

    EXPECT_EQ(regex.match(str), false);
}


TEST(StarRegexTests, Multisymbols) {
    std::string reg("a*cdb");
    Regex regex(reg);
    std::string str("aaaaacdb");

    EXPECT_EQ(regex.match(str), true);
}

TEST(StarRegexTests, OneSymbol) {
    std::string reg("aca*b");
    Regex regex(reg);
    std::string str("acab");

    EXPECT_EQ(regex.match(str), true);
}

TEST(StarRegexTests, SkipSymbol) {
    std::string reg("aca*b");
    Regex regex(reg);
    std::string str("acb");

    EXPECT_EQ(regex.match(str), true);
}

TEST(StarRegexTests, GreedyFail) {
    std::string reg("a*a");
    Regex regex(reg);
    std::string str("aaaa");

    EXPECT_EQ(regex.match(str), false);
}

TEST(PlusRegexTests, Multisymbols) {
    std::string reg("0+rte");
    Regex regex(reg);
    std::string str("0000000rte");

    EXPECT_EQ(regex.match(str), true);
}

TEST(PlusRegexTests, OneSymbol) {
    std::string reg("aca+b");
    Regex regex(reg);
    std::string str("acab");

    EXPECT_EQ(regex.match(str), true);
}

TEST(PlusRegexTests, SkipSymbolFail) {
    std::string reg("aca+b");
    Regex regex(reg);
    std::string str("acb");

    EXPECT_EQ(regex.match(str), false);
}


TEST(PlusRegexTest, GreedyFail) {
    std::string reg("a+a");
    Regex regex(reg);
    std::string str("aa");

    EXPECT_EQ(regex.match(str), false);
}


TEST(QuestionRegexTests, MultisymbolsFail) {
    std::string reg("q?ubk");
    Regex regex(reg);
    std::string str("qqqqqubk");

    EXPECT_EQ(regex.match(str), false);
}

TEST(QuestionRegexTests, OneSymbol) {
    std::string reg("aca?b");
    Regex regex(reg);
    std::string str("acab");

    EXPECT_EQ(regex.match(str), true);
}

TEST(QuestionRegexTests, SkipSymbol) {
    std::string reg("aca?b");
    Regex regex(reg);
    std::string str("acb");

    EXPECT_EQ(regex.match(str), true);
}


TEST(QuestionRegexTest, Ungreedy) {
    std::string reg("a?a");
    Regex regex(reg);
    std::string str("aa");

    EXPECT_EQ(regex.match(str), true);
}

TEST(DotRegexTests, Multisymbols) {
    std::string reg("w..ck");
    Regex regex(reg);
    std::string str("w ack");

    EXPECT_EQ(regex.match(str), true);
}

TEST(DotRegexTests, SkipSymbolFail) {
    std::string reg("p..cd");
    Regex regex(reg);
    std::string str("pacd");

    EXPECT_EQ(regex.match(str), false);
}

TEST(SymbolGroupTests, SimpleGroup) {
    std::string reg("[abc]");
    Regex regex(reg);
    std::string str("a");

    EXPECT_EQ(regex.match(str), true);
}

TEST(SymbolGroupTests, SimpleGroupWithAdditions) {
    std::string reg("sdf[abc]qa");
    Regex regex(reg);
    std::string str("sdfaqa");

    EXPECT_EQ(regex.match(str), true);
}

TEST(SymbolGroupTests, RangeTest) {
    std::string reg("[a-d]");
    Regex regex(reg);
    std::string str("b");

    EXPECT_EQ(regex.match(str), true);
}

TEST(SymbolGroupTests, ManyRangesTest) {
    std::string reg("sdf[a-cdp-sq0]qa");
    Regex regex(reg);
    std::string str("sdfrqa");

    EXPECT_EQ(regex.match(str), true);
}

TEST(ComplexMatchTest, test1) {
    std::string res("q.a+bcd");
    Regex regex(res);
    std::string str("qsabcd");

    EXPECT_EQ(regex.match(str), true);
}

TEST(ComplexMatchTest, test2) {
    std::string res("q.a+bcd[0-9]*");
    Regex regex(res);

    std::string str("qsaaaaabcd");
    EXPECT_EQ(regex.match(str), true);

    str = std::string("qaabcd1000");
    EXPECT_EQ(regex.match(str), true);

    str = std::string("qaaaaaaaabcd240123478");
    EXPECT_EQ(regex.match(str), true);
}

TEST(ComplexMatchTest, test3) {
    std::string res(".*");
    Regex regex(res);

    std::string str("qaabcd1000");
    EXPECT_EQ(regex.match(str), true);

    str = std::string("");
    EXPECT_EQ(regex.match(str), true);
}
