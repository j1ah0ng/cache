#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class CacheMode {
  kAssociative,
  kDirectMapped,
};
enum class CacheResult {
  kHit,
  kCompulsory,
  kConflict,
  kCapacity,
};

template <CacheMode mode> class Cache;

template <> class Cache<CacheMode::kAssociative> {
public:
  Cache(int log_num_lines, int set_bits, int associativity)
      : log_num_lines_{log_num_lines}, set_bits_{set_bits},
        associativity_{associativity} {}
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
  using Map = std::unordered_map<int, Vector>;

  int log_num_lines_;
  int set_bits_;
  int associativity_;

  Set seen_ = Set{};
  Vector fully_associative_ = Vector{};
  Map c_ = Map{};
};
