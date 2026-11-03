// test_graph.cpp  --  graph, BFS and clusters
// Owner: Alyan

#include <vector>

#include "graph.hpp"
#include "test_util.hpp"

using namespace dip;

int main() {
  Graph graph(6);
  CHECK(graph.vertexCount() == 6);
  CHECK(graph.edgeCount() == 0);
  CHECK(graph.degree(0) == 0);

  // 0 - 1 - 2 - 3 is one group, 4 - 5 is another, and nobody else is connected.
  graph.addEdge(0, 1);
  graph.addEdge(1, 2);
  graph.addEdge(2, 3);
  graph.addEdge(4, 5);

  CHECK(graph.edgeCount() == 4);
  CHECK(graph.degree(0) == 1);   // 0 is only connected to 1
  CHECK(graph.degree(1) == 2);   // 1 is connected to 0 and 2
  CHECK(graph.degree(2) == 2);

  // BFS from 0 reaches the whole first group, in distance order.
  std::vector<int> reached = graph.bfs(0);
  CHECK(reached.size() == 4);
  CHECK(reached[0] == 0);        // BFS always starts with the start vertex
  CHECK(reached[1] == 1);        // then the neighbours of 0
  CHECK(!graph.bfs(4).empty());

  // A vertex that is not connected to anything only reaches itself.
  Graph lonely(3);
  CHECK(lonely.bfs(1).size() == 1);

  // Two clusters, and single documents are not reported as clusters.
  std::vector<std::vector<int> > clusters = graph.clusters();
  CHECK(clusters.size() == 2);
  CHECK(clusters[0].size() == 4);
  CHECK(clusters[1].size() == 2);

  // Chain 0-1-2-3-4: BFS must visit them in this exact order.
  Graph chain(5);
  chain.addEdge(0, 1);
  chain.addEdge(1, 2);
  chain.addEdge(2, 3);
  chain.addEdge(3, 4);
  std::vector<int> order = chain.bfs(0);
  CHECK(order.size() == 5);
  CHECK(order[0] == 0);
  CHECK(order[4] == 4);
  // neighbours are stored at the front, so 1 comes before 2 before 3
  CHECK(order[1] == 1);

  return testutil::summary("test_graph");
}
