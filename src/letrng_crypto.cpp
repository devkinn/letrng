#include "letrng_crypto.h"

word32 LetrngCryptoPP::GenerateWord32(word32 min, word32 max)
{
	const word32 range = max - min;
	if (range == 0xffffffffL)
		return GenerateWord();

	const int maxBits = BitPrecision(range);
	word32 value;

	do
	{
		value = Crop(GenerateWord(), maxBits);
	} while (value > range);

	return value + min;
}

void LetrngCryptoPP::GenerateBlock(byte *output, size_t size)
{
	// Handle word32 size blocks
	word32 temp;
	for (size_t i = 0; i < size / 4; i++, output += 4)
	{
#if defined(CRYPTOPP_ALLOW_UNALIGNED_DATA_ACCESS) && defined(IS_LITTLE_ENDIAN)
		*((word32 *)output) = ByteReverse(NextMersenneWord());
#elif defined(CRYPTOPP_ALLOW_UNALIGNED_DATA_ACCESS)
		*((word32 *)output) = NextMersenneWord();
#else
		temp = GenerateWord();
		output[3] = CRYPTOPP_GET_BYTE_AS_BYTE(temp, 0);
		output[2] = CRYPTOPP_GET_BYTE_AS_BYTE(temp, 1);
		output[1] = CRYPTOPP_GET_BYTE_AS_BYTE(temp, 2);
		output[0] = CRYPTOPP_GET_BYTE_AS_BYTE(temp, 3);
#endif
	}

	// No tail bytes
	if (size % 4 == 0)
		return;

	// Handle tail bytes
	temp = GenerateWord();
	switch (size % 4)
	{
	case 3:
		output[2] = CRYPTOPP_GET_BYTE_AS_BYTE(temp, 1); /* fall through */
	case 2:
		output[1] = CRYPTOPP_GET_BYTE_AS_BYTE(temp, 2); /* fall through */
	case 1:
		output[0] = CRYPTOPP_GET_BYTE_AS_BYTE(temp, 3);
		break;
	default:;
		;
	}
}

void LetrngCryptoPP::DiscardBytes(size_t n)
{
	for (size_t i = 0; i < (n + 3) / 4; i++)
		GenerateWord();
}