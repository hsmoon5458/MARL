#pragma once

#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"
#include "lib/agent.h"
#include "lib/utils.h"

#include <glog/logging.h>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <unordered_map>

#define NUMBER_OF_TILES_PER_LINE 20

namespace {
constexpr auto opacity = 120;
struct color_hash {
  std::size_t operator()(const sf::Color &color) const {
    return std::hash<int>()(color.r) ^ std::hash<int>()(color.g) ^
           std::hash<int>()(color.b) ^ std::hash<int>()(color.a);
  }
};

// TODO: Add more color for more agents.
// To distinguish between agent and cleaned tile.
std::unordered_map<sf::Color, sf::Color, color_hash>
    color_tile_grid_for_cleaned_tile{
        {sf::Color::Red, sf::Color(200, 0, 0, opacity)},
        {sf::Color::Green, sf::Color(0, 200, 0, opacity)},
        {sf::Color::Blue, sf::Color(0, 0, 200, opacity)},
        {sf::Color::Cyan, sf::Color(0, 200, 200, opacity)},
        {sf::Color::Magenta, sf::Color(90, 0, 90, opacity)},
        {sf::Color::Yellow, sf::Color(200, 200, 0, opacity)},
        {sf::Color::White, sf::Color(200, 200, 200, opacity)},
    };

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
  TileEnvironment(int state_size, int action_size, int agent_size,
                  int display_width, int display_height,
                  int number_of_tile_per_line = NUMBER_OF_TILES_PER_LINE);

  std::vector<std::vector<sf::RectangleShape *>> GetTileVector() {
    return tile_grid_;
  }

  sf::RectangleShape *GetTile(const std::pair<int, int> &location) {
    return tile_grid_[location.first][location.second];
  }

  // Return the pixel position from the tile coordinate.
  sf::Vector2f GetTilePixelPosition(const std::pair<int, int> &location) {
    return tile_grid_[location.first][location.second]->getPosition();
  }

  // Return coordinate of current agent's tile grid location (e.g. 13,4).
  std::pair<int, int> GetAgentCurrentTileGridLocation(const int &index) {
    if (index >= agents_current_tile_grid_location_.size()) {
      LOG(ERROR)
          << "Agents index out of range of agents_current_tile_grid_location_!";
      exit(1);
    }
    return agents_current_tile_grid_location_[index];
  }

  void UpdateAgentTileGridLocation(const int &index,
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

  void UpdateAgentPixelLocation(const int &index,
                                const sf::Vector2f &location) {
    if (index >= circles_.size()) {
      LOG(ERROR) << "Agents index out of range of circles!";
      exit(1);
    }

    circles_[index]->setPosition(location);
  }

  void ClearCleanedTileState() {
    for (auto &tile : cleaned_tile_grid_) {
      tile.second = false;
    }
    for (auto &row : tile_grid_) {
      for (auto &tile : row) {
        tile->setFillColor(sf::Color::Transparent);
      }
    }
  }

  bool GetAllTilesCleaned() {
    for (const auto &each_tile : cleaned_tile_grid_) {
      if (!each_tile.second)
        return false;
    }
    return true;
  }

  void UpdateCleanedTile(const std::pair<int, int> &location,
                         const sf::Color &color) {
    try {
      cleaned_tile_grid_[location] = true;
      // Only update the tile color when it is transparent.
      if (tile_grid_[location.first][location.second]->getFillColor() ==
          sf::Color::Transparent) {
        tile_grid_[location.first][location.second]->setFillColor(
            color_tile_grid_for_cleaned_tile[color]);
      }
    } catch (const std::exception &e) {
      LOG(ERROR) << e.what();
    }
  }

  void RenderEnvironment(sf::RenderWindow *window) {
    // Update tiles.
    for (int row = 0; row < number_of_tile_per_line_; row++) {
      for (int each_tile = 0; each_tile < number_of_tile_per_line_;
           each_tile++) {
        window->draw(*GetTile({row, each_tile}));
      }
    }

    // Update agents on top of tiles.
    for (auto circle : circles_) {
      window->draw(*circle);
    }
  }

  // Generate random action that avoid boundaries and obstacles.
  int GenerateAgentRandomAction(const std::pair<int, int> location);

  // Update all agents' tile_grid locaiton, pixel location, claned tile, and
  // reward.
  void PerformAgentAction(const std::vector<int> &actions, float &reward);

  std::tuple<std::vector<float>, float, bool>
  Step(const std::vector<int> &actions);

  std::vector<float> Reset();

  ~TileEnvironment() {
    for (auto row : tile_grid_) {
      for (auto each : row) {
        delete each;
      }
    }
  }

  // To update circles position.
  std::vector<std::shared_ptr<sf::CircleShape>> GetCircles() {
    return circles_;
  }

private:
  int state_size_;
  int action_size_;
  int agent_size_;

  // Tile gird
  std::map<std::pair<int, int>, bool> cleaned_tile_grid_;
  int number_of_tile_per_line_;
  sf::Vector2f center_offset_;
  std::vector<std::vector<sf::RectangleShape *>> tile_grid_;
  std::vector<std::shared_ptr<sf::CircleShape>> circles_;

  // Agents
  std::vector<std::pair<int, int>> agents_current_tile_grid_location_;

  float total_reward_;

  // Calculate the state based on the number of agent and tile grid setting.
  std::vector<float> calculate_state() {
    std::vector<float> current_state;

    // Append agents coordinate.
    for (int agent_index = 0; agent_index < agent_size_; agent_index++) {
      current_state.push_back(
          agents_current_tile_grid_location_[agent_index].first);
      current_state.push_back(
          agents_current_tile_grid_location_[agent_index].second);
    }

    // Append cleaned tile state (bool).
    for (const auto &tile : cleaned_tile_grid_) {
      current_state.push_back(tile.second);
    }

    if (current_state.size() != state_size_) {
      LOG(ERROR) << "State size mismatch!";
      exit(1);
    }

    return current_state;
  }

  // <reward_category, reward_score>
  std::map<int, float> reward_policy_map = {
      {RewardPolicy::TILE_ALREADY_CLEANED, -1},
      {RewardPolicy::TILE_NOT_CLEANED, 1},
      {RewardPolicy::ALL_TILES_CLEANED, 1}};

  // Get the reward based on coordinate and tile's cleaned state.
  float GetRewardFromTileState(const std::pair<int, int> &coor);

  // Agents initial coordinates.
  std::map<int, std::pair<int, int>> agent_initial_coors_{
      {0, {0, 0}},
      {1, {number_of_tile_per_line_ - 1, 0}},
      {2, {0, number_of_tile_per_line_ - 1}},
      {3, {number_of_tile_per_line_, number_of_tile_per_line_ - 1}},
  };
};
} // namespace lib::tile_env