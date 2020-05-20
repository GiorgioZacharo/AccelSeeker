#include <fstream>
#include "region.h"
#include <stdlib.h>
#include<string.h>
#include<algorithm>

static char **strsplit(const char *str, char delim)
{
  int l, n;
  char *s, *t;
  char **ret;

  s = strdup(str ? str : "");

  for (n = 1, t = s; *t; t++)
    if (*t == delim)
      n++;

  ret = (char **)malloc((n + 1) * sizeof (char *));
  ret[n] = NULL;

  ret[0] = s;
  for (n = 1, t = s; *t; t++)
    if (*t == delim) {
      *t = '\0';
      ret[n++] = t + 1;
    }

  return ret;
}

static void free_strsplit(char **ptr)
{
  free(ptr[0]);
  free(ptr);

}

static bool region_comp(const region *r1, const region *r2)
{
  return r1->weight > r2->weight;
}

void region_parse_file(const char *fname, std::vector<region *> &regions, bool use_density)
{
  std::ifstream pfile(fname);
  std::string str;
  while (std::getline(pfile, str, '\n')) {
    const char *line = str.c_str();
    char **ptr = strsplit(line, ' ');

    long score = strtol(ptr[2], NULL, 10);
    if (score == 0)
      continue;

    region *reg = new region();
    reg->fname = strdup(ptr[0]);
    reg->rname = strdup(ptr[1]);
    reg->score = score;
    reg->area = strtol(ptr[3], NULL, 10);
    reg->weight = use_density ? (reg->score / reg->area) : reg->score;

    char **bb_list = strsplit(ptr[4], ',');
    for (int i = 0; bb_list[i]; i++) {
      int v = strtol(bb_list[i], NULL, 10);
      reg->bb_list.push_back(v);
    }
    free_strsplit(bb_list);
    std::sort(reg->bb_list.begin(), reg->bb_list.end());

    regions.push_back(reg);
    free_strsplit(ptr);
  }

  std::sort(regions.begin(), regions.end(), region_comp);
}

bool region_disjoint(region *r1, region *r2)
{
  if (!strcmp(r1->fname, r2->fname)) {
      std::vector<int>::iterator it1 = r1->bb_list.begin();
      std::vector<int>::iterator it2 = r2->bb_list.begin();
      while (it1 != r1->bb_list.end() && it2 != r2->bb_list.end()) {
        if (*it1 < *it2)
          it1++;
        else if (*it2 < *it1)
          it2++;
        else
          return false;
      }
  }

  return true;
}

// Local variables:
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
