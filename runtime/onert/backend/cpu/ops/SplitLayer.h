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

#ifndef __ONERT_BACKEND_CPU_OPS_SPLITLAYER_H__
#define __ONERT_BACKEND_CPU_OPS_SPLITLAYER_H__

#include <backend/IPortableTensor.h>

#include <exec/IFunction.h>

namespace onert::backend::cpu::ops
{

class SplitLayer : public ::onert::exec::IFunction
{
public:
  SplitLayer();

public:
  template <typename T> void split(void);

  void configure(const IPortableTensor *input, const IPortableTensor *axis, uint16_t num_splits,
                 std::vector<IPortableTensor *> &outputs);

  void run() override;

private:
  const IPortableTensor *_input;
  const IPortableTensor *_axis;
  uint16_t _num_splits;
  std::vector<IPortableTensor *> _outputs;
};

} // namespace onert::backend::cpu::ops

#endif // __ONERT_BACKEND_CPU_OPS_SPLITLAYER_H__
