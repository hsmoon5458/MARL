#pragma once

// @generated by torchgen/gen.py from NativeFunction.h

#include <c10/core/Scalar.h>
#include <c10/core/Storage.h>
#include <c10/core/TensorOptions.h>
#include <c10/util/Deprecated.h>
#include <c10/util/Optional.h>
#include <c10/core/QScheme.h>
#include <ATen/core/Reduction.h>
#include <ATen/core/Tensor.h>
#include <tuple>
#include <vector>


namespace at {
namespace native {
TORCH_API at::Tensor trapz(const at::Tensor & y, const at::Tensor & x, int64_t dim=-1);
TORCH_API at::Tensor trapz(const at::Tensor & y, double dx=1, int64_t dim=-1);
} // namespace native
} // namespace at
