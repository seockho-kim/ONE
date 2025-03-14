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

#ifndef __ONERT_BACKEND_CPU_OPS_FULLYCONNECTEDLAYER_H__
#define __ONERT_BACKEND_CPU_OPS_FULLYCONNECTEDLAYER_H__

#include <backend/IPortableTensor.h>
#include "../ExternalContext.h"
#include "OperationUtils.h"

#include <exec/IFunction.h>

namespace nnfw::cker
{
class FCTempArena;
} // namespace nnfw::cker

namespace onert::backend::cpu::ops
{

class FullyConnectedLayer : public ::onert::exec::IFunction
{
public:
  FullyConnectedLayer();
  ~FullyConnectedLayer();

public:
  void fullyConnectedFloat32();

  void fullyConnectedQuant8();

  void fullyConnectedHybrid();

  void fullyConnectedSparseWeight();

  void fullyConnectedGGMLWeight();

  void fullyConnected16x1Float32();

  void configure(const IPortableTensor *input, const IPortableTensor *weights,
                 const IPortableTensor *bias, ir::Activation activation,
                 ir::FullyConnectedWeightsFormat weights_format, IPortableTensor *output,
                 const std::shared_ptr<ExternalContext> &external_context);

  void run() override;

  void prepare() override;

protected:
  const IPortableTensor *_input;
  const IPortableTensor *_weights;
  const IPortableTensor *_bias;
  IPortableTensor *_output;

  ir::Activation _activation;
  std::unique_ptr<nnfw::cker::FCTempArena> _temp_arena;

  std::shared_ptr<ExternalContext> _external_context;

  bool _is_hybrid : 1;
  bool _is_shuffled16x1float32 : 1;

#ifdef USE_RUY_GEMV
  uint8_t *_cached_weights = nullptr; // weights to be cached and a key
  bool _is_weights_freed = false;     // is weights freed?
#endif
};

} // namespace onert::backend::cpu::ops

#endif // __ONERT_BACKEND_CPU_OPS_FULLYCONNECTEDLAYER_H__
