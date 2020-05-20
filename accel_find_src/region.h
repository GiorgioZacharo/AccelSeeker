#include <vector>

struct region {
  std::vector<int> bb_list;
  char *fname;
  char *rname;
  long score;
  long area;
  long weight;
};

void region_parse_file(const char *fname, std::vector<region *> &regions, bool use_density);
bool region_disjoint(region *r1, region *r2);

// Local variables:
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
