#include "lib/agent.h"

namespace lib::agent {
void Agent::Step(std::vector<float> &state, int action, float reward,
                 std::vector<float> &next_state, bool done) {
  // Save experience in replay memory
  memory_.Add(state, action, reward, next_state, done);
  // Learn every UPDATE_EVERY time steps.
  t_step_ = (t_step_ + 1) % UPDATE_EVERY;
  if (t_step_ == 0) {
    // If enough samples are available in memory, get random subset and learn
    if (memory_.GetLength() > BATCH_SIZE) {
      auto experiences_tuple = memory_.Sample();

      // Unpack the tuple into separate vectors
      auto &states = std::get<0>(experiences_tuple);
      auto &actions = std::get<1>(experiences_tuple);
      auto &rewards = std::get<2>(experiences_tuple);
      auto &next_states = std::get<3>(experiences_tuple);
      auto &dones = std::get<4>(experiences_tuple);
      // Convert 'dones' vector<bool> to vector<int>
      std::vector<int> dones_int;
      for (bool done : dones) {
        dones_int.push_back(done ? 1 : 0);
      }

      std::vector<float> flattened_states;
      for (const auto &state : next_states) {
        flattened_states.insert(flattened_states.end(), state.begin(),
                                state.end());
      }

      std::vector<int64_t> actions_int64(actions.begin(), actions.end());
      std::vector<float> flattened_next_states;
      for (const auto &state : next_states) {
        flattened_next_states.insert(flattened_next_states.end(), state.begin(),
                                     state.end());
      }

      std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor,
                 torch::Tensor>
          experiences_tensors{
              torch::from_blob(flattened_states.data(),
                               {static_cast<long long>(states.size()),
                                static_cast<long long>(state_size_)},
                               torch::kFloat32)
                  .to(device_),
              torch::from_blob(
                  actions_int64.data(),
                  {static_cast<long long>(actions_int64.size()), 1},
                  torch::kInt64)
                  .to(device_),
              torch::from_blob(rewards.data(),
                               {static_cast<long long>(rewards.size()), 1},
                               torch::kFloat32)
                  .to(device_),
              torch::from_blob(flattened_next_states.data(),
                               {static_cast<long long>(next_states.size()),
                                static_cast<long long>(state_size_)},
                               torch::kFloat32)
                  .to(device_),
              torch::from_blob(dones_int.data(),
                               {static_cast<long long>(dones_int.size()), 1},
                               torch::kFloat32)
                  .to(device_)};

      std::cout << "states in Step before processing:\n" << states << std::endl;
      std::cout << "states in Step after processsing:\n"
                << std::get<0>(experiences_tensors) << std::endl;

      std::cout << "action in Step before processing:\n"
                << actions << std::endl;
      std::cout << "action in Step after processsing:\n"
                << std::get<1>(experiences_tensors) << std::endl;

      std::cout << "rewards in Step before processing:\n"
                << rewards << std::endl;
      std::cout << "rewards in Step after processsing:\n"
                << std::get<2>(experiences_tensors) << std::endl;

      std::cout << "next_states in Step before processing:\n"
                << next_states << std::endl;
      std::cout << "next_states in Step after processsing:\n"
                << std::get<3>(experiences_tensors) << std::endl;

      std::cout << "dones in Step before processing:\n" << dones << std::endl;
      std::cout << "dones in Step after processsing:\n"
                << std::get<4>(experiences_tensors) << std::endl;

      Learn(experiences_tensors, GAMMA);
    }
  }
}

int Agent::Act(std::vector<float> &state, float eps) {
  // Convert state to a torch Tensor
  torch::Tensor state_tensor =
      torch::from_blob(state.data(), {1, state_size_}, torch::kFloat32)
          .to(device_);

  // Set Q-network to evaluation mode and get action values
  qnetwork_local.eval();
  torch::Tensor action_values;
  {
    torch::NoGradGuard no_grad;
    action_values = qnetwork_local.forward(state_tensor);
  }
  qnetwork_local.train();

  int selected_action;

  // Check if randomly generated number is greater than epsilon
  if (generate_random_number(0.0, 1.0) > eps) {
    // Exploitation: choose the best action
    selected_action = action_values.argmax(1).item<int>();
  } else {
    // Exploration: choose a random action
    selected_action = generate_random_number(0, action_size_ - 1);
  }

  return selected_action;
}

void Agent::Learn(std::tuple<torch::Tensor, torch::Tensor, torch::Tensor,
                             torch::Tensor, torch::Tensor> &experiences,
                  float gamma) {
  // Obtain random minibatch of tuples from D
  auto &states = std::get<0>(experiences);
  auto &actions = std::get<1>(experiences);
  auto &rewards = std::get<2>(experiences);
  auto &next_states = std::get<3>(experiences);
  auto &dones = std::get<4>(experiences);

  // DEBUG LOG
  std::cout << "In Learn State" << std::endl;

  const char *tensor_names[] = {"states", "actions", "rewards", "next_states",
                                "dones"};
  int tensor_index = 0;
  for (const auto &tensor : {states, actions, rewards, next_states, dones}) {
    bool has_nan = torch::any(torch::isnan(tensor)).item<bool>();
    bool has_inf = torch::any(torch::isinf(tensor)).item<bool>();
    if (has_nan || has_inf) {
      std::cout << "Error detected in tensor:\n"
                << tensor_names[tensor_index] << std::endl;
      exit(1);
    }
    tensor_index++;
  }
  // DEBUG LOG END

  // Convert actions to a PyTorch tensor
  torch::Tensor qValues;
  {
    torch::NoGradGuard no_grad;
    qValues = qnetwork_target.forward(next_states);
  }

  std::cout << "qValues:\n" << qValues << std::endl;

  // Find the maximum values along dimension 1
  torch::Tensor max_values = std::get<0>(torch::max(qValues, 1));

  // Add a dimension of size 1 at position 1
  torch::Tensor q_targets_next = max_values.unsqueeze(1);

  // Calculate target value from Bellman equation
  torch::Tensor q_targets =
      (rewards + gamma * q_targets_next * (1 - dones)).detach();

  // Forward pass through the network
  torch::Tensor output = qnetwork_local.forward(states);

  std::cout << "output:\n" << output << std::endl;

  // Perform the gather operation
  torch::Tensor q_expected;

  actions = actions.to(torch::kInt64);

  // Perform gather along dimension 1 using index 'actions'
  q_expected = output.gather(1, actions);

  auto loss = torch::mse_loss(q_expected, q_targets);
  // auto loss = torch::nn::functional::huber_loss(q_expected, q_targets);

  loss_value_ = loss.item<float>();

  optimizer.zero_grad();
  loss.backward();

  torch::nn::utils::clip_grad_norm_(qnetwork_local.parameters(), 1.0);

  optimizer.step();

  // Update target network
  SoftUpdate(qnetwork_local, qnetwork_target, TAU);
}

void Agent::SoftUpdate(QNetwork &local_model, QNetwork &target_model,
                       float tau) {
  torch::NoGradGuard no_grad;
  auto local_params = local_model.parameters();
  auto target_params = target_model.parameters();

  for (size_t i = 0; i < local_params.size(); ++i) {
    target_params[i].copy_(tau * local_params[i] +
                           (1.0 - tau) * target_params[i]);
  }
}

} // namespace lib::agent