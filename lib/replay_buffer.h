#pragma once

#include "lib/utils.h"

#include <algorithm>
#include <deque>
#include <random>
#include <tuple>
#include <vector>

struct Experience {
  std::vector<float> state;
  int action;
  float reward;
  std::vector<float> next_state;
  bool done;
};

class ReplayBuffer {
private:
  int action_size;
  int buffer_size;
  int batch_size;
  int seed;

  std::deque<Experience> memory_;

public:
  ReplayBuffer(int action_size, int buffer_size, int batch_size, int seed)
      : action_size(action_size), buffer_size(buffer_size),
        batch_size(batch_size), seed(seed), memory_() {
    // Initialize other variables and structures here...
  }

  void Add(std::vector<float> &state, int action, float reward,
           std::vector<float> &next_state, bool done) {
    if (memory_.size() >= buffer_size) {
      memory_.pop_front();
    }
    Experience experience{state, action, reward, next_state, done};
    memory_.push_back(experience);
  }

  std::tuple<std::vector<std::vector<float>>, std::vector<int>,
             std::vector<float>, std::vector<std::vector<float>>,
             std::vector<bool>>
  Sample() {
    std::vector<std::vector<float>> states;
    std::vector<int> actions;
    std::vector<float> rewards;
    std::vector<std::vector<float>> next_states;
    std::vector<bool> dones;

    int sample_size = std::min(static_cast<int>(memory_.size()), batch_size);

    std::vector<size_t> indices(memory_.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::random_device rd;
    std::mt19937 gen(seed != 0 ? seed : rd());
    std::shuffle(indices.begin(), indices.end(), gen);

    for (int i = 0; i < sample_size; ++i) {
      int index = indices[i];
      states.push_back(memory_[index].state);
      actions.push_back(memory_[index].action);
      rewards.push_back(memory_[index].reward);
      next_states.push_back(memory_[index].next_state);
      dones.push_back(memory_[index].done);
    }

    return std::make_tuple(states, actions, rewards, next_states, dones);
  }

  int GetLength() { return memory_.size(); }
};