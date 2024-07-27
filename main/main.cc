#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"
#include "lib/agent.h"
#include "lib/qnetwork.h"
#include "lib/render_util.h"
#include "lib/tile_env.h"
#include "lib/utils.h"

#include <X11/Xlib.h>
#include <chrono>
#include <deque>
#include <fstream>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <thread>
#include <torch/torch.h>
#include <vector>

#define TIME_STEP_IN_MILLISECOND 10
#define MAX_AGENT_SIZE 4

inline void CheckEvent(sf::RenderWindow *window, sf::Event &event) {
  while (true) {
    while (window->pollEvent(event)) {
      if (event.type == sf::Event::Closed ||
          event.key.code == sf::Keyboard::Escape) {
        window->close();
        exit(0);
      }
    }
  }
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);

  XInitThreads();

  torch::Device device(torch::kCPU);
  LOG(INFO) << "Using device: "
            << (device.type() == torch::kCUDA ? "CUDA" : "CPU");

  // Setup Agent and Environment.
  const int agent_size = 1;
  const int number_of_tile_per_line = 4;
  const int state_size = 2 + number_of_tile_per_line * number_of_tile_per_line;
  const int max_step = number_of_tile_per_line * number_of_tile_per_line * 4;
  const int action_size = 4;
  const int seed = 0;

  std::vector<lib::agent::Agent *> agents_vector;

  if (agent_size > MAX_AGENT_SIZE) {
    LOG(ERROR) << "Number of agents exceed the limit!";
    exit(1);
  }

  // Instantiate agents.
  for (int id = 0; id < agent_size; id++) {
    auto *agent = new lib::agent::Agent(id, number_of_tile_per_line, state_size,
                                        action_size, seed, device);
    agents_vector.push_back(agent);
  }

  // Instantiate environment.
  lib::tile_env::TileEnvironment *env = new lib::tile_env::TileEnvironment(
      state_size, action_size, agent_size, max_step, number_of_tile_per_line);

  // Display size.
  int display_width =
      static_cast<int>(sf::VideoMode::getDesktopMode().width / 3.0f);
  int display_height =
      static_cast<int>(sf::VideoMode::getDesktopMode().height / 3.0f);
  sf::RenderWindow *window =
      new sf::RenderWindow(sf::VideoMode(display_width, display_height),
                           "MALR Test Window", sf::Style::Default);
  // Check close event.
  sf::Event event;
  std::thread check_event_thread(CheckEvent, window, std::ref(event));
  check_event_thread.detach();

  // Setup info text.
  sf::Font font;
  font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMono.ttf");
  float text_margin = 20.f; // Margin from the edge of the window
  float text_size = std::min(display_width, display_height) * 0.03f;
  sf::Text info_text = render_util::SetText(
      font, static_cast<unsigned int>(text_size), text_margin, text_margin,
      sf::Color::White, sf::Text::Style::Regular);

  // SFML agetns and env setup.
  std::vector<std::vector<sf::RectangleShape *>> tile_grid;
  std::vector<std::shared_ptr<sf::CircleShape>> circles;
  // Agents initial coordinates.
  std::map<int, std::pair<int, int>> agent_initial_coors{
      {0, {0, 0}},
      {1, {number_of_tile_per_line - 1, 0}},
      {2, {0, number_of_tile_per_line - 1}},
      {3, {number_of_tile_per_line, number_of_tile_per_line - 1}},
  };

  // GUI tile gird setup.
  const float line_thickness = 2.f;
  const float grid_proportion =
      0.8f; // Grid will occupy 80% of the smaller screen dimension
  const float grid_size =
      std::min(display_width, display_height) * grid_proportion;
  const float tile_size = grid_size / number_of_tile_per_line - line_thickness;

  sf::Vector2f center_offset = {(display_width - grid_size) / 2,
                                (display_height - grid_size) / 2};

  for (int i = 0; i < number_of_tile_per_line; i++) {
    std::vector<sf::RectangleShape *> row;
    for (int j = 0; j < number_of_tile_per_line; j++) {
      sf::RectangleShape *tile = new sf::RectangleShape;
      tile->setSize({tile_size, tile_size});
      tile->setOutlineThickness(line_thickness);
      tile->setOutlineColor(sf::Color::White);
      tile->setFillColor(sf::Color::Transparent);
      tile->setPosition({center_offset.x + (tile_size + line_thickness) * i,
                         center_offset.y + (tile_size + line_thickness) * j});
      row.push_back(tile);
    }
    tile_grid.push_back(row);
  }

  // GUI Agents Setup.
  const float agent_outline_thickness = 1.f;
  float radius = tile_size / 2 -
                 agent_outline_thickness; // Set radius to fit within a tile

  for (int i = 0; i < agent_size; i++) {
    auto circle = std::make_shared<sf::CircleShape>();
    circle->setRadius(radius);

    // Calculate position to center the circle within its tile
    sf::Vector2f tilePosition =
        tile_grid[agent_initial_coors[i].first][agent_initial_coors[i].second]
            ->getPosition();
    circle->setPosition(tilePosition.x + (tile_size - 2 * radius) / 2,
                        tilePosition.y + (tile_size - 2 * radius) / 2);

    circle->setOutlineColor(sf::Color::White);
    circle->setOutlineThickness(agent_outline_thickness);
    circles.push_back(circle);
  }

  for (int i = 0; i < agents_vector.size(); i++) {
    circles[i]->setFillColor(render_util::agent_colors[i]);
  }

  const int n_episodes = 1000;
  const float eps_start = 1.0;
  const float eps_end = 0.01;
  const float eps_decay = 0.995;

  std::ofstream out_file("/home/hmoon/reward_single_agent_1.txt",
                         std::ios::app);
  if (!out_file) {
    LOG(ERROR) << "Unable to open file!";
    return 1;
  }

  // Start main loop of Window GUI.
  while (window->isOpen()) {
    // DQN
    float eps = eps_start;

    for (int i_episode = 1; i_episode <= n_episodes; ++i_episode) {
      // Reset the environment and fresh the state.
      std::vector<float> state = env->Reset();
      float total_reward = 0;
      int step = 0;
      // Each episode, execute max_t actions.
      while (true) {
        // Get action from each agent for environment.
        std::vector<int> actions;
        // In this for loop, state and eps are identical for
        // all agents.
        for (int agent_index = 0; agent_index < agent_size; agent_index++) {
          // Get what action to take from agent.
          actions.push_back(agents_vector[agent_index]->Act(state, eps));
        }

        // Update environment (perform all agents action, update state and
        // reward).
        auto [next_state, reward, done] = env->Step(actions, step);

        // In this for loop, state and eps are identical for ll agents.
        for (int agent_index = 0; agent_index < agent_size; agent_index++) {
          agents_vector[agent_index]->Step(state, actions[agent_index], reward,
                                           next_state, done);
        }

        state = next_state;
        total_reward += reward;

        // Render environment.
        window->clear();
        // Update agents location.
        for (int agent_index = 0; agent_index < agent_size; agent_index++) {
          render_util::UpdateAgentPixelLocation(
              agent_index, env->GetAgentCurrentTileGridLocation(agent_index),
              circles, tile_grid);
        }

        // Update tiles.
        render_util::UpdateCleanedTile(env->GetCleanedTileGrid(), tile_grid);

        // Draw Envrionment.
        render_util::RenderEnvironment(window, circles, tile_grid);

        // Update info text.
        info_text.setString("Number of Agent: " + std::to_string(agent_size) +
                            "\nReward: " + std::to_string(total_reward) +
                            "\nEpisode: " + std::to_string(i_episode) +
                            "\nStep: " + std::to_string(step));
        window->draw(info_text);
        window->display();

        // Check all tiles are cleaned or reached max step.
        if (done) {
          render_util::ClearCleanedTileState(tile_grid);
          break;
        }
        step++;
      }

      render_util::ClearCleanedTileState(tile_grid);

      eps = std::max(eps_end, eps_decay * eps);

      for (int i = 0; i < agent_size; i++) {
        out_file << agents_vector[i]->GetMSELossValue() << " ";
      }
      LOG(INFO) << "Episode: " << i_episode << " Reward: " << total_reward;
      out_file << total_reward << std::endl;
    }
  }

  out_file.close();
  return 0;
}