load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "torch",
    hdrs = glob([
        "include/**/*.h",
        ]),
    srcs = glob([
        "lib/*",
        ]),
    includes = [
        "include",
        "include/torch/csrc/api/include",
    ],
    linkopts = [
        # Add your python library (ldconfig -p | grep libpython)
        "-L/lib/x86_64-linux-gnu/",
        "-lpython3.8",

        #"-L/lib/x86_64-linux-gnu/",
    ]
)