load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "cpp20-test",
    srcs = ["cpp20-test.cc"],
    deps = [
        "//nectar:cpp20",
        "@com_google_gtest//:gtest_main",
    ],
)

cc_test(
    name = "cstring_view-test",
    srcs = ["cstring_view-test.cc"],
    deps = [
        "//nectar:cstring_view",
        "@com_google_gtest//:gtest_main",
    ],
)
