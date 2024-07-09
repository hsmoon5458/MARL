load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tile_env",
    hdrs = ["tile_env.h",],
    srcs = ["tile_env.cpp",],
    deps = [
        "@com_github_google_glog//:glog",
        "//lib/SFML-2.5.1:sfml_lib",
        "//lib/libtorch:torch",
        ":utils",
        ":agent",
        ],
    copts = ["-std=c++17"], # libtorch need c++17.
)

cc_library(
    name = "agent",
    hdrs = ["agent.h",],
    #srcs = ["agent.cpp"],
    deps = [
        "@com_github_google_glog//:glog",
        "//lib/SFML-2.5.1:sfml_lib",
        "//lib/libtorch:torch",
        ":qnetwork",
        ":replay_buffer",
        ],
)

cc_library(
    name = "utils",
    hdrs = ["utils.h",],
    deps = ["//lib/SFML-2.5.1:sfml_lib"],
)

cc_library(
    name = "qnetwork",
    hdrs = ["qnetwork.h",],
    deps = [
        "//lib/libtorch:torch"
        ],
)

cc_library(
    name = "replay_buffer",
    hdrs = ["replay_buffer.h",],
)