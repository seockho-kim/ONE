/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd. All Rights Reserved
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

#ifndef __ONERT_BACKEND_TRAIN_OPS_ELEMENTWISEACTIVATIONLAYER_H__
#define __ONERT_BACKEND_TRAIN_OPS_ELEMENTWISEACTIVATIONLAYER_H__

#include <backend/IPortableTensor.h>
#include <ops/ElementwiseActivationLayer.h>

#include <exec/train/ITrainableFunction.h>

namespace onert::backend::train::ops
{

enum class ElementwiseActivationType
{
  kReLU,
};

class ElementwiseActivationLayer : public ::onert::exec::train::ITrainableFunction,
                                   public cpu::ops::ElementwiseActivationLayer
{
public:
  ElementwiseActivationLayer();

  void configureBackward(const IPortableTensor *input, IPortableTensor *back_prop_input,
                         const IPortableTensor *back_prop_output, float alpha, float beta,
                         ElementwiseActivationType op_type);
  void forward(bool training) override;
  void backward() override;

private:
  IPortableTensor *_back_prop_input = nullptr;
  const IPortableTensor *_back_prop_output = nullptr;

  ElementwiseActivationType _op_type = ElementwiseActivationType::kReLU;
  std::function<void(const IPortableTensor *output, const IPortableTensor *incoming,
                     IPortableTensor *outgoing)>
    _backward_kernel;
};

} // namespace onert::backend::train::ops

#endif // __ONERT_BACKEND_TRAIN_OPS_ELEMENTWISEACTIVATIONLAYER_H__
