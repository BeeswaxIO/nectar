#pragma once

#include <stdexcept>
#include <string_view>

namespace beeswax::nectar {

// cstring_view, which derives from std::string_view but supports c_str().
//
// Use cases:
//
// 1) Interfaces to functions that require zero-terminated strings, such as
// those which call C API's.
//
// 2) For now, suitable for constexpr string literals. Once C++20 is available,
// these will be migrated to constexpr std::string.
//
// Do not use unless you actually need a zero-terminated string; a
// std::string_view is fine.
//
// To ensure that the invariants are enforced, do not reference through base
// class. This is not done anywhere in our code and there is no reason to do
// so, but it should be noted.
//
// Based closely on Andrew Tomazos' ANSI committee proposal.
// http://open-std.org/JTC1/SC22/WG21/docs/papers/2019/p1402r0.pdf
// https://github.com/cplusplus/papers/issues/189
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_cstring_view : public std::basic_string_view<CharT, Traits> {
 public:
  using base = std::basic_string_view<CharT, Traits>;
  using self = basic_cstring_view<CharT, Traits>;
  using size_type = typename base::size_type;
  using base::npos;

  // Expose constructors from base. All of these guarantee termination when not
  // null.
  constexpr basic_cstring_view() noexcept = default;
  constexpr basic_cstring_view(const basic_cstring_view&) noexcept = default;
  constexpr basic_cstring_view(
      const CharT* str)  // NOLINT, implicit is fine here.
      : base(str) {}

  // Construct with specified length, throwing if input is not null or
  // terminated.
  constexpr basic_cstring_view(const CharT* str, size_type len)
      : base{str, len} {
    ThrowIfUnterminated();
  }

  // Construct from std::string, since it doesn't have a conversion operator.
  template <typename OTraits>
  constexpr basic_cstring_view(
      const std::basic_string<CharT, OTraits>& str) noexcept
      : base{str.c_str(), str.size()} {}

  // Expose terminated const char*.
  constexpr const CharT* c_str() const noexcept { return base::data(); }

  // Redundant with C++20.
  constexpr bool starts_with(const std::basic_string_view<CharT>& str) {
    return base::compare(0, str.size(), str) == 0;
  }

  // Redundant with C++20.
  constexpr bool ends_with(const std::basic_string_view<CharT>& str) {
    if (base::size() < str.size()) return false;
    return base::compare(base::size() - str.size(), str.size(), str) == 0;
  }

  // Removing suffix would break termination, so we don't support it.
  constexpr void remove_suffix(size_type) = delete;

  // Substr only valid if it duplicates remove_prefix, so we don't support it.
  constexpr basic_cstring_view substr(size_type, size_type) const = delete;

  // Swap can't maintain invariants, so we don't support it.
  template <typename O>
  constexpr void swap(O& sv) noexcept = delete;

 private:
  constexpr bool IsTerminated() const {
    return !base::data() || *base::end() == CharT('\0');
  }

  constexpr void ThrowIfUnterminated() {
    if (IsTerminated()) return;
    *this = self{};
    throw std::logic_error("Unterminated input to cstring_view");
  }
};

using cstring_view = basic_cstring_view<char>;
using wcstring_view = basic_cstring_view<wchar_t>;
using u16cstring_view = basic_cstring_view<char16_t>;
using u32cstring_view = basic_cstring_view<char32_t>;

inline constexpr basic_cstring_view<char> operator"" _sz(const char* str,
                                                         size_t len) noexcept {
  return basic_cstring_view<char>{str, len};
}

inline constexpr basic_cstring_view<wchar_t> operator"" _sz(
    const wchar_t* str, size_t len) noexcept {
  return basic_cstring_view<wchar_t>{str, len};
}

inline constexpr basic_cstring_view<char16_t> operator"" _sz(
    const char16_t* str, size_t len) noexcept {
  return basic_cstring_view<char16_t>{str, len};
}

inline constexpr basic_cstring_view<char32_t> operator"" _sz(
    const char32_t* str, size_t len) noexcept {
  return basic_cstring_view<char32_t>{str, len};
}

}  // namespace beeswax::nectar
