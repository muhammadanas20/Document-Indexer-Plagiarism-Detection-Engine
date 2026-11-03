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

int findLargest(int numbers[], int count) {
  int largest = numbers[0];
  for (int i = 1; i < count; i++) {
    if (numbers[i] > largest) {
      largest = numbers[i];
    }
  }
  return largest;
}

int main() {
  int marks[5] = {72, 88, 65, 90, 51};
  cout << "Total: " << findTotal(marks, 5) << endl;
  cout << "Average: " << findAverage(marks, 5) << endl;
  cout << "Largest: " << findLargest(marks, 5) << endl;
  return 0;
}
