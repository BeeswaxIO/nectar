#include "nectar/scoper.h"
#include <ostream>
#include <string>

#include "gtest/gtest.h"

namespace {

using namespace beeswax::nectar;  // NOLINT

TEST(ScoperTest, SmokeTest) {
  int i;

  i = 0;
  if (Scoper scope{[&i] { i += 2; }}) {
    EXPECT_EQ(i, 0);
  }
  EXPECT_EQ(i, 2);

  i = 0;
  if (Scoper scope{[&i] { i += 2; }, false}) {
    EXPECT_EQ(i, 0);
  }
  EXPECT_EQ(i, 0);

  i = 0;
  if (Scoper scope{[&i] { i += 2; }, [&i] { i *= 2; }}) {
    EXPECT_EQ(i, 2);
  }
  EXPECT_EQ(i, 4);

  i = 0;
  if (Scoper scope{[&i] { i += 2; }, [&i] { i *= 2; }, false}) {
    EXPECT_EQ(i, 0);
  }
  EXPECT_EQ(i, 0);

  i = 0;
  if (Scoper scope{[&i] { i += 2; }, [&i] { i *= 2; }}) {
    EXPECT_EQ(i, 2);
    scope.Cancel();
  }
  EXPECT_EQ(i, 2);

  i = 0;
  if (Scoper scope{[&i] { i += 2; }, [&i] { i *= 2; }}) {
    EXPECT_EQ(i, 2);
    Scoper other(std::move(scope));
  }
  EXPECT_EQ(i, 4);

  i = 0;
  if (Scoper scope{[&i] { i += 2; }, [&i] { i *= 2; }}) {
    EXPECT_EQ(i, 2);
    Scoper other;
    other = std::move(scope);
  }
  EXPECT_EQ(i, 4);
}

}  // namespace