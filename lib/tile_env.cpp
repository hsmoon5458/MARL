#include "tile_env.h"

namespace lib::tile_env
{
    TileEnvironment::TileEnvironment(int state_size, int action_size, int agent_size, int display_width, int display_height, int number_of_tile_per_line)
        : state_size_(state_size),
          action_size_(action_size),
          agent_size_(agent_size),
          number_of_tile_per_line_(number_of_tile_per_line)
    {
        // GUI tile gird setup.
        const float line_thickness = 2;
        const float tile_size = 100;

        center_offset_ = {display_width / 2 - (tile_size * number_of_tile_per_line_) / 2, display_height / 2 - (tile_size * number_of_tile_per_line_) / 2};

        // Update tile.
        for (int i = 0; i < number_of_tile_per_line_; i++)
        {
            std::vector<sf::RectangleShape *> row;
            for (int j = 0; j < number_of_tile_per_line_; j++)
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

        // Update initial agent coors.
        for (int i = 0; i < agent_size_; i++)
        {
            auto random_agent_location =
                std::make_pair(generate_random_number(0, number_of_tile_per_line_ - 1),
                               generate_random_number(0, number_of_tile_per_line_ - 1));
            agents_current_tile_grid_location_.push_back(random_agent_location);
        }

        // GUI Agents Setup.
        const float agent_outline_thickness = 1;
        float radius = 50;

        for (int i = 0; i < agent_size_; i++)
        {
            auto circle = std::make_shared<sf::CircleShape>();
            circle->setRadius(radius);
            circle->setPosition(GetTilePixelPosition(agents_current_tile_grid_location_[i]));
            circle->setOutlineColor(sf::Color::White);
            circle->setOutlineThickness(agent_outline_thickness);
            circles_.push_back(circle);
        }

        reward_ = 0;
    }

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

        reward_ = 0;
        return state;
    }

    void TileEnvironment::PerformAgentAction(const std::vector<int> &actions)
    {
        for (int agent_index = 0; agent_index < agent_size_; agent_index++)
        {
            std::pair<int, int> new_coor = GetAgentCurrentTileGridLocation(agent_index);

            switch (actions[agent_index])
            {
            case AgentMovement::AGENT_LEFT:
                new_coor.first--;
                break;
            case AgentMovement::AGENT_RIGHT:
                new_coor.first++;
                break;
            case AgentMovement::AGENT_UP:
                new_coor.second--;
                break;
            case AgentMovement::AGENT_DOWN:
                new_coor.second++;
                break;
            // case AgentMovement::AGENT_HOLD:
            //     break;
            default:
                LOG(ERROR) << "Unidentified agent action performed!";
                break;
            }

            update_reward(new_coor);

            UpdateAgentTileGridLocation(agent_index, new_coor);
            UpdateAgentPixelLocation(agent_index, GetTilePixelPosition(new_coor));
            UpdateCleanedTile(new_coor, circles_[agent_index]->getFillColor());
        }
    }

    std::tuple<std::vector<float>, float, bool> TileEnvironment::Step(const std::vector<int> &actions)
    {
        if (actions.size() != agent_size_)
        {
            LOG(ERROR) << "actions size is not equal to agent_size!";
        }

        // Execute all agents action and update the reward.
        PerformAgentAction(actions);

        // Check all tiles are cleaned.
        if (GetAllTilesCleaned())
        {
            reward_ += reward_policy_map[RewardPolicy::ALL_TILES_CLEANED];
        }

        // Update state, reward, and done state.
        std::vector<float> new_state = calculate_state();

        return std::make_tuple(new_state, reward_, GetAllTilesCleaned());
    }

    int TileEnvironment::GenerateAgentRandomAction(const std::pair<int, int> location)
    {
        // Check possible directions. Left(0), Right(1), Up(2), Down(3).
        std::vector<int> possible_directions;

        if (location.first > 0)
            possible_directions.push_back(0);
        if (location.first < number_of_tile_per_line_ - 1)
            possible_directions.push_back(1);
        if (location.second > 0)
            possible_directions.push_back(2);
        if (location.second < number_of_tile_per_line_ - 1)
            possible_directions.push_back(3);

        // TOOD: Add HOLD as 4 if needed.

        // TODO: Check obstacles, and

        // Generate a random number based on the possible pathways.
        int rand_num = generate_random_number(0, possible_directions.size() - 1);
        return possible_directions[rand_num];
    }

    void TileEnvironment::update_reward(const std::pair<int, int> &coor)
    {
        // If the tile is uncleaned,
        if (!cleaned_tile_grid_[coor])
        {
            reward_ += reward_policy_map[RewardPolicy::TILE_NOT_CLEANED] / agent_size_;
        }

        // If the tile is already cleaned,
        else if (cleaned_tile_grid_[coor])
        {
            reward_ += reward_policy_map[RewardPolicy::TILE_ALREADY_CLEANED] / agent_size_;
        }

        // TODO: Add more policy.
    }
}
