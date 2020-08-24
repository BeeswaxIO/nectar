// Test for C++20 placeholders.
#include "nectar/cpp20.h"
#include <memory>
#include <unordered_map>

#include "gtest/gtest.h"

namespace {

class Cpp20Test : public ::testing::Test {
 public:
};

using namespace beeswax::nectar;  // NOLINT

TEST_F(Cpp20Test, Starts) {
  EXPECT_TRUE(starts_with("abc", "abc"));
  EXPECT_TRUE(starts_with("abc", "ab"));
  EXPECT_TRUE(starts_with("abc", "a"));
  EXPECT_TRUE(starts_with("abc", ""));
  EXPECT_TRUE(starts_with("ab", "ab"));
  EXPECT_TRUE(starts_with("ab", "a"));
  EXPECT_TRUE(starts_with("ab", ""));
  EXPECT_TRUE(starts_with("a", "a"));
  EXPECT_TRUE(starts_with("a", ""));
  EXPECT_TRUE(starts_with("", ""));

  EXPECT_FALSE(starts_with("abc", "xbc"));
  EXPECT_FALSE(starts_with("abc", "abx"));
  EXPECT_FALSE(starts_with("abc", "axc"));
  EXPECT_FALSE(starts_with("abc", "abcd"));
  EXPECT_FALSE(starts_with("abc", "bc"));
  EXPECT_FALSE(starts_with("abc", "c"));
  EXPECT_FALSE(starts_with("", "abc"));
  EXPECT_FALSE(starts_with("", "a"));
}

TEST_F(Cpp20Test, Ends) {
  EXPECT_TRUE(ends_with("abc", "abc"));
  EXPECT_TRUE(ends_with("abc", "bc"));
  EXPECT_TRUE(ends_with("abc", "c"));
  EXPECT_TRUE(ends_with("abc", ""));
  EXPECT_TRUE(ends_with("ab", "ab"));
  EXPECT_TRUE(ends_with("ab", "b"));
  EXPECT_TRUE(ends_with("ab", ""));
  EXPECT_TRUE(ends_with("a", "a"));
  EXPECT_TRUE(ends_with("a", ""));
  EXPECT_TRUE(ends_with("", ""));

  EXPECT_FALSE(ends_with("abc", "xbc"));
  EXPECT_FALSE(ends_with("abc", "abx"));
  EXPECT_FALSE(ends_with("abc", "axc"));
  EXPECT_FALSE(ends_with("abc", "abcd"));
  EXPECT_FALSE(ends_with("abc", "ab"));
  EXPECT_FALSE(ends_with("abc", "a"));
  EXPECT_FALSE(ends_with("", "abc"));
  EXPECT_FALSE(ends_with("", "a"));
}

TEST_F(Cpp20Test, VectorEraseIf) {
  std::vector<std::string> v = {"abc", "defgh", "ijk", "lmno", "pqrstuv"};
  EXPECT_EQ(v.size(), 5U);
  EXPECT_EQ(erase_if(v, [](auto& e) { return e.size() < 4; }), 2);
  EXPECT_EQ(v.size(), 3);
}

}  // namespace
