#pragma once

#include "torch/torch.h"
#include "qnetwork.h"
#include "replay_buffer.h"
#include "lib/utils.h"

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

        Agent(int id, int number_of_tile_per_line, int state_size, int action_size, int seed) : id_(id),
                                                                                                number_of_tile_per_line_(number_of_tile_per_line),
                                                                                                state_size_(state_size),
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

        int Act(std::vector<float> &state, float eps = 0.0f);

        // void Learn(std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> &experiences, float gamma);

        // void SoftUpdate(QNetwork &local_model, QNetwork &target_model, float tau);

    private:
        int state_size_;
        int action_size_;
        int seed_;
        ReplayBuffer memory_;
        int t_step_;

        int number_of_tile_per_line_;
        int id_;
        // To add the rule (do not select action that go outside of gird boundary),
        // parse the current location from the state.
        // Check calculate_state() in [tile_env.h] to get informed how state is defined.
        inline std::pair<int, int> get_coor_from_state(const std::vector<float> &state)
        {
            // state = {agent_1_x, agent_1_y, agent_2_x, agent_2_y, ... agent_n_x, agent_n_y, tiles...}
            if (id_ * 2 + 1 > state.size())
            {
                LOG(ERROR) << "State size is not matching with the agent size!";
                exit(1);
            }

            return std::make_pair((int)state[id_ * 2], (int)state[id_ * 2 + 1]);
        }
    };
}