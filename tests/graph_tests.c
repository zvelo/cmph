#include "../src/graph.h"

#define DEBUG
#include "../src/debug.h"

int main(int argc, char **argv)
{
	graph_iterator_t it;
	cmph_uint32 i, neighbor;
	graph_t *g = graph_new(5, 10);

	fprintf(stderr, "Building random graph\n");
	for (i = 0; i < 10; ++i)
	{
		cmph_uint32 v1 = i % 5;
		cmph_uint32 v2 = (i*2) % 5;
		if (v1 == v2) continue;
		graph_add_edge(g, v1, v2);
		DEBUGP("Added edge %u %u\n", v1, v2);
	}
	graph_print(g);
	graph_del_edge(g, 4, 3);
	graph_print(g);
	graph_clear_edges(g);
	graph_print(g);
	graph_destroy(g);

	fprintf(stderr, "Building cyclic graph\n");
	g = graph_new(4, 5);
	graph_add_edge(g, 0, 3);
	graph_add_edge(g, 0, 1);
	graph_add_edge(g, 1, 2);
	graph_add_edge(g, 2, 0);
	if (!graph_is_cyclic(g))
	{
		return 1;
	}
	graph_destroy(g);

	fprintf(stderr, "Building non-cyclic graph\n");
	g = graph_new(5, 4);
	graph_add_edge(g, 0, 1);
	graph_add_edge(g, 1, 2);
	graph_add_edge(g, 2, 3);
	graph_add_edge(g, 3, 4);

	if (graph_is_cyclic(g))
	{
		return 1;
	}

	fprintf(stderr, "Checking neighbors iterator\n");
	it = graph_neighbors_it(g, 1);
	neighbor = graph_next_neighbor(g, &it);
	DEBUGP("Neighbor is %u\n", neighbor);
	if (neighbor != 0 && neighbor != 2) return 1;
	neighbor = graph_next_neighbor(g, &it);
	DEBUGP("Neighbor is %u\n", neighbor);
	if (neighbor != 0 && neighbor != 2) return 1;
	neighbor = graph_next_neighbor(g, &it);
	DEBUGP("Neighbor is %u\n", neighbor);
	if (neighbor != GRAPH_NO_NEIGHBOR) return 1;


	graph_destroy(g);
	return 0;
}
