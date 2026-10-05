// original.cpp -- the source everyone "wrote independently"
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
