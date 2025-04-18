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

#ifndef __ONERT_BACKEND_CPU_OPS_ARGMINMAXLAYER_H__
#define __ONERT_BACKEND_CPU_OPS_ARGMINMAXLAYER_H__

#include <backend/IPortableTensor.h>

#include <exec/IFunction.h>

namespace onert::backend::cpu::ops
{

class ArgMinMaxLayer : public ::onert::exec::IFunction
{
public:
  ArgMinMaxLayer() : _input(nullptr), _output(nullptr), _axis(nullptr), _is_arg_max(true) {}

public:
  void configure(const IPortableTensor *indices, IPortableTensor *output,
                 const IPortableTensor *axis, bool is_arg_max);

  void run() override;

private:
  const IPortableTensor *_input;
  IPortableTensor *_output;
  const IPortableTensor *_axis;
  bool _is_arg_max;
};

} // namespace onert::backend::cpu::ops

#endif // __ONERT_BACKEND_CPU_OPS_ARGMINMAXLAYER_H__
