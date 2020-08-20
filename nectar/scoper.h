#pragma once

#include <functional>
#include <utility>

namespace beeswax::nectar {

// RAII Scoper
//
// Construct with an optional opener and a closer.
//
// The opener, if present, is executed during construction and the closer is
// executed during destruction. As a convenience, the constructor takes an
// optional bool that, when set to false, prevents either from being executed.
//
// If `cancel()` is called prior to destruction, the closer isn't called.
//
// TO DO: Add ability to check return code of initializer, if present, and use
// it to veto do_it.
class Scoper {
 public:
  Scoper() : cleanup_cb_([] {}) {}

  template <typename OpenCb, typename CloseCb>
  explicit Scoper(OpenCb open_cb, CloseCb close_cb, bool do_it = true)
      : cleanup_cb_([do_it, close_cb] {
          if (do_it) close_cb();
        }) {
    if (do_it) open_cb();
  }

  template <typename CloseCb>
  explicit Scoper(CloseCb close_cb, bool do_it = true)
      : cleanup_cb_([do_it, close_cb] {
          if (do_it) close_cb();
        }) {}

  Scoper(Scoper&& scope) noexcept : cleanup_cb_(std::move(scope.cleanup_cb_)) {
    scope.Cancel();
  }

  ~Scoper() { cleanup_cb_(); }

  explicit operator bool() { return true; }

  Scoper& operator=(Scoper&& scope) noexcept {
    cleanup_cb_ = std::move(scope.cleanup_cb_);
    scope.Cancel();
    return *this;
  }

  void Cancel() {
    cleanup_cb_ = [] {};
  }

 private:
  std::function<void()> cleanup_cb_;
};

}  // namespace beeswax::nectar
