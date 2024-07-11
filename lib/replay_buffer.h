#pragma once
#include <vector>
#include <deque>
#include <random>
#include <algorithm>
#include <tuple>

struct Experience
{
    std::vector<float> state;
    int action;
    float reward;
    std::vector<float> next_state;
    bool done;
};

class ReplayBuffer
{
private:
    int action_size;
    int buffer_size;
    int batch_size;
    int seed;

    std::deque<Experience> memory_;

public:
    ReplayBuffer(int action_size, int buffer_size, int batch_size, int seed) : action_size(action_size),
                                                                               buffer_size(buffer_size),
                                                                               batch_size(batch_size),
                                                                               seed(seed)
    {
        // Initialize other variables and structures here...
    }

    void Add(std::vector<float> &state, int action, float reward, std::vector<float> &next_state, bool done)
    {
        Experience e;
        e.state = state;
        e.action = action;
        e.reward = reward;
        e.next_state = next_state;
        e.done = done;
        memory_.push_back(e);
    }

    std::tuple<std::vector<std::vector<float>>, std::vector<int>, std::vector<float>, std::vector<std::vector<float>>, std::vector<bool>> Sample()
    {
        std::vector<std::vector<float>> states;
        std::vector<int> actions;
        std::vector<float> rewards;
        std::vector<std::vector<float>> next_states;
        std::vector<bool> dones;

        std::vector<size_t> indices(memory_.size());
        std::iota(indices.begin(), indices.end(), 0); // Fill indices with 0, 1, ..., memory_.size()-1

        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(indices.begin(), indices.end(), gen); // Shuffle indices

        for (int i = 0; i < batch_size; ++i)
        {
            int index = indices[i];
            states.push_back(memory_[index].state);
            actions.push_back(memory_[index].action);
            rewards.push_back(memory_[index].reward);
            next_states.push_back(memory_[index].next_state);
            dones.push_back(memory_[index].done);
            // std::cout << "index, i: " << index << ", " << i << std::endl;
        }

        return std::make_tuple(states, actions, rewards, next_states, dones);
    }

    int GetLentgh()
    {
        return memory_.size();
    }
};