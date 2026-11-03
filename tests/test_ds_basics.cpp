// test_ds_basics.cpp  --  linked list, stack and queue
// Owner: Anas

#include <string>

#include "list.hpp"
#include "queue.hpp"
#include "stack.hpp"
#include "test_util.hpp"

using namespace dip;

int main() {
  // ---- singly linked list -------------------------------------------------
  LinkedList<int> list;
  CHECK(list.empty());
  list.pushBack(1);
  list.pushBack(2);
  list.pushBack(3);
  CHECK(list.size() == 3);
  CHECK(list.head()->value == 1);          // head is still the first inserted
  CHECK(list.head()->next->value == 2);    // the nodes are chained
  list.pushFront(0);
  CHECK(list.size() == 4);
  CHECK(list.head()->value == 0);

  CHECK(list.remove(2));                   // remove from the middle
  CHECK(list.size() == 3);
  CHECK(!list.remove(99));                 // not there
  CHECK(list.remove(0));                   // remove the head
  CHECK(list.head()->value == 1);
  list.clear();
  CHECK(list.empty());
  CHECK(list.head() == nullptr);

  LinkedList<std::string> words;
  words.pushBack("sum");
  words.pushBack("total");
  CHECK(words.size() == 2);
  CHECK(words.head()->value == "sum");

  // ---- stack (LIFO) -------------------------------------------------------
  Stack<std::string> stack;
  CHECK(stack.empty());
  stack.push("samples");       // the folder we start from
  stack.push("samples/students");
  CHECK(stack.size() == 2);
  CHECK(stack.top() == "samples/students");
  CHECK(stack.pop() == "samples/students");  // last in, first out
  CHECK(stack.pop() == "samples");
  CHECK(stack.empty());

  // The array inside the stack has to grow when we push many items.
  Stack<int> bigStack;
  for (int i = 0; i < 1000; ++i) bigStack.push(i);
  CHECK(bigStack.size() == 1000);
  CHECK(bigStack.pop() == 999);

  // ---- queue (FIFO) -------------------------------------------------------
  Queue<int> queue;
  CHECK(queue.empty());
  queue.push(1);
  queue.push(2);
  queue.push(3);
  CHECK(queue.front() == 1);
  CHECK(queue.pop() == 1);     // first in, first out
  CHECK(queue.pop() == 2);
  queue.push(4);               // this one wraps around the circular array
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

  return testutil::summary("test_ds_basics");
}
