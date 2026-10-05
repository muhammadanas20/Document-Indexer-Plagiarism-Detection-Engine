// verbatim_copy.cpp -- a byte-for-byte copy apart from this comment line.
#include <iostream>

int sum_to(int n) {
  int total = 0;
  for (int i = 1; i <= n; ++i) total += i;
  return total;
}

int main() {
  std::cout << sum_to(100) << std::endl;
  return 0;
}
