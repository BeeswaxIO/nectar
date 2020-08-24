load("@rules_cc//cc:defs.bzl", "cc_test")

BW_COPTS = [
    "-Werror",
    "-std=c++17",
    "-Iexternal/gtest/include",
    "-U_FORTIFY_SOURCE",
    "-fstack-protector",
    "-Wall",
    "-B/usr/bin",
    "-B/usr/bin",
    "-Wunused-but-set-parameter",
    "-Wno-free-nonheap-object",
    "-fno-omit-frame-pointer",
 
    "-Werror=sign-compare",
]

cc_test(
    name = "cpp20-test",
    srcs = ["cpp20-test.cc"],
    copts = BW_COPTS,
    deps = [
        "@gtest//:main",
        "//nectar:cpp20",
    ],
)