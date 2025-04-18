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

#include "MemoryPlanner.h"
#include "util/logging.h"
#include <cassert>

namespace onert::backend::basic
{

void BumpPlanner::claim(const ir::OperandIndex &ind, size_t size)
{
  Block blk{_capacity, size};
  _mem_plans[ind] = blk;
  _capacity += size;

  VERBOSE(BP_PLANNER) << "CLAIM(" << ind << "): " << blk.offset << ", " << blk.size << std::endl;
}

void BumpPlanner::release(const ir::OperandIndex &ind)
{
  VERBOSE(BP_PLANNER) << "RELEASE(" << ind << "): "
                      << "NOTHING does" << std::endl;
}

// There are some assumptions for claiming memory(== making a reservation for memory).
// 1. About _claim_table(std::map).
//   - The table's data structure is std::map so that it always sorts
//     value(OperandIndex) by key(base_offset).
//   - This claim() inserts key/value into _claim_table and the release() removes the key/value from
//     _claim_table.
//   - _claim_table shows the memory status at a certain point in time. Therefore,
//     - If _claim_table has an offset and a certain size at a certain point in time,
//       it means the place at the offset has been already claimed(== can't claim now. need to find
//       someplace new).
//     - If _claim_table doesn't have any element for an offset and a certain size at a certain
//       point in time, it means the place at the offset can be claimed.
// 2. In the loop for _claim_table, we can assume the current claim_base_offset value is bigger than
//    the previous claim_base_offset.
void FirstFitPlanner::claim(const ir::OperandIndex &ind, size_t size)
{
  // Find the right position for claiming
  uint32_t next_offset = 0;
  for (const auto &[claimed_base_offset, claimed_operand_idx] : _claim_table)
  {
    auto claimed_size = _mem_plans[claimed_operand_idx].size;
    if (next_offset + size <= claimed_base_offset)
    {
      break;
    }
    else
    {
      next_offset = claimed_base_offset + claimed_size;
    }
  }

  // Now next_offset is set to the proper offset
  _claim_table[next_offset] = ind;
  _mem_plans[ind] = {next_offset, size};

  VERBOSE(FF_PLANNER) << "claim(" << ind << "): [+" << next_offset << ", " << size << "sz]"
                      << std::endl;

  if (_capacity < next_offset + size)
  {
    _capacity = next_offset + size;
  }
}

void FirstFitPlanner::release(const ir::OperandIndex &ind)
{
  for (auto it = _claim_table.cbegin(); it != _claim_table.cend(); ++it)
  {
    if (it->second == ind)
    {
      uint32_t offset = it->first;
      uint32_t index = ind.value();
      uint32_t size = _mem_plans[ind].size;

      _claim_table.erase(it);

      VERBOSE(FF_PLANNER) << "release(" << index << "): [+" << offset << ", " << size << "sz]"
                          << std::endl;
      return;
    }
  }
  assert(!"Cannot release for given index. It has been not claimed or released already.");
}

WICPlanner::WICPlanner()
  : _initialized(false), _capacity(0), _mem_plans(), _live_operands(), _interference_graph(),
    _operands()
{
  // DO NOTHING
}

void WICPlanner::claim(const ir::OperandIndex &ind, size_t size)
{
  _operands.emplace(size, ind);
  _interference_graph[ind].insert(_interference_graph[ind].end(), _live_operands.cbegin(),
                                  _live_operands.cend());
  for (const auto &live_operand : _live_operands)
  {
    _interference_graph[live_operand].emplace_back(ind);
  }
  _live_operands.emplace(ind);

  VERBOSE(WIC_PLANNER) << "claim(" << ind << "): [" << size << "sz]" << std::endl;
}

void WICPlanner::release(const ir::OperandIndex &ind)
{
  _live_operands.erase(ind);
  VERBOSE(WIC_PLANNER) << "release(" << ind << ")" << std::endl;
}

/*
 * Build memory plans using liveness and size of operands
 * 1. Build inference graph at claim
 *   - Two operands interfere if they have overlapped live range
 * 2. Sort operands in descending order of size
 *   - Use std::multimap to sort operands
 * 3. Allocate memory block for sorted operands
 *   - Find free memory block which does not overlap with interfered operands
 */
void WICPlanner::buildMemoryPlans()
{
  for (const auto &[size, ind] : _operands)
  {
    VERBOSE(WIC_PLANNER) << "build_plan(" << ind << "): [" << size << "sz]" << std::endl;

    uint32_t next_offset = 0;
    if (_interference_graph.count(ind))
    {
      // Find interfered memory plans and sort them by offset
      std::multimap<uint32_t, uint32_t> interfered_plans;
      for (const auto &interference : _interference_graph[ind])
      {
        if (_mem_plans.count(interference))
          interfered_plans.emplace(_mem_plans[interference].offset, _mem_plans[interference].size);
      }

      // Find free memory block in first-fit manner
      for (const auto &[claimed_base_offset, claimed_size] : interfered_plans)
      {
        VERBOSE(WIC_PLANNER) << "interfere : [+" << claimed_base_offset << ", " << claimed_size
                             << "sz]" << std::endl;
        if (next_offset + size <= claimed_base_offset)
        {
          break;
        }
        else if (next_offset < claimed_base_offset + claimed_size)
        {
          next_offset = claimed_base_offset + claimed_size;
        }
      }
    }
    else
    {
      VERBOSE(WIC_PLANNER) << "No interference" << std::endl;
    }

    _mem_plans[ind] = {next_offset, size};
    VERBOSE(WIC_PLANNER) << "alloc(" << ind << "): [+" << next_offset << ", " << size << "sz]"
                         << std::endl;

    if (_capacity < next_offset + size)
    {
      _capacity = next_offset + size;
    }
  }
  _initialized = true;
  _interference_graph.clear();
  _operands.clear();
}

WICPlanner::MemoryPlans &WICPlanner::memory_plans()
{
  if (!_initialized)
    buildMemoryPlans();
  return _mem_plans;
}

} // namespace onert::backend::basic
