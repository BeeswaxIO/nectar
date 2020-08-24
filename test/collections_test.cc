#include "nectar/collections.h"
#include <memory>
#include <unordered_map>

#include "gtest/gtest.h"

namespace {

using std::literals::operator""sv;
using namespace beeswax::nectar;

class CollectionsTest : public ::testing::Test {
 public:
  StringMap<int> dict{{"abc", 1}, {"def", 2}};
};

TEST_F(CollectionsTest, StringLookup) {
  static const std::string k1{"abc"};
  static const std::string k2{"def"};
  static const std::string kb{"bbb"};

  auto it = dict.find(k1);
  ASSERT_NE(it, dict.end());
  ASSERT_EQ(it->second, 1);
  it = dict.find(k2);
  ASSERT_NE(it, dict.end());
  ASSERT_EQ(it->second, 2);
  it = dict.find(kb);
  ASSERT_EQ(it, dict.end());
}

TEST_F(CollectionsTest, StringViewLookup) {
  constexpr auto k1{"abc"sv};
  constexpr auto k2{"def"sv};
  constexpr auto kb{"bbb"sv};

  auto it = dict.find(k1);
  ASSERT_NE(it, dict.end());
  ASSERT_EQ(it->second, 1);
  it = dict.find(k2);
  ASSERT_NE(it, dict.end());
  ASSERT_EQ(it->second, 2);
  it = dict.find(kb);
  ASSERT_EQ(it, dict.end());
}

TEST_F(CollectionsTest, CStyleLookup) {
  constexpr char k1[] = "abc";
  constexpr char k2[] = "def";
  constexpr char kb[] = "bbb";

  auto it = dict.find(k1);
  ASSERT_NE(it, dict.end());
  ASSERT_EQ(it->second, 1);
  it = dict.find(k2);
  ASSERT_NE(it, dict.end());
  ASSERT_EQ(it->second, 2);
  it = dict.find(kb);
  ASSERT_EQ(it, dict.end());
}

TEST_F(CollectionsTest, FindPtrTest) {
  auto v = FindPtr(dict, "abc");
  ASSERT_NE(v, nullptr);
  ASSERT_EQ(*v, 1);
  *v = 2;
  v = FindPtr(dict, "abc");
  ASSERT_NE(v, nullptr);
  ASSERT_EQ(*v, 2);
  *v = 1;
  const auto& kdict = dict;
  auto vv = FindPtr(kdict, "abc");
  ASSERT_NE(vv, nullptr);
  ASSERT_EQ(*vv, 1);
  //*vv = 2; // Const safety prevents.

  // Ease of use:
  bool found;
  constexpr auto k1{"abc"sv};
  constexpr auto kb{"bbb"sv};

  found = false;
  if (auto v = FindPtr(dict, k1)) found = true;
  ASSERT_TRUE(found);

  found = false;
  if (auto v = FindPtr(dict, kb)) found = true;
  ASSERT_FALSE(found);

  std::unordered_map<std::string, int> words{{"abc", 1}};
  found = false;
  if (auto v = FindPtr(words, std::string(k1))) found = true;
  ASSERT_TRUE(found);
}

enum TargetType {
  AIRPORT,
  AUTONOMOUS_COMMUNITY,
  BOROUGH,
  CANTON,
  CITY,
  UNKNOWN
};

const std::string& GetEmpty() {
  static const std::string kEmpty;
  return kEmpty;
}

const std::string& GetZoo() {
  static const std::string kZoo = "zoo";
  return kZoo;
}

const std::string& GetKilled() {
  throw 0;
  return GetEmpty();
}

TEST_F(CollectionsTest, DerefOrDefaultTest) {
  const std::string e = GetEmpty();
  const std::string s = "foo";
  auto ps = &s;
  ASSERT_EQ(*ps, DerefOrDefault(ps));

  ps = nullptr;
  ASSERT_EQ(DerefOrDefault(ps), std::string());
  ASSERT_EQ(DerefOrDefault(ps, s), s);

  std::string z;
  ASSERT_EQ(z, std::string());
  const std::string zz;
  ASSERT_EQ(zz, std::string());

  std::string t = "moo";
  auto pt = &t;

  auto& r = DerefOrDefault(pt, z);
  ASSERT_EQ(r, t);

#if 0
  // Doesn't compile: Return value is by value, so it cannot bind to non-const ref.
  auto& rr = DerefOrDefault(pt);

  // Doesn't compile: Default is const but pointer is non-const.
  auto& rz = DerefOrDefault(pt, zz);
#endif

  ASSERT_EQ(DerefOrDefault(pt, z), t);
  pt = nullptr;
  ASSERT_EQ(DerefOrDefault(pt, z), z);

  const std::string u = "zoo";
  auto pu = &u;
  ASSERT_EQ(*pu, u);
  auto& rr = DerefOrDefault(pu, z);
  ASSERT_EQ(rr, u);
  ASSERT_EQ(DerefOrDefault(pu, z), u);
  pu = nullptr;
  ASSERT_EQ(DerefOrDefault(pu, z), z);

  auto spt = std::make_unique<std::string>(u);
  auto pspt = &spt;
  ASSERT_EQ(*spt, u);
  ASSERT_EQ(**pspt, u);

  ASSERT_EQ(*DerefOrDefault(pspt), u);

  const std::map<std::string, TargetType> kTargetTypeStrMap;
  ASSERT_EQ(DerefOrDefault(FindPtr(kTargetTypeStrMap, u), UNKNOWN), UNKNOWN);

  std::map<int, TargetType> ad_position_map;
  ASSERT_EQ(DerefOrDefault(FindPtr(ad_position_map, 1), UNKNOWN), UNKNOWN);

  std::set<TargetType> empty_target_set;
  std::map<int32_t, std::set<TargetType>> adx_to_openrtb_creative_attribute;
  ASSERT_EQ(DerefOrDefault(FindPtr(adx_to_openrtb_creative_attribute, 1)),
            empty_target_set);

  const std::unordered_map<int, std::string> mis;
  ASSERT_EQ(DerefOrDefault(FindPtr(mis, 0)), std::string());
  ASSERT_EQ(FindOrDefault(mis, 0, u), u);

  std::map<std::string, std::string> mss{{"dog", "cat"}};
  ASSERT_EQ(FindOrDefault(mss, u, u), u);

  ASSERT_EQ(nosniff::FindOrDefaultCb(mss, e, GetZoo), u);
  auto xx1 = nosniff::FindOrDefaultCb(mss, e, [] { return GetZoo(); });
  ASSERT_EQ(xx1, u);

  int zzz1 = 7;
  static_assert(!std::is_invocable_v<std::string>);
  static_assert(!std::is_invocable_v<int>);
  static_assert(std::is_invocable_v<decltype(GetZoo)>);
  auto lam = [] { return GetZoo(); };
  static_assert(std::is_invocable_v<decltype(lam)>);
  auto lam2 = [&zzz1] {
    if (zzz1)
      return GetZoo();
    else
      return GetZoo();
  };
  static_assert(std::is_invocable_v<decltype(lam2)>);

  // Write Value/Cb versions.
  // Stick a pair of unnamed but SFINAEd versions in front
  // Figure out how to distinguish between SetVal and SetCb.

  // ASSERT_EQ(FindOrDefault(mss, e, GetZoo), u);
  // auto xxz1 = FindOrDefault(mss, e, [] { return GetZoo(); });
  // ASSERT_EQ(xxz1, u);

  auto xx2 = nosniff::FindOrDefaultCb(mss, e, [] { return "char*"; });
  ASSERT_EQ(xx2, "char*");

  // Callback is never called.
  ASSERT_EQ(nosniff::FindOrDefaultCb(mss, "dog", GetKilled), "cat");
}

}  // namespace