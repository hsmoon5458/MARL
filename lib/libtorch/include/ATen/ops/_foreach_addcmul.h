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



#include <ATen/ops/_foreach_addcmul_ops.h>

namespace at {


// aten::_foreach_addcmul.Scalar(Tensor[] self, Tensor[] tensor1, Tensor[] tensor2, Scalar value=1) -> Tensor[]
inline ::std::vector<at::Tensor> _foreach_addcmul(at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, const at::Scalar & value=1) {
    return at::_ops::_foreach_addcmul_Scalar::call(self, tensor1, tensor2, value);
}

// aten::_foreach_addcmul.ScalarList(Tensor[] self, Tensor[] tensor1, Tensor[] tensor2, Scalar[] scalars) -> Tensor[]
inline ::std::vector<at::Tensor> _foreach_addcmul(at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, at::ArrayRef<at::Scalar> scalars) {
    return at::_ops::_foreach_addcmul_ScalarList::call(self, tensor1, tensor2, scalars);
}

// aten::_foreach_addcmul.Tensor(Tensor[] self, Tensor[] tensor1, Tensor[] tensor2, Tensor scalars) -> Tensor[]
inline ::std::vector<at::Tensor> _foreach_addcmul(at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, const at::Tensor & scalars) {
    return at::_ops::_foreach_addcmul_Tensor::call(self, tensor1, tensor2, scalars);
}

// aten::_foreach_addcmul_.Scalar(Tensor(a!)[] self, Tensor[] tensor1, Tensor[] tensor2, Scalar value=1) -> ()
inline void _foreach_addcmul_(at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, const at::Scalar & value=1) {
    return at::_ops::_foreach_addcmul__Scalar::call(self, tensor1, tensor2, value);
}

// aten::_foreach_addcmul_.ScalarList(Tensor(a!)[] self, Tensor[] tensor1, Tensor[] tensor2, Scalar[] scalars) -> ()
inline void _foreach_addcmul_(at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, at::ArrayRef<at::Scalar> scalars) {
    return at::_ops::_foreach_addcmul__ScalarList::call(self, tensor1, tensor2, scalars);
}

// aten::_foreach_addcmul_.Tensor(Tensor(a!)[] self, Tensor[] tensor1, Tensor[] tensor2, Tensor scalars) -> ()
inline void _foreach_addcmul_(at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, const at::Tensor & scalars) {
    return at::_ops::_foreach_addcmul__Tensor::call(self, tensor1, tensor2, scalars);
}

// aten::_foreach_addcmul.Scalar_out(Tensor[] self, Tensor[] tensor1, Tensor[] tensor2, Scalar value=1, *, Tensor(a!)[] out) -> ()
inline void _foreach_addcmul_out(at::TensorList out, at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, const at::Scalar & value=1) {
    return at::_ops::_foreach_addcmul_Scalar_out::call(self, tensor1, tensor2, value, out);
}
// aten::_foreach_addcmul.Scalar_out(Tensor[] self, Tensor[] tensor1, Tensor[] tensor2, Scalar value=1, *, Tensor(a!)[] out) -> ()
inline void _foreach_addcmul_outf(at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, const at::Scalar & value, at::TensorList out) {
    return at::_ops::_foreach_addcmul_Scalar_out::call(self, tensor1, tensor2, value, out);
}

// aten::_foreach_addcmul.ScalarList_out(Tensor[] self, Tensor[] tensor1, Tensor[] tensor2, Scalar[] scalars, *, Tensor(a!)[] out) -> ()
inline void _foreach_addcmul_out(at::TensorList out, at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, at::ArrayRef<at::Scalar> scalars) {
    return at::_ops::_foreach_addcmul_ScalarList_out::call(self, tensor1, tensor2, scalars, out);
}
// aten::_foreach_addcmul.ScalarList_out(Tensor[] self, Tensor[] tensor1, Tensor[] tensor2, Scalar[] scalars, *, Tensor(a!)[] out) -> ()
inline void _foreach_addcmul_outf(at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, at::ArrayRef<at::Scalar> scalars, at::TensorList out) {
    return at::_ops::_foreach_addcmul_ScalarList_out::call(self, tensor1, tensor2, scalars, out);
}

// aten::_foreach_addcmul.Tensor_out(Tensor[] self, Tensor[] tensor1, Tensor[] tensor2, Tensor scalars, *, Tensor(a!)[] out) -> ()
inline void _foreach_addcmul_out(at::TensorList out, at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, const at::Tensor & scalars) {
    return at::_ops::_foreach_addcmul_Tensor_out::call(self, tensor1, tensor2, scalars, out);
}
// aten::_foreach_addcmul.Tensor_out(Tensor[] self, Tensor[] tensor1, Tensor[] tensor2, Tensor scalars, *, Tensor(a!)[] out) -> ()
inline void _foreach_addcmul_outf(at::TensorList self, at::TensorList tensor1, at::TensorList tensor2, const at::Tensor & scalars, at::TensorList out) {
    return at::_ops::_foreach_addcmul_Tensor_out::call(self, tensor1, tensor2, scalars, out);
}

}
