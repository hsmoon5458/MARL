#include "lib/agent.h"

namespace lib::agent
{
    void Agent::Step(std::vector<float> &state, int action, float reward, std::vector<float> &next_state, bool done)
    {
        // Save experience in replay memory
        memory_.Add(state, action, reward, next_state, done);

        // Learn every UPDATE_EVERY time steps.
        t_step_ = (t_step_ + 1) % UPDATE_EVERY;
        if (t_step_ == 0)
        {
            // If enough samples are available in memory, get random subset and learn
            if (memory_.GetLength() > BATCH_SIZE)
            {
                auto experiences_tuple = memory_.Sample();

                // Unpack the tuple into separate vectors
                auto &states = std::get<0>(experiences_tuple);
                auto &actions = std::get<1>(experiences_tuple);
                auto &rewards = std::get<2>(experiences_tuple);
                auto &next_states = std::get<3>(experiences_tuple);
                auto &dones = std::get<4>(experiences_tuple);
                // Convert 'dones' vector<bool> to vector<int>
                std::vector<int> dones_int;
                for (bool done : dones)
                {
                    dones_int.push_back(done ? 1 : 0);
                }

                std::vector<long long> states_size = {static_cast<long long>(states.size()), static_cast<long long>(state_size_)};

                std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> experiences_tensors{
                    torch::from_blob(states.data(), {static_cast<long long>(states.size()), static_cast<long long>(state_size_)}).to(device_),
                    torch::from_blob(actions.data(), {static_cast<long long>(actions.size()), 1}).to(device_),
                    torch::from_blob(rewards.data(), {static_cast<long long>(rewards.size()), 1}).to(device_),
                    torch::from_blob(next_states.data(), {static_cast<long long>(next_states.size()), static_cast<long long>(state_size_)}).to(device_),
                    torch::from_blob(dones_int.data(), {static_cast<long long>(dones_int.size()), 1}).to(device_)};

                // std::cout << "experiences_tensors: " << std::get<1>(experiences_tensors).sizes() << std::endl;
                Learn(experiences_tensors, GAMMA);
            }
        }
    }

    int Agent::Act(std::vector<float> &state, float eps)
    {
        // Convert state to a torch Tensor
        torch::Tensor state_tensor = torch::from_blob(state.data(), {1, state_size_}, torch::kFloat32).to(device_);
        state_tensor.unsqueeze_(0);

        // Set Q-network to evaluation mode and get action values
        qnetwork_local.eval();
        torch::NoGradGuard no_grad;
        torch::Tensor action_values = qnetwork_local.Forward(state_tensor);
        qnetwork_local.train();

        std::vector<int> possible_actions = GetPossibleAction(state);

        // Check if randomly generated number is greater than epsilon
        if (generate_random_number(0.0, 1.0) > eps)
        {
            // Exploitation: choose the best action among possible directions
            auto action_values_data = action_values.accessor<float, 2>();
            float max_value = std::numeric_limits<float>::lowest();
            int max_index = -1;

            for (int possible_action : possible_actions)
            {
                if (action_values_data[0][possible_action] > max_value)
                {
                    max_value = action_values_data[0][possible_action];
                    max_index = possible_action;
                }
            }

            return max_index;
        }
        else
        {
            // Generate a random number based on the possible pathways.
            int rand_num = generate_random_number(0, possible_actions.size() - 1);
            return possible_actions[rand_num];
        }
    }

    void Agent::Learn(std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> &experiences, float gamma)
    {
        // Obtain random minibatch of tuples from D
        auto &states = std::get<0>(experiences);
        auto &actions = std::get<1>(experiences);
        auto &rewards = std::get<2>(experiences);
        auto &next_states = std::get<3>(experiences);
        auto &dones = std::get<4>(experiences);

        // Convert actions to a PyTorch tensor
        torch::Tensor qValues = qnetwork_target.Forward(next_states);

        // Detach the tensor to prevent gradient tracking if needed
        qValues = qValues.detach();

        // Find the maximum values along dimension 1
        torch::Tensor max_values = std::get<0>(torch::max(qValues, 1));

        // Add a dimension of size 1 at position 1
        torch::Tensor q_targets_next = max_values.unsqueeze(1);

        // Calculate target value from Bellman equation
        torch::Tensor q_targets = rewards + gamma * q_targets_next * (1 - dones);

        // Forward pass through the network (replace 'your_network' with your actual network)
        torch::Tensor output = qnetwork_local.Forward(states); // Replace 'your_network' with your actual network

        // Perform the gather operation
        torch::Tensor q_expected;

        actions = actions.to(torch::kInt64);

        // Perform gather along dimension 1 using index 'actions'
        q_expected = output.gather(1, actions);

        auto loss = torch::mse_loss(q_expected, q_targets);

        optimizer.zero_grad();
        loss.backward();
        optimizer.step();

        // Update target network
        SoftUpdate(qnetwork_local, qnetwork_target, TAU);
    }

    void Agent::SoftUpdate(QNetwork &local_model, QNetwork &target_model, float tau)
    {
        auto local_params = local_model.parameters();
        auto target_params = target_model.parameters();

        for (size_t i = 0; i < local_params.size(); ++i)
        {
            target_params[i].data().copy_(tau * local_params[i].data() + (1.0 - tau) * target_params[i].data());
        }
    }

}