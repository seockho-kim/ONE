import torch


# Generate Concat operator with Float32, Rank-4 and dim 3 (width)
class net_concat(torch.nn.Module):
    def __init__(self):
        super().__init__()

    def forward(self, inputs):
        return torch.cat((inputs[0], inputs[1]), 3)

    def onnx_opset_version(self):
        # TODO set to appropriate value
        return 14


_model_ = net_concat()

_inputs_ = [torch.randn(1, 32, 1, 8), torch.randn(1, 32, 1, 8)]
