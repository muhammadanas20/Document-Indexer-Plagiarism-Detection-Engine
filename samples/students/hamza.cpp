#include <iostream>
using namespace std;

int findTotal(int numbers[], int count) {
  int total = 0;
  for (int i = 0; i < count; i++) {
    total = total + numbers[i];
  }
  return total;
}

double findAverage(int numbers[], int count) {
  int total = findTotal(numbers, count);
  return (double) total / count;
}

int countPassed(int numbers[], int count) {
  int passed = 0;
  for (int i = 0; i < count; i++) {
    if (numbers[i] >= 50) {
      passed++;
    }
  }
  return passed;
}

int main() {
  int marks[4] = {45, 60, 78, 30};
  cout << "Average: " << findAverage(marks, 4) << endl;
  cout << "Passed: " << countPassed(marks, 4) << endl;
  return 0;
}
