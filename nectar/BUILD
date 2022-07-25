load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "cpp20",
    hdrs = ["cpp20.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "cstring_view",
    hdrs = ["cstring_view.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "scoper",
    hdrs = ["scoper.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "collections",
    hdrs = ["collections.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "common",
    hdrs = ["common.h"],
    visibility = ["//visibility:public"],
    deps = [
        "collections",
        "cpp20",
        "cstring_view",
        "scoper",
    ],
)
