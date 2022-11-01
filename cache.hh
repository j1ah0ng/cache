#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class CacheMode {
  kAssociative,
  kFullyAssociative,
  kDirectMapped,
};
enum class CacheResult {
  kHit,
  kCompulsory,
  kConflict,
  kCapacity,
};

inline std::ostream &operator<<(std::ostream &os, CacheResult value) {
  switch (value) {
  case CacheResult::kHit:
    return os << "hit";
    break;
  case CacheResult::kCompulsory:
    return os << "compulsory";
    break;
  case CacheResult::kConflict:
    return os << "conflict";
    break;
  case CacheResult::kCapacity:
    return os << "capacity";
    break;
  default:
    assert(false);
  }
}

namespace {

class CacheBase {
public:
  CacheBase(int log_num_lines) : log_num_lines_{log_num_lines} {}
  bool full(int query) {
    auto it =
        std::find(fully_associative_.begin(), fully_associative_.end(), query);
    if (it == fully_associative_.end()) {
      // didn't find
      if (fully_associative_.size() >= (1 << log_num_lines_)) {
        fully_associative_.erase(fully_associative_.begin());
      }
      fully_associative_.emplace_back(query);
      return false;
    } else {
      // found
      fully_associative_.erase(it);
      fully_associative_.emplace_back(query);
      return true;
    }
  }

  bool check_is_compulsory(int query) {
    if (seen_.find(query) != seen_.end()) {
      return false;
    } else {
      seen_.insert(query);
      return true;
    }
  }

  using Set = std::unordered_set<int>;
  using Vector = std::vector<int>;

  int log_num_lines_;

  Set seen_ = Set{};
  Vector fully_associative_ = Vector{};
};
} // anonymous namespace

template <CacheMode mode> class Cache;

template <> class Cache<CacheMode::kDirectMapped> : private CacheBase {
public:
  /***
   * @param log_num_lines log-2 of the number of lines in the cache
   * */
  Cache(int log_num_lines) : CacheBase{log_num_lines} {}

  CacheResult lookup(int query) {
    auto tag = query >> log_num_lines_;
    auto set_idx = (tag << log_num_lines_) ^ query;
    auto is_compulsory = check_is_compulsory(query);
    auto full_hit = full(query);

    auto it = c_.find(set_idx);
    if (it == c_.end()) {
      // didn't find
      assert(is_compulsory);
      c_.emplace(std::make_pair(set_idx, tag));
      return CacheResult::kCompulsory;
    } else {
      auto candidate = it->second;
      if (candidate == tag) {
        return CacheResult::kHit;
      } else {
        // miss
        c_.erase(it);
        c_.emplace(std::make_pair(set_idx, tag));
        return (is_compulsory) ? CacheResult::kCompulsory
               : (full_hit)    ? CacheResult::kConflict
                               : CacheResult::kCapacity;
      }
    }
  }

private:
  std::unordered_map<int, int> c_ = {};
};

template <> class Cache<CacheMode::kAssociative> : private CacheBase {
public:
  /***
   * @param log_num_lines log-2 of the number of lines in the cache
   * @param set_bits number of bits for set index
   * @param associativity the associativity of the cache
   * */
  Cache(int log_num_lines, int set_bits, int associativity)
      : CacheBase{log_num_lines}, set_bits_{set_bits}, associativity_{
                                                           associativity} {}
  CacheResult lookup(int query) {
    auto tag = query >> set_bits_;
    auto set_idx = (tag << set_bits_) ^ query;
    auto is_compulsory = check_is_compulsory(query);
    auto full_hit = full(query);

    auto it = c_.find(set_idx);
    if (it == c_.end()) {
      assert(is_compulsory);
      c_.emplace(std::make_pair(set_idx, Vector{tag}));
      return CacheResult::kCompulsory;
    } else {
      auto &v = it->second;
      auto iit = std::find(v.begin(), v.end(), tag);

      if (iit == v.end()) {
        // didn't find
        if (v.size() >= associativity_) {
          v.erase(v.begin());
        }
        v.emplace_back(tag);
        return (is_compulsory) ? CacheResult::kCompulsory
               : (full_hit)    ? CacheResult::kConflict
                               : CacheResult::kCapacity;
      } else {
        // found
        v.erase(iit);
        v.emplace_back(tag);
        return CacheResult::kHit;
      }
    }
  }

private:
  using Map = std::unordered_map<int, Vector>;
  Map c_ = Map{};

  int set_bits_;
  int associativity_;
};
