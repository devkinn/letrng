#pragma once
#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

class Letrng
{
public:
	uint64_t GenerateWord();

private:
	uint64_t FairCoin();
	void TossCoins(std::atomic<uint64_t> &x64, std::atomic<uint64_t> &y64);
	uint64_t FoldBits(const uint64_t &value);

private:
	const unsigned int m_N = 10000000;
};