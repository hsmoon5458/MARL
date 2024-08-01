#pragma once
#include <ctime>
#include <filesystem>
#include <glog/logging.h>
#include <iomanip>
#include <random>
#include <sstream>

static int generate_random_number(int lower_bound, int upper_bound) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(lower_bound,
                                                                upper_bound);
  return dist(rng);
}

static float generate_random_number(float lower_bound, float upper_bound) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_real_distribution<float> dist(lower_bound, upper_bound);
  return dist(rng);
}

static double generate_random_number(double lower_bound, double upper_bound) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_real_distribution<double> dist(lower_bound, upper_bound);
  return dist(rng);
}

static inline std::string get_current_time() {
  std::time_t now = std::time(nullptr);
  std::tm *localTime = std::localtime(&now);

  char buffer[20];
  std::strftime(buffer, sizeof(buffer), "%y_%m_%d_%H%M%S", localTime);

  return std::string(buffer);
}

static inline std::string get_home_directory() {
  const char *home = getenv("HOME");
  if (home) {
    return std::string(home);
  }
}