#pragma once

#include <torch/torch.h>
#include <iostream>
// Define the Q-network model
class QNetwork : public torch::nn::Module
{
public:
    QNetwork(int inputSize, int outputSize, torch::Device device)
    {
        // Define layers for the neural network
        fc1 = register_module("fc1", torch::nn::Linear(inputSize, 64));
        fc2 = register_module("fc2", torch::nn::Linear(64, 64));
        fc3 = register_module("fc3", torch::nn::Linear(64, outputSize));
        this->to(device);
    }

    // Define the forward pass through the neural network
    torch::Tensor Forward(torch::Tensor x)
    {
        x = torch::relu(fc1->forward(x));
        x = torch::relu(fc2->forward(x));
        x = fc3->forward(x);
        return x;
    }

    // Define a method to get the number of actions the model outputs
    int GetNumberOfActions() const
    {
        return fc3->weight.size(0); // Accessing the size of weights for output size
    }

private:
    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
};