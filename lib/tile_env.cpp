#include "tile_env.h"

namespace lib::tile_env {
TileEnvironment::TileEnvironment(int state_size, int action_size,
                                 int agent_size, int max_step,
                                 int number_of_tile_per_line)
    : state_size_(state_size), action_size_(action_size),
      agent_size_(agent_size), max_step_(max_step),
      number_of_tile_per_line_(number_of_tile_per_line) {

  // Update tile.
  for (int i = 0; i < number_of_tile_per_line_; i++) {
    for (int j = 0; j < number_of_tile_per_line_; j++) {
      // Setup the cleaned_map.
      cleaned_tile_grid_[std::make_pair(i, j)];
    }
  }

  // Update initial agent coors.
  for (int i = 0; i < agent_size_; i++) {
    agents_current_tile_grid_location_.push_back(agent_initial_coors_[i]);
  }

  total_reward_ = 0;
}

std::vector<float> TileEnvironment::Reset() {
  ClearCleanedTileState();

  // Reset agents location.
  for (int i = 0; i < agents_current_tile_grid_location_.size(); i++) {
    SetAgentTileGridLocation(i, agent_initial_coors_[i]);
  }

  // Reset the stat, state size will be determined by the number of agents times
  // x,y position. (2*n).
  std::vector<float> state;
  for (auto agent_current_tile_grid_location_ :
       agents_current_tile_grid_location_) {
    state.push_back(agent_current_tile_grid_location_.first);
    state.push_back(agent_current_tile_grid_location_.second);
  }

  // Set cleaned tile for agent initial location.
  for (int i = 0; i < agents_current_tile_grid_location_.size(); i++) {
    SetCleanedTile(agent_initial_coors_[i]);
  }

  total_reward_ = 0;
  return state;
}

void TileEnvironment::PerformAgentAction(const std::vector<int> &actions,
                                         float &reward) {
  for (int agent_index = 0; agent_index < agent_size_; agent_index++) {
    std::pair<int, int> new_coor = GetAgentCurrentTileGridLocation(agent_index);

    switch (actions[agent_index]) {
    case AgentMovement::AGENT_LEFT:
      if (CheckBoundaries({new_coor.first - 1, new_coor.second})) {
        new_coor.first--;
      } else {
        reward += -8;
      }
      break;

    case AgentMovement::AGENT_RIGHT:
      if (CheckBoundaries({new_coor.first + 1, new_coor.second})) {
        new_coor.first++;
      } else {
        reward += -8;
      }
      break;

    case AgentMovement::AGENT_UP:
      if (CheckBoundaries({new_coor.first, new_coor.second - 1})) {
        new_coor.second--;
      } else {
        reward += -8;
      }
      break;

    case AgentMovement::AGENT_DOWN:
      if (CheckBoundaries({new_coor.first, new_coor.second + 1})) {
        new_coor.second++;
      } else {
        reward += -8;
      }
      break;

    default:
      LOG(ERROR) << "Unidentified agent action performed: "
                 << actions[agent_index];
      break;
    }

    // Reward is updated on every agent since reward is based on
    // 'cleaned_tile', so each action is corresponding to 'cleaned_tile'.
    reward = GetRewardFromTileState(new_coor);
    // TODO: Add this to RewardPolicy.
    const auto time_penalty = -1;
    reward += time_penalty;

    SetAgentTileGridLocation(agent_index, new_coor);
    SetCleanedTile(new_coor);
  }
}

std::tuple<std::vector<float>, float, bool>
TileEnvironment::Step(const std::vector<int> &actions,
                      const int &current_step) {
  if (actions.size() != agent_size_) {
    LOG(ERROR) << "actions size is not equal to agent_size!";
  }

  // Execute all agents action and get the reward from the current step action.
  float reward = 0;
  bool done = false;

  PerformAgentAction(actions, reward);

  // If it reaches the max step, give large negative reward.
  if (current_step == max_step_ - 1 && !IsAllTilesCleaned()) {
    reward += -50;
    done = true;
  }

  // Check all tiles are cleaned.
  else if (IsAllTilesCleaned()) {
    reward += reward_policy_map[RewardPolicy::ALL_TILES_CLEANED];
    done = true;
  }

  total_reward_ += reward;

  // Update state, reward, and done state.
  std::vector<float> new_state = calculate_state();

  return std::make_tuple(new_state, reward, done);
}

float TileEnvironment::GetRewardFromTileState(const std::pair<int, int> &coor) {
  // If the tile is uncleaned,
  if (!cleaned_tile_grid_[coor]) {
    return reward_policy_map[RewardPolicy::TILE_NOT_CLEANED];
  } else if (cleaned_tile_grid_[coor]) {
    return reward_policy_map[RewardPolicy::TILE_ALREADY_CLEANED];
  }

  // TODO: Add more policy.

  return 0;
}
} // namespace lib::tile_env
