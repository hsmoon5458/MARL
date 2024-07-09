#pragma once

#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"
#include "lib/agent.h"
#include "lib/utils.h"
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <random>

namespace
{
	constexpr auto opacity = 120;
	struct color_hash
	{
		std::size_t operator()(const sf::Color &color) const
		{
			return std::hash<int>()(color.r) ^ std::hash<int>()(color.g) ^ std::hash<int>()(color.b) ^ std::hash<int>()(color.a);
		}
	};

	// TODO: Add more color for more agents.
	// To distinguish agent and cleaned tile.
	std::unordered_map<sf::Color, sf::Color, color_hash> color_tile_grid_for_cleaned_tile{
		{sf::Color::Red, sf::Color(200, 0, 0, opacity)},
		{sf::Color::Green, sf::Color(0, 200, 0, opacity)},
		{sf::Color::Blue, sf::Color(0, 0, 200, opacity)},
		{sf::Color::Cyan, sf::Color(0, 200, 200, opacity)},
		{sf::Color::Magenta, sf::Color(90, 0, 90, opacity)},
		{sf::Color::Yellow, sf::Color(200, 200, 0, opacity)},
		{sf::Color::White, sf::Color(200, 200, 200, opacity)},
	};
}

namespace lib::tile_env
{
	class TileEnvironment
	{
	public:
		int state_size;
		int action_size;

		TileEnvironment(int display_width, int display_height, int state_size, int action_size,
						std::vector<lib::agent::Agent *> agents, std::vector<std::pair<int, int>> initial_agents_coor, int number_of_tile_per_line = 20);

		std::vector<std::vector<sf::RectangleShape *>> GetTileVector()
		{
			return tile_grid_;
		}

		sf::RectangleShape *GetTile(const std::pair<int, int> &location)
		{
			return tile_grid_[location.first][location.second];
		}

		// Return the pixel position from the tile coordinate.
		sf::Vector2f GetTilePixelPosition(const std::pair<int, int> &location)
		{
			return tile_grid_[location.first][location.second]->getPosition();
		}

		std::pair<int, int> GetAgentCurrentTileGridLocation(const int &index)
		{
			if (index >= agents_current_tile_grid_location_.size())
			{
				std::cout << "Agents index out of range of agents_current_tile_grid_location_!" << std::endl;
				exit(1);
			}
			return agents_current_tile_grid_location_[index];
		}

		int GetAgentsSize()
		{
			return agents_.size();
		}

		void UpdateAgentTileGridLocation(const int &index, const std::pair<int, int> &location)
		{
			if (index >= agents_current_tile_grid_location_.size())
			{
				std::cout << "Agents index out of range of agents_current_tile_grid_location_!" << std::endl;
				exit(1);
			}
			agents_current_tile_grid_location_[index] = location;
		}

		void UpdateAgentPixelLocation(const int &index, const sf::Vector2f &location)
		{
			if (index >= circles.size())
			{
				std::cout << "Agents index out of range of circles!" << std::endl;
				exit(1);
			}
			circles[index]->setPosition(location);
		}

		void ClearCleanedTileState()
		{
			for (auto &tile : cleaned_tile_grid_)
			{
				tile.second = false;
			}
			for (auto &row : tile_grid_)
			{
				for (auto &tile : row)
				{
					tile->setFillColor(sf::Color::Transparent);
				}
			}
		}

		bool AllTilesCleaned()
		{
			for (const auto &each_tile : cleaned_tile_grid_)
			{
				if (!each_tile.second)
					return false;
			}
			return true;
		}

		void UpdateCleanedTile(const std::pair<int, int> &location, const sf::Color &color)
		{
			try
			{
				cleaned_tile_grid_[location] = true;
				// Only update the tile color when it is transparent.
				if (tile_grid_[location.first][location.second]->getFillColor() == sf::Color::Transparent)
				{
					tile_grid_[location.first][location.second]->setFillColor(color_tile_grid_for_cleaned_tile[color]);
				}
			}
			catch (const std::exception &e)
			{
				// print the exception
				std::cout << "Exception " << e.what() << std::endl;
			}
		}

		std::vector<float> Reset();

		~TileEnvironment()
		{
			for (auto row : tile_grid_)
			{
				for (auto each : row)
				{
					delete each;
				}
			}
		}

		// Make shared_ptr and public to easily modfiy the position.
		std::vector<std::shared_ptr<sf::CircleShape>> circles;

	private:
		// Tile gird
		std::map<std::pair<int, int>, bool> cleaned_tile_grid_;
		int number_of_tile_per_line_;
		sf::Vector2f center_offset_;
		std::vector<std::vector<sf::RectangleShape *>> tile_grid_;

		// Agents
		std::vector<lib::agent::Agent *> agents_;
		std::vector<std::pair<int, int>> agents_current_tile_grid_location_;

		float reward_;
	};
}