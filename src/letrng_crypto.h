#pragma once
#include "cryptopp/osrng.h"
#include "letrng.h"

using namespace CryptoPP;

class LetrngCryptoPP : public RandomNumberGenerator, public Letrng
{
public:
	virtual ~LetrngCryptoPP() {}

	virtual void GenerateBlock(byte *output, size_t size);
	virtual word32 GenerateWord32(word32 min = 0, word32 max = 0xffffffffL);
	virtual void DiscardBytes(size_t n);
};