#include <vector>
#include <algorithm>

struct node {
  std::vector<int> adj_list;
  long score;
  long area;
  long weight;
};

struct graph {
  std::vector<node *> nodes;
  int edges;

  void init(int num_nodes)
  {
    nodes.resize(num_nodes);
    for (int i = 0; i < num_nodes; i++) {
      nodes[i] = new node;
    }
    edges = 0;
  }

  ~graph()
  {
    for (int i = 0; i < nodes.size(); i++) {
      delete nodes[i];
    }
  }

  void set_attrs(int u, long score, long area, long weight)
  {
    nodes[u]->score = score;
    nodes[u]->area = area;
    nodes[u]->weight = weight;
  }

  void add_edge(int u, int v)
  {
    if (std::find(nodes[u]->adj_list.begin(), nodes[u]->adj_list.end(), v) == nodes[u]->adj_list.end()) {
      nodes[u]->adj_list.push_back(v);
      nodes[v]->adj_list.push_back(u);
      edges += 2;
    }
  }

  void invert()
  {
    edges = 0;
    for (int i = 0; i < nodes.size(); i++) {
      int pos = 0;
      int size = nodes[i]->adj_list.size();
      std::sort(nodes[i]->adj_list.begin(), nodes[i]->adj_list.end());
      for (int u = 0; u < nodes.size(); u++) {
        if (pos < size && nodes[i]->adj_list[pos] == u)
          pos++;
        else if (u != i)
          nodes[i]->adj_list.push_back(u);
      }

      nodes[i]->adj_list.erase(nodes[i]->adj_list.begin(), nodes[i]->adj_list.begin() + size);
      edges += nodes[i]->adj_list.size();
    }
  }
};

// Local variables:
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
