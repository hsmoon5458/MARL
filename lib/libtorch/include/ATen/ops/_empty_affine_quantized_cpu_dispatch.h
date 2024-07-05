#pragma once
// @generated by torchgen/gen.py from DispatchKeyFunction.h

// NB: The implementing C++ file is RegisterDispatchKey.cpp

// The only #includes we need are for custom classes that have defaults in the C++ API
#include <c10/core/MemoryFormat.h>
#include <c10/core/Scalar.h>
#include <ATen/core/Reduction.h>

// Forward declarations of any types needed in the operator signatures.
// We can't directly include these classes because it will cause circular include dependencies.
// This file is included by TensorBody.h, which defines the Tensor class.
#include <ATen/core/ATen_fwd.h>

namespace at {

namespace cpu {

TORCH_API at::Tensor _empty_affine_quantized(at::IntArrayRef size, at::TensorOptions options={}, double scale=1, int64_t zero_point=0, c10::optional<at::MemoryFormat> memory_format=MemoryFormat::Contiguous);
TORCH_API at::Tensor _empty_affine_quantized(at::IntArrayRef size, c10::optional<at::ScalarType> dtype, c10::optional<at::Layout> layout, c10::optional<at::Device> device, c10::optional<bool> pin_memory, double scale, int64_t zero_point, c10::optional<at::MemoryFormat> memory_format);
TORCH_API at::Tensor _empty_affine_quantized_symint(c10::SymIntArrayRef size, at::TensorOptions options={}, double scale=1, int64_t zero_point=0, c10::optional<at::MemoryFormat> memory_format=MemoryFormat::Contiguous);
TORCH_API at::Tensor _empty_affine_quantized_symint(c10::SymIntArrayRef size, c10::optional<at::ScalarType> dtype, c10::optional<at::Layout> layout, c10::optional<at::Device> device, c10::optional<bool> pin_memory, double scale, int64_t zero_point, c10::optional<at::MemoryFormat> memory_format);

} // namespace cpu
} // namespace at
