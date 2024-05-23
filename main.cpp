#include <fstream>

#include "letrng.h"

int main(int, char **)
{
	Letrng letrng;
	const unsigned int nNumbers = 1000;

	std::ofstream fp;
	std::string filename = "output.txt";
	fp.open(filename);

	if (!fp)
	{
		std::cout << "An error occured while trying to open a file " << filename << "\n";
		return 0;
	}

	for (unsigned int n = 0; n < nNumbers; n++)
	{
		uint64_t randomNumber = letrng.GenerateWord();
		std::string numberStr = std::to_string(static_cast<unsigned int>(randomNumber));
		// std::cout << numberStr << "\n";
		fp << numberStr << "\n";
	}

	fp.close();

	std::cout << "Output saved to file " << filename << "\n";

	return 0;
}