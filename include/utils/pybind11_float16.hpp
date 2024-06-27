#include "utils/half.hpp"
#include "utils/pybind11_numpy_scalar.h"
#include <pybind11/numpy.h>

namespace pybind11 { namespace detail {

template <typename T>
struct npy_scalar_caster {
  PYBIND11_TYPE_CASTER(T, _("PleaseOverride"));
  using Array = array_t<T>;

  bool load(handle src, bool convert) {
    // Taken from Eigen casters. Permits either scalar dtype or scalar array.
    handle type = dtype::of<T>().attr("type");  // Could make more efficient.
    if (!convert && !isinstance<Array>(src) && !isinstance(src, type))
      return false;
    Array tmp = Array::ensure(src);
    if (tmp && tmp.size() == 1 && tmp.ndim() == 0) {
      this->value = *tmp.data();
      return true;
    }
    return false;
  }

  static handle cast(T src, return_value_policy, handle) {
    Array tmp({1});
    tmp.mutable_at(0) = src;
    tmp.resize({});
    // You could also just return the array if you want a scalar array.
    object scalar = tmp[tuple()];
    return scalar.release();
  }
};

}}  // namespace pybind11::detail


using float16 = half_float::half;
static_assert(sizeof(float16) == 2, "Bad size");

namespace pybind11 { namespace detail {

// python3 -c 'import numpy as np; print(np.dtype(np.float16).num)'
constexpr int NPY_FLOAT16 = 23;

template <>
struct npy_format_descriptor<float16> {
  static constexpr auto name = _("numpy.float16");
  static pybind11::dtype dtype() {
    handle ptr = npy_api::get().PyArray_DescrFromType_(NPY_FLOAT16);
    return reinterpret_borrow<pybind11::dtype>(ptr);
  }
};

template <>
struct type_caster<float16> : npy_scalar_caster<float16> {
  static constexpr auto name = _("float16");
};

}} 