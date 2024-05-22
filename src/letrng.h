#pragma once
#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

class Letrng
{
public:
	uint64_t generate_random_number();

private:
	const unsigned int N = 10000000;
	uint64_t fair_coin();
	void toss_coins(std::atomic<uint64_t> &x64, std::atomic<uint64_t> &y64);
	uint64_t fold_bits(const uint64_t &value);
};