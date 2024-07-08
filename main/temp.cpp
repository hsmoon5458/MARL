std::vector<float> DQN(int n_episodes = 1000, int max_t = 1000, float eps_start = 1.0, float eps_end = 0.01, float eps_decay = 0.995)
{
    const int state_size = 4;
    const int action_size = 4;
    const int number_of_agent = 1;

    std::vector<lib::agent::Agent *> agents;
    std::vector<std::pair<int, int>> random_agents_location;

    // srand(time(0));
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

    TileEnvironment *env = new TileEnvironment(800, 600, state_size, action_size, agents, random_agents_location);

    std::vector<float> scores;       // List containing scores from each episode
    std::deque<float> scores_window; // Last 100 scores
    float eps = eps_start;           // Initialize epsilon
    // env.render(); // Render the environment
    for (int i_episode = 1; i_episode <= n_episodes; ++i_episode)
    {
        // Assuming 'env' and 'agent' are initialized instances of your Environment and Agent classes
        std::vector<float> state = env->Reset();
        float score = 0;
        int range = rand() % 21;
        for (int t = 0; t < max_t; ++t)
        {
            // TODO: Make this iteratable.
            int action = agents[0]->Act(state, eps);

            auto [next_state, reward_get, done] = env->Step(action, range);

            // TODO: Make this iteratable.
            agents[0]->Step(state, action, reward_get, next_state, done);
            state = next_state;
            score += reward_get;
            if (done)
            {
                // std::cout << "breaking out done: " << done << std::endl;
                break;
            }
            // env.render();
        }
        scores_window.push_back(score);
        scores.push_back(score);
        eps = std::max(eps_end, eps_decay * eps);

        std::cout << "\rEpisode " << i_episode << "\tAverage Score: " << std::accumulate(scores_window.begin(), scores_window.end(), 0.0f) / scores_window.size() << std::flush;

        if (i_episode % 100 == 0)
        {
            std::cout << "\rEpisode " << i_episode << "\tAverage Score: " << std::accumulate(scores_window.begin(), scores_window.end(), 0.0f) / scores_window.size() << std::endl;
        }
    }
    std::cout << "Training complete!" << std::endl;
    // saveToCSV(scores, n_episodes);

    return scores;
}