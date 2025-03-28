/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All Rights Reserved
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

#include <backend/basic/TensorBuilder.h>

#include <util/logging.h>

#include <cassert>

namespace onert::backend::basic
{

TensorBuilder::TensorBuilder(
  const std::shared_ptr<TensorRegistry> &tensor_reg,
  const ir::OperandIndexMap<ir::OperandIndex> &shared_memory_operand_indexes)
  : _tensor_reg{tensor_reg}, _dynamic_tensor_mgr{new DynamicTensorManager(_tensor_reg)},
    _static_tensor_mgr{new StaticTensorManager(_tensor_reg, _dynamic_tensor_mgr.get(),
                                               shared_memory_operand_indexes)},
    _shared_memory_operand_indexes{shared_memory_operand_indexes}
{
  /* empty */
}

TensorBuilder::TensorBuilder(
  const std::shared_ptr<TensorRegistry> &tensor_reg, const std::string planner_id,
  const ir::OperandIndexMap<ir::OperandIndex> &shared_memory_operand_indexes)
  : _tensor_reg{tensor_reg}, _dynamic_tensor_mgr{new DynamicTensorManager(_tensor_reg)},
    _static_tensor_mgr{new StaticTensorManager(_tensor_reg, planner_id, _dynamic_tensor_mgr.get(),
                                               shared_memory_operand_indexes)}
{
  /* empty */
}

void TensorBuilder::registerTensorInfo(const ir::OperandIndex &ind, const ir::OperandInfo &info)
{
  _tensor_info_map.emplace(ind, info);

  if (info.isDynamic())
  {
    _dynamic_tensor_mgr->buildTensor(ind, info);
  }
  else
  {
    _static_tensor_mgr->buildTensor(ind, info, info.isConstant());
  }
}

void TensorBuilder::notifyFirstUse(const ir::OperandIndex &ind)
{
  assert(_tensor_info_map.find(ind) != _tensor_info_map.end());
  const auto &tensor_info = _tensor_info_map.at(ind);

  if (!_tensor_reg->getNativeTensor(ind)->is_dynamic())
  {
    const auto size = tensor_info.total_size();
    _static_tensor_mgr->claimPlan(ind, size);
  }
}

void TensorBuilder::notifyLastUse(const ir::OperandIndex &ind)
{
  if (!_tensor_reg->getNativeTensor(ind)->is_dynamic())
  {
    _static_tensor_mgr->releasePlan(ind);
  }
}

bool TensorBuilder::isRegistered(const ir::OperandIndex &ind) const
{
  return _tensor_info_map.find(ind) != _tensor_info_map.end();
}

void TensorBuilder::allocate(void) { _static_tensor_mgr->allocateNonconsts(); }

const ir::OperandIndexMap<ir::OperandIndex> &TensorBuilder::getSharedMemoryOperandIndexes() const
{
  return _shared_memory_operand_indexes;
}

} // namespace onert::backend::basic
