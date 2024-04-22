#pragma once
#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

const unsigned int N = 10000000;

class Letrng {
 public:
  void generate_sequence(const unsigned int n_numbers);

 private:
  uint64_t generate_random_number();
  uint64_t fair_coin();
  void toss_coins(std::atomic<uint64_t> &x64, std::atomic<uint64_t> &y64);
  uint64_t copy_from_shared(const uint64_t &value, std::mutex &mtx);
  uint64_t fold_bits(const uint64_t &value);
};