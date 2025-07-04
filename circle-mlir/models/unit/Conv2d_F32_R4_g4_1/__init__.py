import torch


# Generate Conv2d operator with Float32, Rank-4 with groups.
# this graph is not convertable to DepthwiseConv and should be
# converted to Transpose-Split-Conv-Concat-Add-Transpose.
class net_Conv2d(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.op = torch.nn.Conv2d(8, 4, 1, groups=4)

    def forward(self, input):
        return self.op(input)

    def onnx_opset_version(self):
        # TODO set to appropriate value
        return 14


_model_ = net_Conv2d()

_inputs_ = torch.randn(1, 8, 6, 3)
