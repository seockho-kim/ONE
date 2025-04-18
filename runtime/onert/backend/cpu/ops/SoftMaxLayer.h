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

#ifndef __ONERT_BACKEND_CPU_OPS_SOFTMAXLAYER_H__
#define __ONERT_BACKEND_CPU_OPS_SOFTMAXLAYER_H__

#include <backend/IPortableTensor.h>

#include <exec/IFunction.h>

namespace onert::backend::cpu::ops
{

class SoftMaxLayer : public ::onert::exec::IFunction
{
public:
  SoftMaxLayer();

public:
  void softmaxFloat32();

  template <typename T> void softmaxQuant8();

  void configure(const IPortableTensor *input, const float beta, IPortableTensor *output);

  void run() override;

protected:
  const IPortableTensor *_input;
  IPortableTensor *_output;

private:
  float _beta;

  float _table[256];
  uint8_t _uint8_table1[256];
  uint8_t _uint8_table2[256];
};

} // namespace onert::backend::cpu::ops

#endif // __ONERT_BACKEND_CPU_OPS_SOFTMAXLAYER_H__
