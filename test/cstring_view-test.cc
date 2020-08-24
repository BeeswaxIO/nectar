// Test for cstring_view.
#include <list>
#include <memory>
#include <set>
#include <unordered_map>
#include "nectar/cstring_view.h"

#include "gtest/gtest.h"

using namespace std::literals;  // NOLINT.

namespace {

using namespace beeswax::nectar;  // NOLINT

size_t GetStringViewSize(std::string_view sv) { return sv.size(); }

size_t IReallyNeedTermination(cstring_view sz) { return sz.size(); }

TEST(CstringviewTest, Overall) {
  auto sz = "abc"_sz;
  EXPECT_EQ(sz, "abc");
  EXPECT_EQ(sz[sz.size()], '\0');

  // Can assign from string.
  std::string s;
  s = "abc";
  sz = s;
  EXPECT_EQ(sz.size(), s.size());
  EXPECT_EQ(sz.size(), GetStringViewSize(sz));

  // Can construct from string.
  {
    cstring_view v{s};
    EXPECT_EQ(v, s);
  }

  // Unterminated aborts.
  EXPECT_THROW(sz = cstring_view(s.c_str(), 1), std::logic_error);

  // Is a std::string_view, so compares and assigns.
  auto sv = "abc"sv;
  sz = "abc";
  EXPECT_EQ(sv, sz);
  sz = "def";
  EXPECT_NE(sv, sz);

  IReallyNeedTermination(sz);
  // Next line would not compile, because not all string_view instances are
  // cstring_view instances. IReallyNeedTermination("Terminated?"sv);

  // Next lines would not compile, as the methods are explicitly deleted.
  // sz.remove_suffix(1);
  // sz = sz.substr(5);
  // sz.swap(sz);
}

TEST(CstringviewTest, Inherited) {
  // Confirms that non-mutating base methods still work.
  auto sz = "zabc"_sz;
  sz.remove_prefix(1);
  EXPECT_EQ(*sz.begin(), 'a');
  EXPECT_EQ(*sz.end(), '\0');
  EXPECT_EQ(*sz.cbegin(), 'a');
  EXPECT_EQ(*sz.cend(), '\0');
  EXPECT_EQ(*sz.rbegin(), 'c');
  EXPECT_EQ(*sz.rend(), 'z');
  EXPECT_EQ(*sz.crbegin(), 'c');
  EXPECT_EQ(*sz.crend(), 'z');
  EXPECT_EQ(sz[0], 'a');
  EXPECT_EQ(sz.at(0), 'a');
  EXPECT_EQ(sz.back(), 'c');
  EXPECT_EQ(*sz.data(), 'a');
  EXPECT_EQ(*sz.c_str(), 'a');
  cstring_view::pointer p = sz.data();
  EXPECT_NE(p, nullptr);
  p = cstring_view().data();
  EXPECT_EQ(p, nullptr);
  EXPECT_NE(sz.max_size(), 0);
  EXPECT_TRUE(sz.starts_with("ab"));
  EXPECT_TRUE(sz.ends_with("bc"));
  char buf[] = "zz";
  sz.copy(buf, 1, 1);
  EXPECT_EQ(buf[0], 'b');
  EXPECT_EQ(buf[1], 'z');
  int t{};
  for (auto c : sz) t += c;
  EXPECT_EQ(t, 294);
  EXPECT_EQ(sz.compare("abc"), 0);
  EXPECT_EQ(sz.find('f'), std::string_view::npos);
  std::set<cstring_view> all{"abc"_sz, "def"_sz};
}

}  // namespace