load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "cpp20_test",
    srcs = ["cpp20_test.cc"],
    deps = [
        "//nectar:cpp20",
        "@com_google_gtest//:gtest_main",
    ],
)

cc_test(
    name = "cstring_view_test",
    srcs = ["cstring_view_test.cc"],
    deps = [
        "//nectar:cstring_view",
        "@com_google_gtest//:gtest_main",
    ],
)

cc_test(
    name = "scoper_test",
    srcs = ["scoper_test.cc"],
    deps = [
        "//nectar:scoper",
        "@com_google_gtest//:gtest_main",
    ],
)

cc_test(
    name = "collections_test",
    srcs = ["collections_test.cc"],
    deps = [
        "//nectar:collections",
        "@com_google_gtest//:gtest_main",
    ],
)