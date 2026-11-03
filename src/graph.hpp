// graph.hpp  --  Graph + Breadth-First Search  (Week 14 topic: Graphs,
// representation and traversal)
// Owner: Alyan
//
// Where it is used in the project:
//   We build a "similarity graph": every document is a vertex, and we add an
//   edge between two documents when their similarity is above a threshold.
//   Then one BFS per unvisited vertex gives the groups (clusters) of students
//   who copied from each other - a group of 4 copied documents shows up as one
//   cluster instead of 6 separate pairs.
//
// Representation: adjacency list (array of linked lists). For a sparse graph
// like this one it uses much less memory than an adjacency matrix.

#ifndef DIP_GRAPH_HPP
#define DIP_GRAPH_HPP

#include <cstddef>
#include <vector>

#include "list.hpp"
#include "queue.hpp"

namespace dip {

class Graph {
 public:
  explicit Graph(int vertexCount)
      : adjacency_(nullptr), vertexCount_(vertexCount), edgeCount_(0) {
    adjacency_ = new LinkedList<int>*[vertexCount_];
    for (int i = 0; i < vertexCount_; ++i) adjacency_[i] = new LinkedList<int>();
  }

  // Rule of Three: free every adjacency list.
  ~Graph() {
    for (int i = 0; i < vertexCount_; ++i) delete adjacency_[i];
    delete[] adjacency_;
  }
  Graph(const Graph&) = delete;
  Graph& operator=(const Graph&) = delete;

  // Undirected edge a <-> b: we add it to both adjacency lists. O(1).
  void addEdge(int a, int b) {
    adjacency_[a]->pushBack(b);
    adjacency_[b]->pushBack(a);
    ++edgeCount_;
  }

  int degree(int vertex) const {
    return static_cast<int>(adjacency_[vertex]->size());
  }
  int vertexCount() const { return vertexCount_; }
  int edgeCount() const { return edgeCount_; }

  // BFS from one start vertex. Returns every vertex reachable from it.
  // O(V + E) because every vertex and every edge is handled once.
  std::vector<int> bfs(int start) const {
    std::vector<int> visited;
    std::vector<bool> seen(static_cast<std::size_t>(vertexCount_), false);

    Queue<int> queue;  // the queue is the whole point of BFS
    queue.push(start);
    seen[static_cast<std::size_t>(start)] = true;

    while (!queue.empty()) {
      int current = queue.pop();
      visited.push_back(current);

      for (LinkedList<int>::Node* n = adjacency_[current]->head(); n != nullptr;
           n = n->next) {
        int neighbour = n->value;
        if (!seen[static_cast<std::size_t>(neighbour)]) {
          seen[static_cast<std::size_t>(neighbour)] = true;
          queue.push(neighbour);
        }
      }
    }
    return visited;
  }

  // One BFS per vertex that we have not seen yet = the connected components.
  // Each returned vector is one cluster of documents.
  std::vector<std::vector<int> > clusters() const {
    std::vector<std::vector<int> > result;
    std::vector<bool> seen(static_cast<std::size_t>(vertexCount_), false);

    for (int start = 0; start < vertexCount_; ++start) {
      if (seen[static_cast<std::size_t>(start)]) continue;
      std::vector<int> cluster = bfs(start);
      for (std::size_t i = 0; i < cluster.size(); ++i) {
        seen[static_cast<std::size_t>(cluster[i])] = true;
      }
      // A single document with no similar partner is not interesting.
      if (cluster.size() > 1) result.push_back(cluster);
    }
    return result;
  }

 private:
  LinkedList<int>** adjacency_;  // one linked list per vertex
  int vertexCount_;
  int edgeCount_;
};

}  // namespace dip

#endif  // DIP_GRAPH_HPP
