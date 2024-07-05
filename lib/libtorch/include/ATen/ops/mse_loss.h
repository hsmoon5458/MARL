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



#include <ATen/ops/mse_loss_ops.h>

namespace at {


// aten::mse_loss.out(Tensor self, Tensor target, int reduction=Mean, *, Tensor(a!) out) -> Tensor(a!)
inline at::Tensor & mse_loss_out(at::Tensor & out, const at::Tensor & self, const at::Tensor & target, int64_t reduction=at::Reduction::Mean) {
    return at::_ops::mse_loss_out::call(self, target, reduction, out);
}
// aten::mse_loss.out(Tensor self, Tensor target, int reduction=Mean, *, Tensor(a!) out) -> Tensor(a!)
inline at::Tensor & mse_loss_outf(const at::Tensor & self, const at::Tensor & target, int64_t reduction, at::Tensor & out) {
    return at::_ops::mse_loss_out::call(self, target, reduction, out);
}

// aten::mse_loss(Tensor self, Tensor target, int reduction=Mean) -> Tensor
inline at::Tensor mse_loss(const at::Tensor & self, const at::Tensor & target, int64_t reduction=at::Reduction::Mean) {
    return at::_ops::mse_loss::call(self, target, reduction);
}

}
