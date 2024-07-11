#pragma once
#include <random>
#include <glog/logging.h>

static int generate_random_number(int lower_bound, int upper_bound)
{
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(lower_bound, upper_bound);
  return dist(rng);
}

static float generate_random_number(float lower_bound, float upper_bound)
{
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_real_distribution<float> dist(lower_bound, upper_bound);
  return dist(rng);
}

static double generate_random_number(double lower_bound, double upper_bound)
{
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_real_distribution<double> dist(lower_bound, upper_bound);
  return dist(rng);
}