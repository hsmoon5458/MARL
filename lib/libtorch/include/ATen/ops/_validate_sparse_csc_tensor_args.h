#pragma once

// @generated by torchgen/gen.py from Function.h

#include <ATen/Context.h>
#include <ATen/DeviceGuard.h>
#include <ATen/TensorUtils.h>
#include <ATen/TracerMode.h>
#include <ATen/core/Generator.h>
#include <ATen/core/Reduction.h>
#include <ATen/core/Tensor.h>
#include <c10/core/Scalar.h>
#include <c10/core/Storage.h>
#include <c10/core/TensorOptions.h>
#include <c10/util/Deprecated.h>
#include <c10/util/Optional.h>



#include <ATen/ops/_validate_sparse_csc_tensor_args_ops.h>

namespace at {


// aten::_validate_sparse_csc_tensor_args(Tensor ccol_indices, Tensor row_indices, Tensor values, int[] size) -> ()
inline void _validate_sparse_csc_tensor_args(const at::Tensor & ccol_indices, const at::Tensor & row_indices, const at::Tensor & values, at::IntArrayRef size) {
    return at::_ops::_validate_sparse_csc_tensor_args::call(ccol_indices, row_indices, values, size);
}

}
