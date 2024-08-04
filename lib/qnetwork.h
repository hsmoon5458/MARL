#pragma once

#include <iostream>
#include <torch/nn/init.h>
#include <torch/torch.h>

// Define the Q-network model
class QNetwork : public torch::nn::Module {
public:
  QNetwork(int inputSize, int outputSize, torch::Device device) {
    // Define layers for the neural network
    fc1 = register_module("fc1", torch::nn::Linear(inputSize, 64));
    fc2 = register_module("fc2", torch::nn::Linear(64, 64));
    fc3 = register_module("fc3", torch::nn::Linear(64, outputSize));

    torch::nn::init::xavier_uniform_(fc1->weight);
    torch::nn::init::xavier_uniform_(fc2->weight);
    torch::nn::init::xavier_uniform_(fc3->weight);

    this->to(device);
  }

  // Define the forward pass through the neural network
  torch::Tensor forward(torch::Tensor x) {
    x = torch::relu(fc1(x));
    x = torch::relu(fc2(x));
    x = fc3(x);
    return x;
  }

  // Define a method to get the number of actions the model outputs
  int GetNumberOfActions() const {
    return fc3->weight.size(0); // Accessing the size of weights for output size
  }

private:
  torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
};