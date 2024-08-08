load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("@rules_python//python:defs.bzl", "py_library")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_github_gflags_gflags//:gflags",
        "//lib:tile_env",
        "//lib:agent",
        "//lib:render_util",
        "//lib:utils",
        "//lib/SFML-2.5.1:sfml_lib",
        "//lib/libtorch:torch",
        "//lib:qnetwork",
    ],
    copts = [
        "-std=c++17", # libtorch need c++17.
        "-pthread",
    ],
    linkopts = [
        "-lX11",
        "-pthread",
        ],
)

cc_binary(
    name = "main_pybind",
    srcs = ["main_pybind.cc"],
    deps = [
        "@com_github_google_glog//:glog",
        "//lib/pybind:pybind11",
    ],
    # copts = [
    #     "-fPIC",
    #     "-Iexternal/local_python3_8/include/python3.8",
    # ],
    linkopts = [
        "-lpython3.8",
        "-L/usr/lib/python3.8/config-3.8-x86_64-linux-gnu",  # Adjust this path if necessary
    ],
    data = [":add_numbers_py"],
)

py_library(
    name = "add_numbers_py",
    srcs = ["add_numbers.py"],
)

pybind_extension(
    name = "add_wrapper",
    srcs = ["wrapper.cpp"],
    deps = [
        ":add_numbers_py",
    ],
)
