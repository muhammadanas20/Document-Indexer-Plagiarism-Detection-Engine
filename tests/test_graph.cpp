#include <vector>

#include "structures.hpp"
#include "test_util.hpp"

int main() {
  Graph graph(6);
  CHECK(graph.vertexCount() == 6);
  CHECK(graph.edgeCount() == 0);
  CHECK(graph.degree(0) == 0);

  graph.addEdge(0, 1);
  graph.addEdge(1, 2);
  graph.addEdge(2, 3);
  graph.addEdge(4, 5);

  CHECK(graph.edgeCount() == 4);
  CHECK(graph.degree(0) == 1);
  CHECK(graph.degree(1) == 2);
  CHECK(graph.degree(2) == 2);

  std::vector<int> reached = graph.bfs(0);
  CHECK(reached.size() == 4);
  CHECK(reached[0] == 0);
  CHECK(reached[1] == 1);
  CHECK(!graph.bfs(4).empty());

  Graph lonely(3);
  CHECK(lonely.bfs(1).size() == 1);

  std::vector<std::vector<int> > clusters = graph.clusters();
  CHECK(clusters.size() == 2);
  CHECK(clusters[0].size() == 4);
  CHECK(clusters[1].size() == 2);

  Graph chain(5);
  chain.addEdge(0, 1);
  chain.addEdge(1, 2);
  chain.addEdge(2, 3);
  chain.addEdge(3, 4);
  std::vector<int> order = chain.bfs(0);
  CHECK(order.size() == 5);
  CHECK(order[0] == 0);
  CHECK(order[4] == 4);

  CHECK(order[1] == 1);

  return testSummary("test_graph");
}
