#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"
#include "lib/agent.h"
#include "lib/qnetwork.h"
#include "lib/tile_env.h"
#include "lib/utils.h"

#include <X11/Xlib.h>
#include <chrono>
#include <deque>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <thread>
#include <torch/torch.h>
#include <vector>

#define TIME_STEP_IN_MILLISECOND 10
#define MAX_AGENTS_NUMBER 4

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
  XInitThreads();

  torch::Device device(torch::cuda::is_available() ? torch::kCUDA
                                                   : torch::kCPU);
  LOG(INFO) << "Using device: "
            << (device.type() == torch::kCUDA ? "CUDA" : "CPU");

  // Display size.
  int display_width = sf::VideoMode::getDesktopMode().width;
  int display_height = sf::VideoMode::getDesktopMode().height;
  sf::RenderWindow *window =
      new sf::RenderWindow(sf::VideoMode(display_width, display_height),
                           "MALR Test Window", sf::Style::Default);

  // Setup info text.
  sf::Font font;
  font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMono.ttf");
  sf::Text info_text =
      SetText(font, 40, display_width - 1000.f, display_height - 600.f,
              sf::Color::White, sf::Text::Style::Regular);
  sf::Text test_running =
      SetText(font, 80, display_width / 2, display_height / 2, sf::Color::White,
              sf::Text::Style::Bold);

  // Setup Agent and Environment.
  const int number_of_agent = 1;
  const int number_of_tile_per_line = 20;
  const int state_size =
      number_of_agent * 2 + number_of_tile_per_line * number_of_tile_per_line;
  const int action_size = 4;
  const int seed = 0;

  std::vector<lib::agent::Agent *> agents_vector;

  if (number_of_agent > MAX_AGENTS_NUMBER) {
    LOG(ERROR) << "Number of agents exceed the limit!";
    exit(1);
  }

  // Instantiate agents.
  for (int id = 0; id < number_of_agent; id++) {
    auto *agent = new lib::agent::Agent(id, number_of_tile_per_line, state_size,
                                        action_size, seed, device);
    agents_vector.push_back(agent);
  }

  // Instantiate environment and setup the color of each agent.
  lib::tile_env::TileEnvironment *env = new lib::tile_env::TileEnvironment(
      state_size, action_size, agents_vector.size(), display_width,
      display_height);

  // TODO: Move this into the Env class.
  for (int i = 0; i < agents_vector.size(); i++) {
    env->GetCircles()[i]->setFillColor(agent_colors[i]);
  }

  const int n_episodes = 1000;
  const int max_t = 5000;
  const float eps_start = 1.0;
  const float eps_end = 0.01;
  const float eps_decay = 0.995;

  // Check close event.
  sf::Event event;
  std::thread check_event_thread(CheckEvent, window, std::ref(event));
  check_event_thread.detach();

  std::vector<float> total_reward_vector;
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

      // Each episode, execute max_t actions.
      for (int t = 0; t < max_t; ++t) {
        // Get action from each agent for environment.
        std::vector<int> actions;
        for (int agent_index = 0; agent_index < number_of_agent;
             agent_index++) // In this for loop, state and eps are identical for
                            // all agents.
        {
          // Get what action to take from agent.
          actions.push_back(agents_vector[agent_index]->Act(state, eps));
        }

        // Update environment (perform all agents action, update state and
        // reward).
        auto [next_state, reward, done] = env->Step(actions);

        // In this for loop, state and eps are identical for ll agents.
        for (int agent_index = 0; agent_index < number_of_agent;
             agent_index++) {
          agents_vector[agent_index]->Step(state, actions[agent_index], reward,
                                           next_state, done);
        }

        state = next_state;
        total_reward += reward;

        // Render environment.
        window->clear();
        env->RenderEnvironment(window);
        // Update info text.
        info_text.setString(
            "Number of Agent: " + std::to_string(number_of_agent) +
            "\nReward: " + std::to_string(total_reward) + "\nEpisode: " +
            std::to_string(i_episode) + "\nStep: " + std::to_string(t));
        window->draw(info_text);

        window->display();

        // Timestep interval for visualiziing agents movment.
        std::this_thread::sleep_for(
            std::chrono::milliseconds(TIME_STEP_IN_MILLISECOND));

        // Check all tiles are cleaned.
        if (done) {
          break;
        }
      }
      for (int i = 0; i < number_of_agent; i++) {
        out_file << agents_vector[i]->GetMSELossValue() << " ";
      }
      LOG(INFO) << "Episode: " << i_episode << " Reward: " << total_reward;
      total_reward_vector.push_back(total_reward);
      out_file << total_reward << std::endl;
    }
  }

  out_file.close();
  return 0;
}