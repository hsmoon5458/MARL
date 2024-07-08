#pragma once
#include <random>

static int generate_random_number(const int &lower_bound, const int &upper_bound)
{
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(lower_bound, upper_bound);
  return dist(rng);
}