// Gap-fillers for missing C++20 features. To be removed once C++20 is
// available.
#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace beeswax::nectar {

// Placeholder for C++20 std version.
//
// See:
// https://en.cppreference.com/mwiki/index.php?title=Special%3ASearch&search=contains
//
// Returns whether container contains element.
//
// Use only when c.contains() is not available. If it's a map and you plan to
// insert if not found, use MapKey, instead.
template <typename C, typename K>
bool contains(C& c, const K& k) {
  return c.find(k) != c.end();
}

// Placeholder for C++20 std version.
//
// See: https://en.cppreference.com/w/cpp/container/map/erase_if
//
// Usage:
//    nectar::erase_if(map, [&](auto& elm) {
//      return !base::contains(ids, elm.first); });
template <class Key, class T, class Compare, class Alloc, class Pred>
void erase_if(std::map<Key, T, Compare, Alloc>& c, Pred pred) {
  for (auto i = c.begin(), last = c.end(); i != last;) {
    if (pred(*i)) {
      i = c.erase(i);
    } else {
      ++i;
    }
  }
}

// Placeholder for C++20 std version.
//
// See: https://en.cppreference.com/w/cpp/container/vector/erase_if
//
// Usage:
//    nectar::erase_if(v, [&](auto& elm) {
//      return !nectar::contains(ids, elm); });
template <class T, class Alloc, class Pred>
constexpr typename std::vector<T, Alloc>::size_type erase_if(
    std::vector<T, Alloc>& c, Pred pred) {
  auto it = std::remove_if(c.begin(), c.end(), pred);
  auto r = std::distance(it, c.end());
  c.erase(it, c.end());
  return r;
}

constexpr bool starts_with(std::string_view whole, std::string_view part) {
  return whole.compare(0, part.size(), part) == 0;
}

constexpr bool ends_with(std::string_view whole, std::string_view part) {
  if (whole.size() < part.size()) return false;
  return whole.compare(whole.size() - part.size(), part.size(), part) == 0;
}

}  // namespace beeswax::nectar
