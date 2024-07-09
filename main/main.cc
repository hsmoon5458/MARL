#include "lib/tile_env.h"
#include "lib/agent.h"
#include "lib/utils.h"
#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"
#include "lib/qnetwork.h"

#include <glog/logging.h>
#include <torch/torch.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <deque>

#define TIME_STEP_IN_MILLISECOND 100
#define NUMBER_OF_TILES_PER_LINE 20
#define MAX_AGENTS_NUMBER 7
const sf::Color agent_colors[] = {
	sf::Color::Red,
	sf::Color::Green,
	sf::Color::Blue,
	sf::Color::Cyan,
	sf::Color::Magenta,
	sf::Color::Yellow,
	sf::Color::White};

int main(int argc, char **argv)
{
	// Display size.
	int display_width = sf::VideoMode::getDesktopMode().width;
	int display_height = sf::VideoMode::getDesktopMode().height;
	sf::RenderWindow window(sf::VideoMode(display_width, display_height), "MALR Test Window", sf::Style::Default);

	const int number_of_agent = 4;
	// State size is determined by # agents' coordinate + # of tile grids
	const int state_size = number_of_agent * 2 + NUMBER_OF_TILES_PER_LINE * NUMBER_OF_TILES_PER_LINE;
	const int action_size = 4;

	std::vector<lib::agent::Agent *> agents;
	std::vector<std::pair<int, int>> random_agents_location;

	if (number_of_agent > MAX_AGENTS_NUMBER)
	{
		LOG(ERROR) << "Number of agents exceed the limit!";
		exit(1);
	}

	// Instantiate agents.
	for (int i = 0; i < number_of_agent; i++)
	{
		auto random_agent_location =
			std::make_pair(generate_random_number(0, NUMBER_OF_TILES_PER_LINE - 1),
						   generate_random_number(0, NUMBER_OF_TILES_PER_LINE - 1));
		auto *agent = new lib::agent::Agent(state_size, action_size, 0);

		// TODO: Delete this vector at the end;
		agents.push_back(agent);
		random_agents_location.push_back(random_agent_location);
	}

	// Instantiate environment and setup the color of each agent.
	// TODO: Remove agents argument from env.
	lib::tile_env::TileEnvironment *env = new lib::tile_env::TileEnvironment(display_width, display_height, state_size, action_size, NUMBER_OF_TILES_PER_LINE, agents, random_agents_location);
	for (int i = 0; i < agents.size(); i++)
	{
		env->circles[i]->setFillColor(agent_colors[i]);
	}

	// Set text.
	sf::Font font;
	font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMono.ttf");
	sf::Text info_text;
	info_text.setFont(font);
	info_text.setCharacterSize(40); // in pixel
	info_text.setPosition({display_width - 1000.f, display_height - 600.f});
	info_text.setFillColor(sf::Color::White);
	info_text.setStyle(sf::Text::Style::Regular);

	// # of steps.
	int number_of_step = 0;
	int number_of_episode = 1;
	const int max_previous_rewards_size = 100;
	std::deque<float> previous_rewards;

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
				env->Reset();
			}
		}

		// Check all tiles are cleaned.
		if (env->GetAllTilesCleaned())
		{
			previous_rewards.push_back(env->GetReward());
			if (previous_rewards.size() > max_previous_rewards_size)
				previous_rewards.pop_front();
			env->Reset();
			number_of_step = 0;
			number_of_episode++;
		}

		// Update agents' position.
		for (int agent_index = 0; agent_index < env->GetAgentsSize(); agent_index++)
		{
			env->PerformAgentAction(agent_index,
									env->GenerateAgentRandomAction(env->GetAgentCurrentTileGridLocation(agent_index)));
		}

		number_of_step++;

		// Timestep interval.
		if (TIME_STEP_IN_MILLISECOND != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(TIME_STEP_IN_MILLISECOND));
		}

		// Update display.
		for (int row = 0; row < NUMBER_OF_TILES_PER_LINE; row++)
		{
			for (int each_tile = 0; each_tile < NUMBER_OF_TILES_PER_LINE; each_tile++)
			{
				window.draw(*env->GetTile({row, each_tile}));
			}
		}

		// Update agents display on top of tiles.
		for (auto circle : env->circles)
		{
			window.draw(*circle);
		}

		// Update info text.
		info_text.setString("Number of Agent: " + std::to_string(env->GetAgentsSize()) +
							"\nReward: " + std::to_string(env->GetReward()) +
							"\nNumber of Step: " + std::to_string(number_of_step) +
							"\nNumber of Episode: " + std::to_string(number_of_episode));
		window.draw(info_text);

		window.display();
	}

	// TODO: Delete all pointers.

	return 0;
}