// Collections utilities.
#pragma once

#include <map>
#include <string>
#include <string_view>
#include <type_traits>

namespace beeswax::nectar {

// Transparent std::less<string> that works with anything that can
// be cast into a std::string_view.
//
// Note that, for now, this works for maps and sets, but not their unordered
// versions. This will change in C++20, allowing us to add StringHash to this
// header.
//
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0919r3.html
// https://herbsutter.com/2018/11/13/trip-report-fall-iso-c-standards-meeting-san-diego/
struct TransparentLessString {
  using is_transparent = void;

  template <typename T, typename U>
  bool operator()(const T& l, const U& r) const {
    return static_cast<std::string_view>(l) < static_cast<std::string_view>(r);
  }
};

// A StringMap is a map keyed on std::string, where the value defaults to
// std::string but can be specified.
//
// By using this type, you automatically get transparent lookup, so that the key
// doesn't have to be converted to a std::string temporary.
template <typename V = std::string,
          typename A = std::allocator<std::pair<const std::string, V>>>
using StringMap = std::map<std::string, V, TransparentLessString, A>;

// Internal implementation details; do not use.
namespace details {
// Helper sniffer to get dereferenced type from pointer.
template <typename T>
using deref_t = std::remove_reference_t<decltype(*std::declval<T>())>;

// Helper sniffer for whether the dereferenced type is copyable.
template <typename T>
constexpr bool is_deep_copyable_ptr_v =
    std::is_copy_constructible_v<deref_t<T>>;

// Helper sniffer for pointer to value returned from map-like container.
template <typename C, typename K>
using mapvalptr_t =
    decltype(&(std::declval<C>().find(std::declval<K>())->second));
}  // namespace details

namespace nosniff {
// DO NOT USE THIS.
//
// Returns dereferenced value of pointer, if not nullptr.
// Otherwise, returns specified default.
//
// Note: Only intended for use if DerefOrDefault fails to compile.
//
// Usage:
//    // Don't use this. Use DerefOrDefault.
//    auto v = base::nosniff::DerefOrDefaultVal(GetPtr(), kDefaultStr);
//
// Example without helper:
//    auto p = GetPtr();
//    auto v = p ? *p : kDefaultStr;
template <typename T>
auto DerefOrDefaultVal(T pt, const details::deref_t<T>& def)
    -> const details::deref_t<T>& {
  if (pt) return *pt;
  return def;
}

// DO NOT USE THIS.
//
// Returns dereferenced value of pointer, if not nullptr.
// Otherwise, returns the callback's return value.
//
// Note: Only intended for use if DerefOrDefault fails to compile.
//
// Usage:
//    // Don't use this. Use DerefOrDefault.
//    auto v = nosniff::DerefOrDefaultCb(GetPtr(), [] { return
//    CreateExpensively(); });
//
// Example without helper:
//    auto p = GetPtr();
//    if (p) return *p;
//    return CreativeExpensively();
template <typename T, typename Cb>
auto DerefOrDefaultCb(T pt, Cb cb) -> details::deref_t<T> {
  if (pt) return *pt;
  return cb();
}
}  // namespace nosniff

// Returns dereferenced value of pointer, if not nullptr.
// Otherwise, returns specified default.
//
// Usage:
//    auto v = DerefOrDefault(GetPtr(), kDefaultStr);
// Or:
//    auto v = DerefOrDefault(GetPtr(), [] { return CreateExpensively(); });
//
// Example without helper:
//    auto p = GetPtr();
//    auto v = p ? *p : kDefaultStr;
// Or:
//    auto p = GetPtr();
//    if (p) return *p;
//    return CreativeExpensively();
template <typename T, typename D>
decltype(auto) DerefOrDefault(T pt, D&& def) {
  if constexpr (std::is_invocable_v<D>)
    return nosniff::DerefOrDefaultCb(pt, std::forward<D>(def));
  else
    return nosniff::DerefOrDefaultVal(pt, std::forward<D>(def));
}

// Returns dereferenced value of pointer, if not nullptr.
// Otherwise, returns the default for that type, by value.
//
// Usage:
//    auto v = DerefOrDefault(GetPtr());
//
// Example without helper:
//    auto p = GetPtr();
//    auto v = p ? *p : 0;
//
// Note that, due to the need to return the local default instance, the
// function must return by value. If this is not optimal or you need to
// return a reference, call the overload that lets you pass your own
// own default in.
template <typename T,
          std::enable_if_t<details::is_deep_copyable_ptr_v<T>, int> = 0>
auto DerefOrDefault(T pt) -> details::deref_t<T> {
  if (pt) return *pt;
  return details::deref_t<T>{};
}

// Returns dereferenced value of pointer to pointer, if not nullptr.
// Otherwise, returns the default for that type, by value.
//
// Usage:
//    auto v = DerefOrDefault(GetUniquePtr());
//
// Example without helper:
//    auto p = GetUniquePtr();
//    auto v = p ? p->get() : 0;
//
// This specialization handles the issue of a pointer to a move-only
// smart pointer, which could not be returned without unwanted side-effects.
// The solution is to downgrade the smart pointer to dumb by returning a
// raw pointer.
//
// TODO: Maybe combine this with the general-purpose one by using if constexpr.
template <typename T,
          std::enable_if_t<!details::is_deep_copyable_ptr_v<T>, int> = 0>
auto DerefOrDefault(T ppt) -> std::remove_reference_t<decltype(&**ppt)> {
  if (ppt && *ppt) return &**ppt;
  return std::remove_reference_t<decltype(&**ppt)>{};
}

// Find value by key, returning value by address.
// In other words, returns a pointer to the value, or if not found, nullptr.
// If only testing for existence, instead use `contains`.
//
// Usage:
//    if (auto v = findPtr(c,k)) foo(*v);
//
// Example without helper:
//    if (auto it = c.find(k); it != c.end()) foo(it->second);
//
// Note: This works for any std::map or std::unordered_map or any other
// container class that is sufficiently similar. It also works for any type of
// key that can successfully be passed to `C::find`, which is why we template on
// `typename K` independently of `typename C` instead of just using `typename
// const C::key_type& k` as the parameter.
//
// Returns value by pointer.
template <typename C, typename K>
auto FindPtr(C& c, const K& k) -> details::mapvalptr_t<C, K> {
  if (auto it = c.find(k); it != c.end()) return &it->second;
  return nullptr;
}

namespace nosniff {
// DO NOT USE THIS.
//
// Find value by key, returning value, or if not found, the specified
// default passed through by reference.
//
// Note: Only intended for use if FindOrDefault fails to compile.
//
// Usage:
//    // Don't use this. Use FindOrDefault.
//    const std::string& foo(const std::string& key) {
//      static const std::string kEmpty;
//      return base::nosniff::FindOrDefaultVal(c_, key, kEmpty); }
//
// Example without helper:
//    const std::string& foo(const std::string& key) {
//    if (auto it = c_.find(key); it != c_.end()) return it->second;
//    static const std::string kEmpty;
//    return kEmpty; }
//
// Returns by reference, so def must not be temporary because const&
// life extension is not transitive.
template <typename C, typename K, typename D>
decltype(auto) FindOrDefaultVal(C& c, const K& k, const D& def) {
  return DerefOrDefaultVal(FindPtr(c, k), def);
}

// DO NOT USE THIS.
//
// Find value by key, returning value, or if not found, the default
// generated by invoking the specified callback.
//
// Note: Only intended for use if FindOrDefault fails to compile.
//
// Usage:
//    // Don't use this. Use FindOrDefault.
//    const std::string foo(const std::string& key) {
//      static const std::string kEmpty;
//      return base::nosniff::FindOrDefaultVal(c_, key, CreativeExpensively); }
//
// Example without helper:
//    const std::string foo(const std::string& key) {
//      if (auto it = c_.find(key); it != c_.end()) return it->second;
//      return CreativeExpensively(); }
template <typename C, typename K, typename D>
decltype(auto) FindOrDefaultCb(C& c, const K& k, D&& def) {
  return DerefOrDefaultCb(FindPtr(c, k), std::forward<D>(def));
}

}  // namespace nosniff

// Find value by key, returning value, or if not found, the specified
// default. When the default is a value, it is passed through by reference.
// When it is a callback, returns the default generated by invoking it.
//
// Usage:
//    const std::string& foo(const std::string& key) {
//      static const std::string kEmpty;
//      return base::FindOrDefault(c_, key, kEmpty); }
//
//    const std::string foo(const std::string& key) {
//      static const std::string kEmpty;
//      return base::nosniff::FindOrDefaultVal(c_, key, CreativeExpensively); }
//
// Example without helper:
//    const std::string& foo(const std::string& key) {
//      if (auto it = c_.find(key); it != c_.end()) return it->second;
//      static const std::string kEmpty;
//      return kEmpty; }
//
//    const std::string foo(const std::string& key) {
//      if (auto it = c_.find(key); it != c_.end()) return it->second;
//      return CreativeExpensively(); }
template <typename C, typename K, typename D>
decltype(auto) FindOrDefault(C& c, const K& k, D&& def) {
  return DerefOrDefault(FindPtr(c, k), std::forward<D>(def));
}

// Find value by key, returning value, or if not found, the default
// instance for that type.
//
// Returns by value, not reference or pointer.
template <typename C, typename K>
auto FindOrDefault(C& c, const K& k) {
  return DerefOrDefault(FindPtr(c, k));
}

// Map and key for efficient manipulation.
//
// Avoids anti-pattern of contains/find followed by insert.
//
// Usage:
//    auto mk = MakeMapKey(c, k);
//    if (!mk)
//      mk.Insert(def);
//
// Use the MakeMapKey helper function to create these.
template <typename KeyT,
          typename ValueT,
          typename CompareT,
          typename AllocatorT,
          typename FinderT>
class MapKey {
 public:
  using MapT = std::map<KeyT, ValueT, CompareT, AllocatorT>;
  using IterT = typename MapT::iterator;

