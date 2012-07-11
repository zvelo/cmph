#include <cassert>

#include "trigraph.h"

using cxxmph::TriGraph;

int main(int argc, char** argv) {
  TriGraph g(4, 2);
  g.AddEdge(TriGraph::Edge(0, 1, 2));
  g.AddEdge(TriGraph::Edge(1, 3, 2));
  assert(g.vertex_degree()[0] == 1);
  assert(g.vertex_degree()[1] == 2);
  assert(g.vertex_degree()[2] == 2);
  assert(g.vertex_degree()[3] == 1);
  g.RemoveEdge(0);
  assert(g.vertex_degree()[0] == 0);
  assert(g.vertex_degree()[1] == 1);
  assert(g.vertex_degree()[2] == 1);
  assert(g.vertex_degree()[3] == 1);
  std::vector<TriGraph::Edge> edges;
  g.ExtractEdgesAndClear(&edges);
}
