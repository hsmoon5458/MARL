#pragma once

#include "torch/torch.h"
#include "qnetwork.h"
#include "replay_buffer.h"

#include <glog/logging.h>
#include <memory>

namespace
{
    constexpr float BUFFER_SIZE = int(1e5); // replay buffer size
    constexpr int BATCH_SIZE = 64;          // minibatch size
    constexpr float GAMMA = 0.99;           // discount factor
    constexpr float TAU = 1e-3;             // for soft update of target parameters
    constexpr float LEARNING_RATE = 5e-4;   // learning rate
    constexpr int UPDATE_EVERY = 4;         // how often to update the network
}

namespace lib::agent
{
    class Agent
    {
    public:
        QNetwork qnetwork_local;
        QNetwork qnetwork_target;
        torch::optim::Adam optimizer;

        Agent(int state_size, int action_size, int seed) : state_size_(state_size),
                                                           action_size_(action_size),
                                                           seed_(seed),
                                                           qnetwork_local(state_size, action_size),
                                                           qnetwork_target(state_size, action_size),
                                                           memory_(action_size, BUFFER_SIZE, BATCH_SIZE, seed),
                                                           optimizer(qnetwork_local.parameters(), torch::optim::AdamOptions(LEARNING_RATE))

        {
            t_step_ = 0;
        }

        // void Step(std::vector<float> &state, int action, float reward,
        //           std::vector<float> &next_state, bool done);

        // int Act(std::vector<float> &state, float eps = 0.0f);

        // void Learn(std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> &experiences, float gamma);

        // void SoftUpdate(QNetwork &local_model, QNetwork &target_model, float tau);

    private:
        int state_size_;
        int action_size_;
        int seed_;
        ReplayBuffer memory_;
        int t_step_;
    };
}