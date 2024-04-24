#include "letrng.h"

int main(int, char **) {
  const unsigned int n_numbers = 100000;
  Letrng letrng;
  letrng.generate_sequence(n_numbers);
  // letrng.generate_sequence_no_proc(n_numbers);
  return 0;
}