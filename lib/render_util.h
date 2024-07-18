#pragma once
#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"
#include <glog/logging.h>
#include <iostream>
#include <map>
#include <unordered_map>

namespace render_util {

const sf::Color agent_colors[] = {
    sf::Color::Red,     sf::Color::Green,  sf::Color::Blue, sf::Color::Cyan,
    sf::Color::Magenta, sf::Color::Yellow, sf::Color::White};

inline static sf::Text SetText(const sf::Font &font, const unsigned int &size,
                               const float &x, const float &y,
                               const sf::Color &color,
                               const sf::Text::Style &style) {
  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(size); // in pixel
  text.setPosition({x, y});
  text.setFillColor(color);
  text.setStyle(style);
  return text;
}

static void RenderEnvironment(
    sf::RenderWindow *window,
    const std::vector<std::shared_ptr<sf::CircleShape>> &circles,
    const std::vector<std::vector<sf::RectangleShape *>> &tile_grid) {
  // Update tiles.
  for (int row = 0; row < tile_grid.size(); row++) {
    for (int each_tile = 0; each_tile < tile_grid[row].size(); each_tile++) {
      window->draw(*tile_grid[row][each_tile]);
    }
  }

  // Update agents on top of tiles.
  for (auto circle : circles) {
    window->draw(*circle);
  }
}

static void UpdateAgentPixelLocation(
    const int &index, const std::pair<int, int> &coor,
    std::vector<std::shared_ptr<sf::CircleShape>> &circles,
    const std::vector<std::vector<sf::RectangleShape *>> &tile_grid) {
  if (index >= circles.size()) {
    LOG(ERROR) << "Agents index out of range of circles!";
    exit(1);
  }

  circles[index]->setPosition(
      tile_grid[coor.first][coor.second]->getPosition());
}

static void ClearCleanedTileState(
    std::vector<std::vector<sf::RectangleShape *>> &tile_grid) {
  for (auto &row : tile_grid) {
    for (auto &tile : row) {
      tile->setFillColor(sf::Color::Transparent);
    }
  }
}

static void
UpdateCleanedTile(std::map<std::pair<int, int>, bool> cleaned_tile_grid,
                  std::vector<std::vector<sf::RectangleShape *>> &tile_grid) {
  for (const auto &tile : cleaned_tile_grid) {
    if (tile.second) {
      tile_grid[tile.first.first][tile.first.second]->setFillColor(
          sf::Color(200, 200, 200, 120));
    }
  }
}
} // namespace render_util
