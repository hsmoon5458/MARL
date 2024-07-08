load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        "//lib:tile_env",
        "//lib:agent",
        "//lib:utils",
        "//lib/SFML-2.5.1:sfml_lib",
        "//lib/libtorch:torch",
        "//lib:qnetwork",
    ],
    copts = ["-std=c++17"],
)