#pragma once

#include "lib/utils.h"
#include "qnetwork.h"
#include "replay_buffer.h"
#include "torch/torch.h"

#include <glog/logging.h>
#include <memory>

namespace {
constexpr float BUFFER_SIZE = int(1e5); // replay buffer size
constexpr int BATCH_SIZE = 64;          // minibatch size
constexpr float GAMMA = 0.99;           // discount factor
constexpr float TAU = 0.001;            // for soft update of target parameters
constexpr float LEARNING_RATE = 0.0007; // learning rate
constexpr int UPDATE_EVERY = 4;         // how often to update the network
} // namespace

namespace lib::agent {
class Agent {
public:
  QNetwork qnetwork_local;
  QNetwork qnetwork_target;
  torch::optim::Adam optimizer;

  Agent(int id, int number_of_tile_per_line, int state_size, int action_size,
        torch::Device device)
      : id_(id), number_of_tile_per_line_(number_of_tile_per_line),
        state_size_(state_size), action_size_(action_size), device_(device),
        qnetwork_local(state_size, action_size, device),
        qnetwork_target(state_size, action_size, device),
        memory_(action_size, BUFFER_SIZE, BATCH_SIZE),
        optimizer(qnetwork_local.parameters(),
                  torch::optim::AdamOptions(LEARNING_RATE))

  {
    t_step_ = 0;
    device_ = device;
  }

  void Step(std::vector<float> &state, int action, float reward,
            std::vector<float> &next_state, bool done);

  int Act(std::vector<float> &state, float eps);

  void Learn(std::tuple<torch::Tensor, torch::Tensor, torch::Tensor,
                        torch::Tensor, torch::Tensor> &experiences,
             float gamma);

  void SoftUpdate(QNetwork &local_model, QNetwork &target_model, float tau);

  float GetMSELossValue() { return loss_value_; }

private:
  int state_size_;
  int action_size_;
  torch::Device device_;
  ReplayBuffer memory_;
  int t_step_;

  float loss_value_;

  int number_of_tile_per_line_;
  int id_;
  // To add the rule (do not select action that go outside of gird boundary),
  // parse the current location from the state.
  // Check calculate_state() in [tile_env.h] to get informed how state is
  // defined.
  inline std::pair<int, int>
  get_coor_from_state(const std::vector<float> &state) {
    // state = {agent_1_x, agent_1_y, agent_2_x, agent_2_y, ... agent_n_x,
    // agent_n_y, tiles...}
    if (id_ * 2 + 1 > state.size()) {
      LOG(ERROR) << "State size is not matching with the agent size!";
      exit(1);
    }

    return std::make_pair((int)state[id_ * 2], (int)state[id_ * 2 + 1]);
  }
};
} // namespace lib::agent