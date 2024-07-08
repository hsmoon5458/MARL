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
		{sf::Color::Blue, sf::Color(0, 0, 200, opacity)}};
}

namespace lib::tile_env
{
	class TileEnvironment
	{
	public:
		int state_size;
		int action_size;

		TileEnvironment(int display_width, int display_height, int state_size, int action_size,
						std::vector<lib::agent::Agent *> agents, std::vector<std::pair<int, int>> initial_agents_coor, int number_of_tile_per_line = 20)
			: number_of_tile_per_line_(number_of_tile_per_line),
			  state_size(state_size),
			  action_size(action_size),
			  agents_(agents)
		{
			// GUI tile gird setup.
			const float line_thickness = 2;
			const float tile_size = 100;

			center_offset_ = {display_width / 2 - (tile_size * number_of_tile_per_line) / 2, display_height / 2 - (tile_size * number_of_tile_per_line) / 2};

			for (int i = 0; i < number_of_tile_per_line; i++)
			{
				std::vector<sf::RectangleShape *> row;
				for (int j = 0; j < number_of_tile_per_line; j++)
				{
					sf::RectangleShape *tile = new sf::RectangleShape;
					tile->setSize({tile_size, tile_size});
					tile->setOutlineThickness(line_thickness);
					tile->setOutlineColor(sf::Color::White);
					tile->setFillColor(sf::Color::Transparent);
					tile->setPosition({(tile_size + line_thickness) * i + center_offset_.x, (tile_size + line_thickness) * j + center_offset_.y});
					row.push_back(tile);

					// Setup the cleaned_map.
					cleaned_tile_grid_[std::make_pair(i, j)];
				}
				tile_grid_.push_back(row);
			}

			// GUI Agents Setup.
			const float agent_outline_thickness = 1;
			float radius = 50;

			int agents_size = agents.size();
			if (agents_size != initial_agents_coor.size())
			{
				std::cout << "Initial Coordinate for agents and agent pointers size mismatch!" << std::endl;
				exit(1);
			}

			for (int i = 0; i < agents_size; i++)
			{
				auto circle = std::make_shared<sf::CircleShape>();
				circle->setRadius(radius);
				circle->setPosition(GetTilePixelPosition(initial_agents_coor[i]));
				circle->setOutlineColor(sf::Color::White);
				circle->setOutlineThickness(agent_outline_thickness);
				agents_current_tile_grid_location_.push_back(initial_agents_coor[i]);
				circles.push_back(circle);
			}

			reward_ = 0;
		}

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

		std::vector<float> Reset()
		{
			ClearCleanedTileState();

			// Reset agents location.
			for (int i = 0; i < agents_current_tile_grid_location_.size(); i++)
			{
				auto new_coor = std::make_pair(generate_random_number(0, number_of_tile_per_line_ - 1),
											   generate_random_number(0, number_of_tile_per_line_ - 1));

				UpdateAgentTileGridLocation(i, new_coor);
				UpdateAgentPixelLocation(i, GetTilePixelPosition(new_coor));
			}

			// Reset the stat, state size will be determined by the number of agents times x,y position. (2*n).
			std::vector<float> state;
			for (auto agent_current_tile_grid_location_ : agents_current_tile_grid_location_)
			{
				state.push_back(agent_current_tile_grid_location_.first);
				state.push_back(agent_current_tile_grid_location_.second);
			}

			reward_ = 0.0f;
			return state;
		}

		// // XX, reward , done
		// std::tuple<std::vector<float>, float, bool> Step(Action action, int range)
		// {
		// 	lander.performAction(action);
		// 	leftLeg.updatePosition(lander.getBase().getPosition().x - 5.0f, lander.getBase().getPosition().y + 40.0f);
		// 	rightLeg.updatePosition(lander.getBase().getPosition().x + 55.0f, lander.getBase().getPosition().y + 40.0f);
		// 	world.Step(timeStep, 7, 2);
		// 	std::vector<float> new_state = calculateState();
		// 	Reward get_reward = reward;
		// 	bool done = episode_complete;
		// 	return std::make_tuple(new_state, get_reward, done);
		// }

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