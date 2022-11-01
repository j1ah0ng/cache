#include "cache.hh"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  Cache<CacheMode::kAssociative> c(4, 2, 4);
  Cache<CacheMode::kDirectMapped> d(4);
  vector<int> accesses = {
      0, 64, 1, 1, 65, 2, 2, 66, 3, 3, 67, 4,
      4, 68, 5, 5, 69, 6, 6, 70, 7, 7, 71, 8,
  };

  cout << "----- 4-way associative -----" << endl;
  for (auto a : accesses) {
    auto addr = a * 4;
    auto no_offset = addr >> 4;
    auto result = c.lookup(no_offset);

    cout << a << '\t' << result << endl;
  }

  cout << endl << endl << "----- direct-mapped -----" << endl;
  for (auto a : accesses) {
    auto addr = a * 4;
    auto no_offset = addr >> 4;
    auto result = d.lookup(no_offset);

    cout << a << '\t' << result << endl;
  }
}
