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
        "k8": ":cc-compiler-clang13",
    },
)

cc_toolchain(
    name = "cc-compiler-clang13",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 1,
    toolchain_config = ":clang13_toolchain",
    toolchain_identifier = "clang13_toolchain",
)

cc_toolchain_config(
    name = "clang13_toolchain",
    compiler = "clang13",
    cpu = "k8",
)