  // Constructs from map and key, finding matching value or where it would go.
  MapKey(MapT& m, FinderT&& key)
      : map_(m),
        key_(std::forward<FinderT>(key)),
        it_(map_.lower_bound(key_)),
        found_(it_ != map_.end() && !(map_.key_comp()(it_->first, key_))) {}

  // Returns map.
  MapT& Map() { return map_; }

  // Returns key. Note that this may be cleared by move when inserting.
  const FinderT& Key() { return key_; }

  // Returns whether found.
  bool Found() const { return found_; }

  // Returns pointer to value, if found, else nullptr.
  ValueT* ValuePtr() const { return found_ ? &(it_->second) : nullptr; }

  // Returns value. If none, first sets value to default of empty instance.
  // This has side-effect semantics like `map::operator[]`.
  ValueT& DefaultValue() {
    if (!found_) {
      if constexpr (std::is_default_constructible_v<ValueT>)
        Emplace();
      else
        throw std::runtime_error("null deref");
    }
    return it_->second;
  }

  // Returns value. If none, first sets value to default from parameter.
  // This has side-effect semantics like `map::operator[]`, except the caller
  // specifies the default value.
  template <
      typename NewValueT,
      std::enable_if_t<!std::is_invocable_r_v<ValueT, NewValueT>, int> = 0>
  ValueT& DefaultValue(NewValueT&& defaultValue) {
    if (!found_) Assign(defaultValue);
    return it_->second;
  }

