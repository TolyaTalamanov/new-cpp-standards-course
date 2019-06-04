#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "string_fragmentator.hpp"

TEST(Fragment, CreateDefault) {
    EXPECT_NO_THROW(Fragment());
}

TEST(Fragment, CreateInit) {
    Fragment f(0, 1, 2, 4, "TEST");

    EXPECT_EQ(0, f.id());
    EXPECT_EQ(1, f.fid());
    EXPECT_EQ(2, f.total());
    EXPECT_EQ(4, f.size());
    EXPECT_EQ(0, strncmp("TEST", f.data(), f.size()));
}

TEST(Fragment, CreateCopy) {
    Fragment f1(0, 1, 2, 4, "TEST");
    Fragment f2(f1);

    EXPECT_EQ(f1.id()   , f2.id());
    EXPECT_EQ(f1.fid()  , f2.fid());
    EXPECT_EQ(f1.total(), f2.total());
    EXPECT_EQ(f1.size() , f2.size());
    EXPECT_EQ(0, strncmp(f1.data(), f2.data(), f1.size()));
}

TEST(Fragment, CreateMove) {
    Fragment f1(0, 1, 2, 4, "TEST");
    Fragment f2(std::move(f1));

    EXPECT_EQ(0, f2.id());
    EXPECT_EQ(1, f2.fid());
    EXPECT_EQ(2, f2.total());
    EXPECT_EQ(4, f2.size());
    EXPECT_EQ(0, strncmp("TEST", f2.data(), f1.size()));
    EXPECT_EQ(nullptr, f1.data());
}

TEST(Fragment, CopyAssign) {
    Fragment f1(0, 1, 2, 5, "CORGE");
    Fragment f2(1, 2, 3, 4, "QUUX ");

    f2 = f1;

    EXPECT_EQ(f1.id()   , f2.id());
    EXPECT_EQ(f1.fid()  , f2.fid());
    EXPECT_EQ(f1.total(), f2.total());
    EXPECT_EQ(f1.size() , f2.size());
    EXPECT_EQ(0, strncmp(f1.data(), f2.data(), f1.size()));
}

TEST(Fragment, SelfCopyAssign) {
    Fragment f(0, 1, 2, 4, "TEST");

    f = f;

    EXPECT_EQ(0, f.id());
    EXPECT_EQ(1, f.fid());
    EXPECT_EQ(2, f.total());
    EXPECT_EQ(4, f.size());
    EXPECT_EQ(0, strncmp("TEST", f.data(), f.size()));
}

TEST(Fragment, SelfMoveAssign) {
    Fragment f(0, 1, 2, 4, "TEST");

    f = std::move(f);

    EXPECT_EQ(0, f.id());
    EXPECT_EQ(1, f.fid());
    EXPECT_EQ(2, f.total());
    EXPECT_EQ(4, f.size());
    EXPECT_EQ(0, strncmp("TEST", f.data(), f.size()));
}

TEST(Fragment, CorrectDestroy) {
    auto* f = new Fragment(0, 1, 2, 4, "TEST");

    EXPECT_NO_THROW(delete f);
}

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
        EXPECT_EQ(0, strcmp(data_ans[i].data(), fragments[i].data()));
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
        EXPECT_EQ(0, strcmp(data_ans[i].data(), fragments[i].data()));
    }
}

TEST(StringFragmentator, FragmentsMoreThanSymbols)
{
    StringFragmentator sf;
    std::vector<std::string> strs = { "TEST" };
    size_t num_fragments = 10;

    EXPECT_ANY_THROW(sf.Split(strs, num_fragments));
}

TEST(StringFragmentator, EmptyStringSet)
{
    StringFragmentator sf;
    std::vector<std::string> strs;

    EXPECT_ANY_THROW(sf.Split(strs, 4));
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
