// renamed_copy.cpp -- same code, different variable names and spacing.
// Must be caught: the normalised token view is identical.
#include <iostream>

int sum_to(int limit) {
  int acc = 0;
  for (int i = 1; i <= limit; ++i) acc += i;
  return acc;
}

int main() {
  std::cout << sum_to(100) << std::endl;
  return 0;
}
