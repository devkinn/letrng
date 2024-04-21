#include "letrng.h"

void Letrng::generate_sequence(const unsigned int n_numbers) {
  std::ofstream fp;
  fp.open("../output.txt");
  for (unsigned int n = 0; n < n_numbers; n++) {
    uint64_t random_number = generate_random_number();
    std::string number_str = std::to_string(static_cast<int>(random_number));
    std::cout << number_str << "\n";
    fp << number_str << "\n";
  }
  fp.close();
}

uint64_t Letrng::generate_random_number() {
  uint64_t ret = 0;
  for (unsigned int n = 0; n < 7; n++) {
    uint64_t random_bit = fair_coin();
    ret ^= random_bit;
    ret <<= 1;
  }
  return ret;
}

uint64_t Letrng::fair_coin() {
  uint64_t x64 = 0;
  uint64_t y64 = 0;

  std::mutex x64mutex;
  std::mutex y64mutex;

  while (true) {
    toss_coins(x64, x64mutex, y64, y64mutex);

    uint64_t x64_out = copy_from_shared(x64, x64mutex);
    uint64_t y64_out = copy_from_shared(y64, y64mutex);

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
  for (int n = 1; n <= 64; n++) {
    uint64_t tmp = (value << (64 - n));
    tmp >>= 63;
    result ^= tmp;
  }
  return result;
}

void Letrng::toss_coins(uint64_t &x64, std::mutex &x64mutex, uint64_t &y64, std::mutex &y64mutex) {
  std::atomic<uint64_t> coin(0);
  std::atomic<bool> active(false);
  std::vector<std::thread> threads;

  std::thread writer1 = std::thread([&] {
    active.store(true);
    for (unsigned int n = 0; n < N; n++) {
      coin.store(n % 2);
      if (active.load() == false) {
        break;
      }
    }
    active.store(false);
  });
  threads.push_back(std::move(writer1));

  std::thread writer2 = std::thread([&] {
    active.store(true);
    for (unsigned int n = N; n > 0; n--) {
      coin.store(n % 2);
      if (active.load() == false) {
        break;
      }
    }
    active.store(false);
  });
  threads.push_back(std::move(writer2));

  std::thread sampler1 = std::thread([&] {
    uint64_t x64_out = 0;
    while (active.load()) {
      uint64_t loc = coin.load();
      x64_out <<= 1;
      x64_out ^= loc;
    }
    x64mutex.lock();
    x64 = x64_out;
    x64mutex.unlock();
  });
  threads.push_back(std::move(sampler1));

  std::thread sampler2 = std::thread([&] {
    uint64_t y64_out = 0;
    while (active.load()) {
      uint64_t loc = coin.load();
      y64_out <<= 1;
      y64_out ^= loc;
    }
    y64mutex.lock();
    y64 = y64_out;
    y64mutex.unlock();
  });
  threads.push_back(std::move(sampler2));

  for (auto &t : threads) {
    t.join();
  }
}

uint64_t Letrng::copy_from_shared(const uint64_t &value, std::mutex &mtx) {
  uint64_t result = 0;
  mtx.lock();
  result = value;
  mtx.unlock();
  return result;
}