#include <algorithm>
#include <stdio.h>
#include "bitset.h"
#include "graph.h"
#include "region.h"
#include <unistd.h>
#include <iostream>
#include <fstream>

std::ofstream RegionFile; // File that Number of Regions/BBs/Funcs are written.

struct mis_ctx {
  WORD *config;
  WORD *nodes_left;
  unsigned count;
  unsigned calls;
  long score;
  long area;
  long weight;
  long best_weight;
  long max_area;

  mis_ctx(int nodes, long max_area)
  {
    config = bit_alloc(nodes);
    nodes_left = bit_alloc(nodes);
    count = 0;
    calls = 0;
    score = 0;
    area = 0;
    weight = 0;
    best_weight = 0;
    this->max_area = max_area;
  }
};

static inline void
bit_dump(WORD *name, int bits)
{
  for (int i = 0; i < bits; i++)
    if (bit_test(name, i))
      printf("%d,", i);
}

// Giorgio Start
static inline void
nodes_dump(WORD *name, int bits, long max_area)
{
  int region_selection = 0;

  for (int i = 0; i < bits; i++)
    if (bit_test(name, i))
      region_selection++;

  printf("  %d", region_selection);

  // Write to the RegionFile
  RegionFile.open ("NumberOfSelections.txt", std::fstream::app);
  RegionFile << region_selection << "\t" << max_area << "\n";
  RegionFile.close();
}
// Giorgio End

void mis_visit_bk(graph *graph, struct mis_ctx *ctx, bool find)
{
  int num_nodes = graph->nodes.size();

  ctx->calls++;

  if (ctx->area > ctx->max_area)
    return;

  if (find) {
    if (ctx->weight > ctx->best_weight)
      ctx->best_weight = ctx->weight;
  } else if (ctx->weight == ctx->best_weight) {
      // printf("this is madness!\n");
    ctx->count++;
    printf("%ld %ld ",ctx->area, ctx->score);
    bit_dump(ctx->config, graph->nodes.size());
    nodes_dump(ctx->config, graph->nodes.size(), ctx->max_area);
    printf("/%ld ", graph->nodes.size());
    printf(" Selected ");
    // printf(" Selection Size / List Size "); 
    printf("\n");

    return;
  }

  long n = 0;
  bit_iter iter;
  bit_iter_init(&iter, ctx->nodes_left, num_nodes);
  for (;;) {
    int id = bit_iter_next(&iter);
    if (id == -1)
      break;

    n += graph->nodes[id]->weight;
  }

  if (ctx->weight < ctx->best_weight - n)
    return;

  WORD *P = bit_alloc(num_nodes);
  bit_copy(P, ctx->nodes_left, num_nodes);

  for (;;) {
    int id = bit_minimum(P, num_nodes);
    if (id == -1)
      break;

    node *node = graph->nodes[id];

    bit_copy(ctx->nodes_left, P, num_nodes);

    bit_clear(ctx->nodes_left, id);
    for (int j = 0; j < node->adj_list.size(); j++) {
      int v = node->adj_list[j];
      bit_clear(ctx->nodes_left, v);
    }

    bit_set(ctx->config, id);
    ctx->score += graph->nodes[id]->score;
    ctx->area += graph->nodes[id]->area;
    ctx->weight += graph->nodes[id]->weight;

    mis_visit_bk(graph, ctx, find);

    bit_clear(ctx->config, id);
    ctx->score -= graph->nodes[id]->score;
    ctx->area -= graph->nodes[id]->area;
    ctx->weight -= graph->nodes[id]->weight;

    bit_clear(P, id);
  }
  free(P);
}

void find_mis(std::vector<region *> &regions, long max_area)
{
  graph graph;
  graph.init(regions.size());
  for (int i = 0; i < regions.size(); i++) {
    graph.set_attrs(i, regions[i]->score, regions[i]->area, regions[i]->weight);
    for (int j = 0; j < regions.size(); j++)
      if (i < j && !region_disjoint(regions[i], regions[j])) {
        //fprintf(stderr, "regions %d %d overlap\n", i+1, j+1);
        graph.add_edge(i, j);
      }
  }

  struct mis_ctx ctx(graph.nodes.size(), max_area);

  for (int i = 0; i < graph.nodes.size(); i++)
    bit_set(ctx.nodes_left, i);
  mis_visit_bk(&graph, &ctx, true);

  for (int i = 0; i < graph.nodes.size(); i++)
    bit_set(ctx.nodes_left, i);
  mis_visit_bk(&graph, &ctx, false);

  //fprintf(stderr, "nodes: %lu edges: %d\n", graph.nodes.size(), graph.edges / 2);
  //fprintf(stderr, "#MIS: %d calls: %d\n", ctx.count, ctx.calls);
}

void find_greedy(std::vector<region *> &regions, long max_area)
{
  std::vector<int> output;
  long score = 0;
  long area = 0;

  for (int i = 0; i < regions.size(); i++) {
    bool add = true;

    if (area + regions[i]->area > max_area)
      add = false;
    else
      for (int j = 0; j < output.size(); j++)
        if (!region_disjoint(regions[i], regions[output[j]])) {
          add = false;
          break;
        }

    if (add) {
      output.push_back(i);
      score += regions[i]->score;
      area += regions[i]->area;
    }
  }

  printf("%ld %ld ", area, score);
  for (int i = 0; i < output.size(); i++)
    printf("%d,", output[i]);
  printf("\n");
}

int main(int argc, char **argv)
{
  bool use_density = false;
  bool use_greedy = false;
  int c;
  while ((c = getopt(argc, argv, "dg")) != -1) {
    switch (c) {
    case 'd':
      use_density = true;
      break;
    case 'g':
      use_greedy = true;
      break;
    }
  }
  argc -= optind - 1;
  argv += optind - 1;

  if (argc < 3) {
    printf("Usage: region-find [OPTIONS] FILE MAX_AREA\n"
            "  -d use density weights\n"
            "  -g use greedy search\n"
      );
    return 1;
  }

  std::vector<region *> regions;
  region_parse_file(argv[1], regions, use_density);
  long max_area = strtol(argv[2], NULL, 10);

  for (int i = 0; i < regions.size(); i++) {
    region *reg = regions[i];
    fprintf(stderr, "region %d: fname:%s rname:%s score:%ld area:%ld\n", i, reg->fname, reg->rname, reg->score, reg->area);
  }
  fprintf(stderr, "\n");

  if (use_greedy)
    find_greedy(regions, max_area);
  else
    find_mis(regions, max_area);
}

// Local variables:
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
