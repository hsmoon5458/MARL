#include "lib/pybind/include/pybind11/pybind11.h"

namespace py = pybind11;

int add_numbers_wrapper(int a, int b) {
  py::module_ add_module = py::module_::import("add_numbers");
  py::object result = add_module.attr("add_numbers")(a, b);
  return result.cast<int>();
}

PYBIND11_MODULE(add_wrapper, m) {
  m.def("add_numbers", &add_numbers_wrapper,
        "A function that adds two numbers using Python");
}