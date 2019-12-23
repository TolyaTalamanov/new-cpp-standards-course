#include <string>
#include <vector>
#include <random>
#include <algorithm>

#include <gtest/gtest.h>

#include "string_fragmentator.hpp"

// FIXME Add more tests, and avoid code dublicate (Define TestFixture)
TEST(StringFragmentator, SplitOneWithEvenSize)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST" };

    // FIXME extra brackets ??? C++14 fixed ?
    std::array<std::array<int, 4>, 4> answers  = {{ {0, 0, 4, 1}, {0, 1, 4, 1}, {0, 2, 4, 1}, {0, 3, 4, 1} }};
    std::array<std::string, 4>        data_ans = { "T", "E", "S", "T" };

    auto fragments = sf.Split(strs, 4);

    for (int i = 0; i < fragments.size(); ++i)
    {
        EXPECT_EQ(answers[i][0], fragments[i].id());
        EXPECT_EQ(answers[i][1], fragments[i].fid());
        EXPECT_EQ(answers[i][2], fragments[i].total());
        EXPECT_EQ(answers[i][3], fragments[i].size());
        EXPECT_EQ(data_ans[i].data(), fragments[i].data());
    }
}

TEST(StringFragmentator, SplitOneWithOddSize)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TESTTE" };

    std::array<std::array<int, 4>, 4> answers  = {{ {0, 0, 4, 1}, {0, 1, 4, 1}, {0, 2, 4, 1}, {0, 3, 4, 3} }};
    std::array<std::string, 4>        data_ans = { "T", "E", "S", "TTE" };

    auto fragments = sf.Split(strs, 4);

    for (int i = 0; i < fragments.size(); ++i)
    {
        EXPECT_EQ(answers[i][0], fragments[i].id());
        EXPECT_EQ(answers[i][1], fragments[i].fid());
        EXPECT_EQ(answers[i][2], fragments[i].total());
        EXPECT_EQ(answers[i][3], fragments[i].size());
        EXPECT_EQ(data_ans[i].data(), fragments[i].data());
    }
}

TEST(StringFragmentator, FragmentsMoreThanSymbols)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST" };
    size_t num_fragments = 10;

    EXPECT_ANY_THROW(sf.Split(strs, num_fragments));
}

TEST(StringFragmentator, NumFragmentsMoreThanZero)
{
    StringFragmentator sf;
    std::vector<std::string> strs;
    size_t num_fragments = 0;

    EXPECT_ANY_THROW(sf.Split(strs, num_fragments));
}

TEST(StringFragmentator, ConcatOneString)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST" };
    auto fragments = sf.Split(strs, 4);

    sf.AddFragments(fragments);

    const auto& str_map = sf.GetStringMap();

    EXPECT_EQ(1u, str_map.size());
    EXPECT_EQ("TEST", str_map.at(0));
}

TEST(StringFragmentator, ConcatMoreOneStrings)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST", "BAZ", "FOO", "QUUX", "CORGE" };
    auto fragments = sf.Split(strs, 3);

    sf.AddFragments(fragments);

    const auto& str_map = sf.GetStringMap();

    EXPECT_EQ(5u, str_map.size());
    for (int i = 0; i < strs.size(); ++i) {
        EXPECT_EQ(strs[i], str_map.at(i));
    }
}

TEST(StringFragmentator, UnorderedFragments)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST", "BAZ", "FOO", "QUUX", "CORGE" };
    auto fragments = sf.Split(strs, 3);

    std::mt19937 g{std::random_device()()};
    std::shuffle(fragments.begin(), fragments.end(), g);

    sf.AddFragments(fragments);

    const auto& str_map = sf.GetStringMap();

    EXPECT_EQ(5u, str_map.size());
    for (int i = 0; i < strs.size(); ++i) {
        EXPECT_EQ(strs[i], str_map.at(i));
    }
}

TEST(StringFragmentator, LastStringNotCompiled)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST", "BAZ", "FOO", "QUUX", "CORGE" };
    auto fragments = sf.Split(strs, 3);

    // Last string will not be collected
    fragments.pop_back();

    sf.AddFragments(fragments);

    const auto& str_map = sf.GetStringMap();

    EXPECT_EQ(4u, str_map.size());
    for (int i = 0; i < strs.size() - 1; ++i) {
        EXPECT_EQ(strs[i], str_map.at(i));
    }

    // String with id 4 not found
    EXPECT_ANY_THROW(str_map.at(4));
}

TEST(StringFragmentator, MiddleStringNotCompiled)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST", "BAZ", "FOO", "QUUX", "CORGE" };
    auto fragments = sf.Split(strs, 3);
    // BAZ will no be collected
    fragments.erase(fragments.begin() + 3);

    sf.AddFragments(fragments);
    const auto& str_map = sf.GetStringMap();

    EXPECT_EQ(4, str_map.size());
    EXPECT_ANY_THROW(str_map.at(1));
}

TEST(StringFragmentator, FragmentIsMissingButAddedLater)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST" };
    auto fragments = sf.Split(strs, 4);
    auto last_fragment = std::move(fragments.back());
    fragments.pop_back();

    sf.AddFragments(fragments);
    const auto& str_map = sf.GetStringMap();

    EXPECT_TRUE(str_map.empty());

    sf.AddFragment(last_fragment);
    EXPECT_FALSE(str_map.empty());
}

TEST(StringFragmentator, FragmentIsMissing)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST" };
    auto fragments = sf.Split(strs, 4);
    fragments.pop_back();

    sf.AddFragments(fragments);

    const auto& str_map = sf.GetStringMap();

    EXPECT_TRUE(str_map.empty());
}

TEST(StringFragmentator, MoveFragments)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST" };
    auto fragments = sf.Split(strs, 4);

    sf.AddFragments(std::move(fragments));
    const auto& str_map = sf.GetStringMap();

    EXPECT_EQ(1u, str_map.size());
    EXPECT_EQ("TEST", str_map.at(0));
}