  // Returns value. If none, first sets value to return from callback.
  // This has side-effect semantics like `map::operator[]`, except the callback
  // specifies the default value. The callback is only invoked if a value needs
  // to be inserted.
  template <typename Cb,
            std::enable_if_t<std::is_invocable_r_v<ValueT, Cb>, int> = 0>
  bool DefaultValueCb(Cb cb) {
    if (!found_) return Assign(cb());
    return it_->second;
  }

  // Returns value. If none, first sets value to default from initializer.
  // This has side-effect semantics like `map::operator[]`, except the caller
  // specifies the initializers for the default value.
  template <typename... Args>
  ValueT& DefaultValueEmplace(Args&&... args) {
    if (!found_) Emplace(std::forward<Args>(args)...);
    return it_->second;
  }

  // Sets value from parameter. Returns whether inserted.
  template <typename NewValueT>
  bool Assign(NewValueT&& value) {
    if (found_) {
      it_->second = std::forward<NewValueT>(value);
      return false;
    } else {
      it_ = map_.insert_or_assign(
          it_, std::move<FinderT>(key_), std::forward<NewValueT>(value));
      found_ = true;
      return true;
    }
  }

  // Sets value from initializer. Returns whether inserted.
  template <typename... Args>
  bool Emplace(Args&&... args) {
    if (found_) {
      it_->second = ValueT(std::forward<Args>(args)...);
      return false;
    } else {
      it_ = map_.try_emplace(
          it_, std::move<FinderT>(key_), std::forward<Args>(args)...);
      found_ = true;
      return true;
    }
  }

  // Returns whether found.
  explicit operator bool() const { return Found(); }

  // Returns value, defaulting to empty.
  ValueT& operator*() { return DefaultValue(); }

  // Returns pointer to value, defaulting to empty if default-constructable.
  ValueT* operator->() { return &DefaultValue(); }

  ValueT* get() const { return ValuePtr(); }

 private:
  MapT& map_;
  FinderT key_;
  IterT it_;
  bool found_;
};

// Use this helper to make MapKey instances.
template <typename KeyT,
          typename ValueT,
          typename CompareT,
          typename AllocatorT,
          typename FinderT>
MapKey<KeyT, ValueT, CompareT, AllocatorT, FinderT> MakeMapKey(
    std::map<KeyT, ValueT, CompareT, AllocatorT>& m, FinderT&& key) {
  return MapKey<KeyT, ValueT, CompareT, AllocatorT, FinderT>(
      m, std::forward<FinderT>(key));
}

}  // namespace beeswax::nectar