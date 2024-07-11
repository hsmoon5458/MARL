std::vector<float> dqn(int n_episodes = 1000, int max_t = 1000, float eps_start = 1.0, float eps_end = 0.01, float eps_decay = 0.995)
{
    // Create an instance of the LunarLanderEnvironment
    LunarLanderEnvironment env;
    srand(time(0));
    Agent agent(env.state_size, env.action_size, 0);
    std::vector<float> scores;       // List containing scores from each episode
    std::deque<float> scores_window; // Last 100 scores
    float eps = eps_start;           // Initialize epsilon
    // env.render(); // Render the environment
    for (int i_episode = 1; i_episode <= n_episodes; ++i_episode)
    {
        // Assuming 'env' and 'agent' are initialized instances of your Environment and Agent classes
        std::vector<float> state = env.reset();
        // std::cout << "state: \n " << state[0] << " " << state[1] << std::endl;
        // std::cout<< "state full: " << state << std::endl;
        float score = 0;
        int range = rand() % 21;
        for (int t = 0; t < max_t; ++t)
        {
            // std::cout << "entered here" << std::endl;
            int action = agent.act(state, eps);
            // std::cout << "action: " << action << std::endl;
            Action action_enum = env.getActionFromIndex(action);
            auto [next_state, reward_get, done] = env.step(action_enum, range);
            // std::cout << "reward_get: " << reward_get << std::endl;
            agent.step(state, action, reward_get, next_state, done);
            state = next_state;
            score += reward_get;
            if (done)
            {
                // std::cout << "breaking out done: " << done << std::endl;
                break;
            }
            env.render();
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
    saveToCSV(scores, n_episodes);

    return scores;
}