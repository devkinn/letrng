#include "letrng.h"

void Letrng::generate_sequence(const unsigned int n_numbers) {
  std::ofstream fp;
  fp.open("output.txt");
  for (unsigned int n = 0; n < n_numbers; n++) {
    uint64_t random_number = generate_random_number();
    std::string number_str = std::to_string(static_cast<unsigned int>(random_number));
    std::cout << number_str << "\n";
    fp << number_str << "\n";
  }
  fp.close();
}

uint64_t Letrng::generate_random_number() {
  uint64_t result = 0;
  for (unsigned int n = 0; n < 8; n++) {
    uint64_t random_bit = fair_coin();
    result <<= 1;
    result ^= random_bit;
  }
  return result;
}

uint64_t Letrng::fair_coin() {
  std::atomic<uint64_t> x64 = 0;
  std::atomic<uint64_t> y64 = 0;

  while (true) {
    toss_coins(x64, y64);

    uint64_t x64_out = x64.load();
    uint64_t y64_out = y64.load();

    // std::cout << x64_out << " " << y64_out << std::endl;

    uint64_t x64_folded = fold_bits(x64_out);
    uint64_t y64_folded = fold_bits(y64_out);

    // std::cout << x64_folded << " " << y64_folded << std::endl;

    if (x64_folded == y64_folded) continue;
    if (x64_folded == 1)
      return 1;
    else
      return 0;
  }

  return 0;
}

uint64_t Letrng::fold_bits(const uint64_t &value) {
  uint64_t result = 0;
  for (unsigned int n = 1; n <= 64; n++) {
    uint64_t tmp = (value << (64 - n));
    tmp >>= 63;
    result ^= tmp;
  }
  return result;
}

void Letrng::toss_coins(std::atomic<uint64_t> &x64, std::atomic<uint64_t> &y64) {
  std::atomic<uint64_t> coin(0);
  std::atomic<uint8_t> sampler_finished_c(0);
  std::vector<std::thread> threads;

  threads.emplace_back(std::thread([&coin, &sampler_finished_c] {
    for (unsigned int n = 0; n < N; n++) {
      coin.store(n % 2);
      if (sampler_finished_c.load() == 2) {
        break;
      }
    }
  }));

  threads.emplace_back(std::thread([&coin, &sampler_finished_c] {
    for (unsigned int n = N; n > 0; n--) {
      coin.store(n % 2);
      if (sampler_finished_c.load() == 2) {
        break;
      }
    }
  }));

  threads.emplace_back(std::thread([&coin, &sampler_finished_c, &x64] {
    uint64_t x64_out = 0;
    for (unsigned int n = 0; n < 64; n++) {
      uint64_t loc = coin.load();
      x64_out <<= 1;
      x64_out |= loc;
    }
    x64.store(x64_out);
    sampler_finished_c.store(sampler_finished_c.load() + 1);
  }));

  threads.emplace_back(std::thread([&coin, &sampler_finished_c, &y64] {
    uint64_t y64_out = 0;
    for (unsigned int n = 0; n < 64; n++) {
      uint64_t loc = coin.load();
      y64_out <<= 1;
      y64_out |= loc;
    }
    y64.store(y64_out);
    sampler_finished_c.store(sampler_finished_c.load() + 1);
  }));

  for (auto &t : threads) t.join();
}

uint64_t Letrng::copy_from_shared(const uint64_t &value, std::mutex &mtx) {
  uint64_t result = 0;
  mtx.lock();
  result = value;
  mtx.unlock();
  return result;
}