#include <iostream>
using namespace std;

// Assignment 2 - marks of five students.
// I wrote this myself, I only renamed the functions.

int getSum(int numbers[], int count) {
  int total = 0;
  for (int i = 0; i < count; i++) {
    total = total + numbers[i];
  }
  return total;
}

double getMean(int numbers[], int count) {
  int total = getSum(numbers, count);
  return (double) total / count;
}

int getMax(int numbers[], int count) {
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
  cout << "Total: " << getSum(marks, 5) << endl;
  cout << "Average: " << getMean(marks, 5) << endl;
  cout << "Largest: " << getMax(marks, 5) << endl;
  return 0;
}
