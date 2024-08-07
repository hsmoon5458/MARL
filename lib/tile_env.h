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

  int GetNumberOfUncleanedTile() {
    int count = 0;
    for (const auto &each_tile : cleaned_tile_grid_) {
      if (!each_tile.second)
        ++count;
    }
    return count;
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
    const int NUM_NEARBY_TILES =
        4; // For example, track the 4 nearest uncleaned tiles
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

      std::vector<std::pair<float, float>> nearby_uncleaned_tiles;
      int agent_x = agents_current_tile_grid_location_[agent_index].first;
      int agent_y = agents_current_tile_grid_location_[agent_index].second;

      for (const auto &tile : cleaned_tile_grid_) {
        if (!tile.second) { // If the tile is uncleaned
          int tile_x = tile.first.first;
          int tile_y = tile.first.second;
          float dx = (tile_x - agent_x) /
                     (float)number_of_tile_per_line_; // Normalized relative x
          float dy = (tile_y - agent_y) /
                     (float)number_of_tile_per_line_; // Normalized relative y
          nearby_uncleaned_tiles.push_back({dx, dy});
        }
      }

      // Sort by distance and take the nearest NUM_NEARBY_TILES
      std::sort(nearby_uncleaned_tiles.begin(), nearby_uncleaned_tiles.end(),
                [](const auto &a, const auto &b) {
                  return (a.first * a.first + a.second * a.second) <
                         (b.first * b.first + b.second * b.second);
                });

      nearby_uncleaned_tiles.resize(NUM_NEARBY_TILES,
                                    {0.0f, 0.0f}); // Pad with (0,0) if needed

      for (const auto &tile : nearby_uncleaned_tiles) {
        current_state.push_back(tile.first);
        current_state.push_back(tile.second);
      }
    }

    // Append cleaned state of each tile.
    for (const auto &tile : cleaned_tile_grid_) {
      current_state.push_back(tile.second ? 1.0f : 0.0f);
    }

    current_state.push_back(GetNumberOfUncleanedTile() /
                            number_of_tile_per_line_ *
                            number_of_tile_per_line_);

    if (current_state.size() != state_size_) {
      LOG(ERROR) << "State size mismatch:" << current_state.size() << " "
                 << state_size_;

      exit(1);
    }

    return current_state;
  }

  //   Relative Position:
  // Instead of absolute coordinates, consider using the relative position of
  // the agent to the center of the grid or to the nearest uncleaned tile. This
  // can help the agent generalize better. Distance to Nearest Uncleaned Tile:
  // Include the distance to the nearest uncleaned tile. This can provide more
  // direct guidance for the agent. Directional Information: Instead of a flat
  // 5x5 grid, consider encoding the state of tiles in different directions (up,
  // down, left, right, diagonals) relative to the agent. Count of Uncleaned
  // Tiles: Include a count or percentage of uncleaned tiles. This can give the
  // agent a sense of overall progress. Recent Actions: Consider including the
  // agent's last action or a short history of actions. This can help the agent
  // learn action sequences. Encoding Cleaned State: Instead of boolean values,
  // you could use a single integer to represent the number of cleaned tiles in
  // each row and column.

  // <reward_category, reward_score>
  std::map<int, float> reward_policy_map = {
      {RewardPolicy::TILE_ALREADY_CLEANED, -5},
      {RewardPolicy::TILE_NOT_CLEANED, 5},
      {RewardPolicy::ALL_TILES_CLEANED, 20}};

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