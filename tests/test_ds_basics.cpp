#include <string>

#include "structures.hpp"
#include "test_util.hpp"

int main() {

  LinkedList<int> list;
  CHECK(list.empty());
  list.pushBack(1);
  list.pushBack(2);
  list.pushBack(3);
  CHECK(list.size() == 3);
  CHECK(list.head()->value == 1);
  CHECK(list.head()->next->value == 2);
  list.pushFront(0);
  CHECK(list.size() == 4);
  CHECK(list.head()->value == 0);

  CHECK(list.remove(2));
  CHECK(list.size() == 3);
  CHECK(!list.remove(99));
  CHECK(list.remove(0));
  CHECK(list.head()->value == 1);
  list.clear();
  CHECK(list.empty());
  CHECK(list.head() == nullptr);

  LinkedList<std::string> words;
  words.pushBack("sum");
  words.pushBack("total");
  CHECK(words.size() == 2);
  CHECK(words.head()->value == "sum");

  Stack<std::string> stack;
  CHECK(stack.empty());
  stack.push("samples");
  stack.push("samples/students");
  CHECK(stack.size() == 2);
  CHECK(stack.top() == "samples/students");
  CHECK(stack.pop() == "samples/students");
  CHECK(stack.pop() == "samples");
  CHECK(stack.empty());

  Stack<int> bigStack;
  for (int i = 0; i < 1000; ++i) bigStack.push(i);
  CHECK(bigStack.size() == 1000);
  CHECK(bigStack.pop() == 999);

  Queue<int> queue;
  CHECK(queue.empty());
  queue.push(1);
  queue.push(2);
  queue.push(3);
  CHECK(queue.front() == 1);
  CHECK(queue.pop() == 1);
  CHECK(queue.pop() == 2);
  queue.push(4);
  CHECK(queue.pop() == 3);
  CHECK(queue.pop() == 4);
  CHECK(queue.empty());

  Queue<int> bigQueue;
  for (int i = 0; i < 1000; ++i) bigQueue.push(i);
  CHECK(bigQueue.size() == 1000);
  for (int i = 0; i < 1000; ++i) {
    if (bigQueue.pop() != i) {
      CHECK(false);
      break;
    }
  }
  CHECK(bigQueue.empty());

  return testSummary("test_ds_basics");
}
