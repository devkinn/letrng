#include <fstream>
#include "letrng.h"

int main(int, char **)
{
	Letrng letrng;
	const unsigned int n_numbers = 100000;

	std::ofstream fp;
	std::string filename = "output.txt";
	fp.open(filename);

	if (!fp)
	{
		std::cout << "An error occured while trying to open a file " << filename << "\n";
		return 0;
	}

	for (unsigned int n = 0; n < n_numbers; n++)
	{
		uint64_t random_number = letrng.generate_random_number();
		std::string number_str = std::to_string(static_cast<unsigned int>(random_number));
		// std::cout << number_str << "\n";
		fp << number_str << "\n";
	}

	fp.close();

	std::cout << "Output saved to file " << filename << "\n";

	return 0;
}