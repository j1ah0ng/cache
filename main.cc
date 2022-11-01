#include "cache.hh"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  Cache<CacheMode::kAssociative> c(4, 2, 4);
  vector<int> accesses = {
      0, 64, 1, 1, 65, 2, 2, 66, 3, 3, 67, 4,
      4, 68, 5, 5, 69, 6, 6, 70, 7, 7, 71, 8,
  };
  for (auto a : accesses) {
    auto addr = a * 4;
    auto no_offset = addr >> 4;
    auto result = c.lookup(no_offset);

    cout << a << '\t';
    switch (result) {
    case CacheResult::kHit:
      cout << "hit" << endl;
      break;
    case CacheResult::kCompulsory:
      cout << "compulsory" << endl;
      break;
    case CacheResult::kConflict:
      cout << "conflict" << endl;
      break;
    case CacheResult::kCapacity:
      cout << "capacity" << endl;
      break;
    default:
      assert(false);
    }
  }
}
