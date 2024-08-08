load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "pybind11",
    hdrs = glob([
        "include/pybind11/**/*.h",
        "include/pybind11/*.h",
    ]),
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = ["@local_python3_8//:python3_8_headers"],
)