load("@rules_cc//cc:defs.bzl", "cc_toolchain", "cc_toolchain_suite")
load(":cpp_toolchain.bzl", "cc_toolchain_config")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "empty",
    srcs = [],
)

cc_toolchain_suite(
    name = "cpp",
    toolchains = {
        "k8": ":cc-compiler-clang7",
    },
)

cc_toolchain(
    name = "cc-compiler-clang7",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 1,
    toolchain_config = ":clang7_toolchain",
    toolchain_identifier = "clang7_toolchain",
)

cc_toolchain_config(
    name = "clang7_toolchain",
    compiler = "clang7",
    cpu = "k8",
)
