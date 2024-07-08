#include "tile_env.h"

namespace lib::tile_env
{
    TileEnvironment::TileEnvironment(int display_width, int display_height, int state_size, int action_size,
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
    };

    std::vector<float> TileEnvironment::Reset()
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
    };
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