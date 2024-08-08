#include "lib/pybind/include/pybind11/embed.h"
#include "lib/pybind/include/pybind11/pybind11.h"
#include <glog/logging.h>

namespace py = pybind11;

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);

  py::scoped_interpreter guard{};

  try {
    py::module_ add_wrapper = py::module_::import("add_wrapper");
    int result = add_wrapper.attr("add_numbers")(5, 7).cast<int>();
    LOG(INFO) << "Result of adding 5 and 7: " << result;
  } catch (py::error_already_set &e) {
    LOG(ERROR) << "Python error: " << e.what();
    return 1;
  }

  return 0;
}