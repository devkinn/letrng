#include "letrng.h"

int main(int, char **) {
  const unsigned int n_numbers = 500;
  Letrng letrng;
  letrng.generate_sequence(n_numbers);
  return 0;
}
