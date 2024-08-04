#pragma once

#include "lib/agent.h"
#include "lib/utils.h"

#include <glog/logging.h>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>

namespace {

enum AgentMovement {
  AGENT_LEFT,
  AGENT_RIGHT,
  AGENT_UP,
  AGENT_DOWN,
  // AGENT_HOLD,
};

enum RewardPolicy {
  TILE_ALREADY_CLEANED,
  TILE_NOT_CLEANED,
  ALL_TILES_CLEANED,
};
} // namespace

namespace lib::tile_env {
class TileEnvironment {
  // TODO: Move public method to private if it not used in main function.
public:
  TileEnvironment(int state_size, int action_size, int agent_size, int max_step,
                  int number_of_tile_per_line);

  // Return coordinate of current agent's tile grid location (e.g. 13,4).
  std::pair<int, int> GetAgentCurrentTileGridLocation(const int &index) {
    if (index >= agents_current_tile_grid_location_.size()) {
      LOG(ERROR)
          << "Agents index out of range of agents_current_tile_grid_location_!";
      exit(1);
    }
    return agents_current_tile_grid_location_[index];
  }

  void SetAgentTileGridLocation(const int &index,
                                const std::pair<int, int> &location) {
    if (index >= agents_current_tile_grid_location_.size()) {
      LOG(ERROR)
          << "Agents index out of range of agents_current_tile_grid_location_!";
      exit(1);
    }

    if (location.first < 0 || location.first >= number_of_tile_per_line_ ||
        location.second < 0 || location.second >= number_of_tile_per_line_) {
      LOG(ERROR) << "Location is out of tile boundary.";
      exit(1);
    }

    agents_current_tile_grid_location_[index] = location;
  }

  // Return false if the coordinate is out of boundaries.
  bool CheckBoundaries(const std::pair<int, int> coor) {
    if (coor.first < 0 || coor.second < 0 ||
        coor.first >= number_of_tile_per_line_ ||
        coor.second >= number_of_tile_per_line_) {
      return false;
    }
    return true;
  }

  void ClearCleanedTileState() {
    for (auto &tile : cleaned_tile_grid_) {
      tile.second = false;
    }
  }

  bool IsAllTilesCleaned() {
    for (const auto &each_tile : cleaned_tile_grid_) {
      if (!each_tile.second)
        return false;
    }
    return true;
  }

  void SetCleanedTile(const std::pair<int, int> &location) {
    cleaned_tile_grid_[location] = true;
  }

  std::map<std::pair<int, int>, bool> GetCleanedTileGrid() {
    return cleaned_tile_grid_;
  }

  // Update all agents' tile_grid locaiton, pixel location, claned tile, and
  // reward.
  void PerformAgentAction(const std::vector<int> &actions, float &reward);

  std::tuple<std::vector<float>, float, bool>
  Step(const std::vector<int> &actions, const int &current_step);

  std::vector<float> Reset();

  ~TileEnvironment() {}

private:
  int state_size_;
  int action_size_;
  int agent_size_;
  int max_step_;

  // Tile gird
  std::map<std::pair<int, int>, bool> cleaned_tile_grid_;
  int number_of_tile_per_line_;

  // Agents
  std::vector<std::pair<int, int>> agents_current_tile_grid_location_;

  float total_reward_;

  // Calculate the state based on the number of agent and tile grid setting.
  std::vector<float> calculate_state() {
    std::vector<float> current_state;

    // Append agents coordinate.
    for (int agent_index = 0; agent_index < agent_size_; agent_index++) {
      float normalized_x =
          agents_current_tile_grid_location_[agent_index].first /
          (float)number_of_tile_per_line_;
      float normalized_y =
          agents_current_tile_grid_location_[agent_index].second /
          (float)number_of_tile_per_line_;

      current_state.push_back(normalized_x);
      current_state.push_back(normalized_y);
    }

    // Append cleaned state of each tile.
    for (const auto &tile : cleaned_tile_grid_) {
      current_state.push_back(tile.second ? 1.0f : 0.0f);
    }

    if (current_state.size() != state_size_) {
      LOG(ERROR) << "State size mismatch:" << current_state.size() << " "
                 << state_size_;

      exit(1);
    }

    return current_state;
  }

  // <reward_category, reward_score>
  std::map<int, float> reward_policy_map = {
      {RewardPolicy::TILE_ALREADY_CLEANED, -5},
      {RewardPolicy::TILE_NOT_CLEANED, 5},
      {RewardPolicy::ALL_TILES_CLEANED, 10}};

  // Get the reward based on coordinate and tile's cleaned state.
  float GetRewardFromTileState(const std::pair<int, int> &coor);

  std::map<int, std::pair<int, int>> agent_initial_coors_{
      {0, {0, 0}},
      {1, {number_of_tile_per_line_ - 1, 0}},
      {2, {0, number_of_tile_per_line_ - 1}},
      {3, {number_of_tile_per_line_, number_of_tile_per_line_ - 1}},
  };
};
} // namespace lib::tile_env