#include "letrng.h"

uint64_t Letrng::GenerateWord()
{
	uint64_t result = 0;
	for (unsigned int n = 0; n < 23; n++)
	{
		uint64_t randomBit = FairCoin();
		result <<= 1;
		result ^= randomBit;
	}
	return result;
}

uint64_t Letrng::FairCoin()
{
	std::atomic<uint64_t> x64 = 0;
	std::atomic<uint64_t> y64 = 0;

	while (true)
	{
		TossCoins(x64, y64);

		uint64_t x64Out = x64.load();
		uint64_t y64Out = y64.load();

		uint64_t x64Folded = FoldBits(x64Out);
		uint64_t y64Folded = FoldBits(y64Out);

		if (x64Folded == y64Folded)
			continue;
		if (x64Folded == 1)
			return 1;
		else
			return 0;
	}

	return 0;
}

uint64_t Letrng::FoldBits(const uint64_t &value)
{
	uint64_t result = 0;
	for (unsigned int n = 1; n <= 64; n++)
	{
		uint64_t tmp = (value << (64 - n));
		tmp >>= 63;
		result ^= tmp;
	}
	return result;
}

void Letrng::TossCoins(std::atomic<uint64_t> &x64, std::atomic<uint64_t> &y64)
{
	std::atomic<uint64_t> coin(0);
	std::atomic<uint8_t> samplerFinished(0);
	std::vector<std::thread> threads;

	threads.emplace_back(std::thread([this, &coin, &samplerFinished]
									 {
	for (unsigned int n = 0; n < m_N; n++) {
	  coin.store(n % 2);
	  if (samplerFinished.load() == 2) {
		break;
	  }
	} }));

	threads.emplace_back(std::thread([this, &coin, &samplerFinished]
									 {
	for (unsigned int n = m_N; n > 0; n--) {
	  coin.store(n % 2);
	  if (samplerFinished.load() == 2) {
		break;
	  }
	} }));

	threads.emplace_back(std::thread([&coin, &samplerFinished, &x64]
									 {
	uint64_t x64Out = 0;
	for (unsigned int n = 0; n < 64; n++) {
	  uint64_t loc = coin.load();
	  x64Out <<= 1;
	  x64Out |= loc;
	}
	x64.store(x64Out);
	samplerFinished.store(samplerFinished.load() + 1); }));

	threads.emplace_back(std::thread([&coin, &samplerFinished, &y64]
									 {
	uint64_t y64Out = 0;
	for (unsigned int n = 0; n < 64; n++) {
	  uint64_t loc = coin.load();
	  y64Out <<= 1;
	  y64Out |= loc;
	}
	y64.store(y64Out);
	samplerFinished.store(samplerFinished.load() + 1); }));

	for (auto &t : threads)
		t.join();
}