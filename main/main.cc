#include "lib/tile.h"
#include "lib/agent.h"
#include "lib/utils.h"
#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <chrono>
#include <thread>

#define TIME_STEP_IN_MILLISECOND 50
#define NUMBER_OF_TILES_PER_LINE 20

enum AgentMovement
{
	AGENT_LEFT,
	AGENT_RIGHT,
	AGENT_UP,
	AGENT_DOWN
};

std::pair<int, int> GenerateAgentRandomMovement(const std::pair<int, int> location)
{
	int future_x = location.first;
	int future_y = location.second;

	// Check possible directions. Left(0), Right(1), Up(2), Down(3).
	std::vector<int> possible_directions;
	if (location.first > 0)
		possible_directions.push_back(0);
	if (location.first < NUMBER_OF_TILES_PER_LINE - 1)
		possible_directions.push_back(1);
	if (location.second > 0)
		possible_directions.push_back(2);
	if (location.second < NUMBER_OF_TILES_PER_LINE - 1)
		possible_directions.push_back(3);
	// TODO: Check obstacles.

	// Generate a random number based on the possible pathways.
	int random_num = generate_random_number(0, possible_directions.size() - 1);

	switch (possible_directions[random_num])
	{
	case AgentMovement::AGENT_LEFT:
		future_x--;
		break;
	case AgentMovement::AGENT_RIGHT:
		future_x++;
		break;
	case AgentMovement::AGENT_UP:
		future_y--;
		break;
	case AgentMovement::AGENT_DOWN:
		future_y++;
		break;
	default:
		std::cout << "Dead Lock!" << std::endl;
		break;
	}

	return std::make_pair(future_x, future_y);
};

int main(int argc, char **argv)
{
	// Display size.
	int display_width = sf::VideoMode::getDesktopMode().width;
	int display_height = sf::VideoMode::getDesktopMode().height;
	sf::RenderWindow window(sf::VideoMode(display_width, display_height), "Test Window", sf::Style::Default);

	// Generate tiles, default size 20*20.
	lib::tile::Tile tile_map(display_width, display_height, NUMBER_OF_TILES_PER_LINE);

	// Add three agents.
	std::vector<std::unique_ptr<lib::agent::Agent>> agent_vector;
	for (int id = 0; id < 3; id++)
	{
		auto rand_coor =
				std::make_pair(generate_random_number(0, NUMBER_OF_TILES_PER_LINE - 1),
											 generate_random_number(0, NUMBER_OF_TILES_PER_LINE - 1));

		auto agent = std::make_unique<lib::agent::Agent>(id, tile_map.GetTilePixelPosition(rand_coor), rand_coor);
		agent_vector.push_back(std::move(agent));
	}

	agent_vector[0]->circle->setFillColor(sf::Color::Red);
	agent_vector[1]->circle->setFillColor(sf::Color::Green);
	agent_vector[2]->circle->setFillColor(sf::Color::Blue);

	// Start updating GUI.
	while (window.isOpen())
	{
		// Clear before update the display for each frame.
		window.clear();

		// Check close event.
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
				window.close();

			// Reset.
			if (event.key.code == sf::Keyboard::R)
			{
				// Clear cleaned tiles.
				tile_map.ClearCleanedTileState();

				// Update agents initial positions.
				for (auto &agent : agent_vector)
				{
					auto rand_coor =
							std::make_pair(generate_random_number(0, NUMBER_OF_TILES_PER_LINE - 1),
														 generate_random_number(0, NUMBER_OF_TILES_PER_LINE - 1));
					agent->UpdatePixelLocation(tile_map.GetTilePixelPosition(rand_coor));
					agent->UpdateTileGridLocation(rand_coor);
				}
			}
		}

		// Update agents' position.
		for (auto &agent : agent_vector)
		{
			auto new_coor = GenerateAgentRandomMovement(agent->GetCurrentTileGridLocation());

			agent->circle->setPosition(tile_map.GetTilePixelPosition(new_coor));
			agent->UpdateTileGridLocation(new_coor);
			tile_map.UpdateCleanedTile(new_coor, agent->circle->getFillColor());
		}

		// Timestep interval.
		std::this_thread::sleep_for(std::chrono::milliseconds(TIME_STEP_IN_MILLISECOND));

		// Update display.
		for (int row = 0; row < NUMBER_OF_TILES_PER_LINE; row++)
		{
			for (int each_tile = 0; each_tile < NUMBER_OF_TILES_PER_LINE; each_tile++)
			{
				window.draw(*tile_map.GetTile({row, each_tile}));
			}
		}
		// Update agents display on top of tiles.
		for (auto &agent : agent_vector)
		{
			window.draw(*agent->circle);
		}

		window.display();
	}

	return 1;
}