/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ONERT_IR_SHAPE_H__
#define __ONERT_IR_SHAPE_H__

#include "ir/Layout.h"

#include <cassert>
#include <cstdint>
#include <vector>
#include <algorithm>

namespace onert::ir
{

/**
 * @brief  Structure to have values of dimensions for feature
 */
struct FeatureShape
{
  int32_t N; /**< The batch value  */
  int32_t C; /**< The depth value  */
  int32_t H; /**< The height value */
  int32_t W; /**< The width value  */

  /**
   * @brief  Construct FeatureShape object using default constrcutor
   */
  FeatureShape() = default;
  /**
   * @brief  Construct FeatureShape object with three values of dimensions
   * @param[in]  depth  The depth value
   * @param[in]  height The height value
   * @param[in]  width  The width value
   */
  FeatureShape(int32_t depth, int32_t height, int32_t width) : N{1}, C{depth}, H{height}, W{width}
  {
    // DO NOTHING
  }
  /**
   * @brief  Construct FeatureShape object with four values of dimensions
   * @param[in]  batch  The batch value
   * @param[in]  depth  The depth value
   * @param[in]  height The height value
   * @param[in]  width  The width value
   */
  FeatureShape(int32_t batch, int32_t depth, int32_t height, int32_t width)
    : N{batch}, C{depth}, H{height}, W{width}
  {
    // DO NOTHING
  }
};

struct Shape
{
public:
  static inline int32_t const kUnspecifiedDim = -1;
  // NNFW_MAX_RANK is 6
  static inline int32_t const kMaxRank = 6;

  Shape() = default;

  explicit Shape(int rank) : _dimensions(rank) {}

  Shape(std::initializer_list<int32_t> dimensions) : _dimensions(dimensions) {}

  int rank() const { return _dimensions.size(); }

  const std::vector<int32_t> &dims() const { return _dimensions; }

  int32_t dim(int i) const
  {
    assert(rank() != 0 || i == 0);
    return rank() == 0 ? 1 : _dimensions.at(i);
  }

  // TODO Fix different behavior with const version
  int32_t &dim(int i) { return _dimensions.at(i); }

  /**
   * @brief Returns number of elements when rank or dim is specified
   */
  uint64_t num_elements() const;

public:
  FeatureShape asFeature() const;

  /**
   * @brief Add dimension to the beginning
   * @param[in] d dimension to add to the beginning
   */
  void prepend(int32_t d) { _dimensions.insert(_dimensions.cbegin(), d); }

  /**
   * @brief Add dimension to the end
   * @param[in] d dimension to add to the end
   */
  void append(int32_t d) { _dimensions.emplace_back(d); }

  /**
   * @brief Extend rank of Shape object for operand with param.
   * @param[in] to_rank The rank value to be extended to
   */
  void extendRank(int to_rank);

  /**
   * @brief Find out if any dimension is unspecified. If the rank is not specified, it returns
   * false.
   * \see https://developer.android.com/ndk/reference/struct/a-neural-networks-operand-type
   * @note  base_loader set dim to -1 when there is unknown dim in input tensor
   */
  bool hasUnspecifiedDims() const
  {
    assert(_dimensions.size() > 0);
    return (std::find(_dimensions.begin(), _dimensions.end(), kUnspecifiedDim) !=
            _dimensions.end());
  }

private:
  std::vector<int32_t> _dimensions;
};

inline bool operator==(const Shape &lhs, const Shape &rhs) { return lhs.dims() == rhs.dims(); }
inline bool operator!=(const Shape &lhs, const Shape &rhs) { return lhs.dims() != rhs.dims(); }

/**
 * @brief   Converts shape when its rank is 4
 *
 * @return  Return a shape based on permutation type.
 *          If rank is not 4, input shape is returned without conversion.
 */
Shape convertShape(const Shape &shape, const PermuteType &type);

/**
 * @brief Find out if tha rank in this shape is "maybe" unspecified.
 *        Note that when rank == 0, shape could represent scalar or unspecified rank
 * \see https://developer.android.com/ndk/reference/struct/a-neural-networks-operand-type
 */
inline bool rankMaybeUnspecified(const ir::Shape &shape) { return (shape.rank() == 0); }

} // namespace onert::ir

#endif // __ONERT_IR_SHAPE_H__
