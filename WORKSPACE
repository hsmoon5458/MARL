load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

http_archive(
    name = "com_github_google_glog",
    sha256 = "c17d85c03ad9630006ef32c7be7c65656aba2e7e2fbfc82226b7e680c771fc88",
    strip_prefix = "glog-0.7.1",
    urls = ["https://github.com/google/glog/archive/v0.7.1.zip"],
)

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

# Local python path
new_local_repository(
    name = "local_python3_8",
    path = "/usr",
    build_file_content = """
cc_library(
    name = "python3_8_headers",
    hdrs = glob(["include/python3.8/**/*.h"]),
    includes = ["include/python3.8"],
    visibility = ["//visibility:public"],
)
""",
)

# Pybind11
http_archive(
    name = "pybind11_bazel",
    strip_prefix = "pybind11_bazel-faf56fb3df11287f26dbc66fdedf60a2fc2c6631",
    urls = ["https://github.com/pybind/pybind11_bazel/archive/faf56fb3df11287f26dbc66fdedf60a2fc2c6631.zip"],
)

http_archive(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    strip_prefix = "pybind11-2.11.1",
    urls = ["https://github.com/pybind/pybind11/archive/v2.11.1.tar.gz"],
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")

# Rules Python
http_archive(
    name = "rules_python",
    sha256 = "5868e73107a8e85d8f323806e60cad7283f34b32163ea6ff1020cf27abef6036",
    strip_prefix = "rules_python-0.25.0",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.25.0/rules_python-0.25.0.tar.gz",
)

load("@rules_python//python:repositories.bzl", "py_repositories")
py_repositories()

# Optional: Only if you need pip dependencies
load("@rules_python//python:pip.bzl", "pip_parse")

pip_parse(
    name = "pip_deps",
    requirements_lock = "//:requirements.txt",
)

load("@pip_deps//:requirements.bzl", "install_deps")
install_deps()