#include "fzy.h"

#include <numeric>

namespace fzy {

bool startsWith(std::string_view prefix, std::string_view str) {
  return !str.compare(0, prefix.length(), prefix);
}

void startsWithHighlights(std::string_view prefix, std::string_view str,
                          std::vector<int> &matches) {
  if (!startsWith(prefix, str)) {
    matches.clear();
    return;
  }

  matches.resize(prefix.length());
  std::iota(matches.begin(), matches.end(), 0);
}

bool substrSearch(std::string_view needle, std::string_view haystack) {
  return haystack.find(needle) != std::string_view::npos;
}

void substrHighlights(std::string_view needle, std::string_view haystack,
                      std::vector<int> &matches) {
  const auto start_pos = haystack.find(needle);
  if (start_pos == std::string_view::npos) {
    matches.clear();
    return;
  }

  matches.resize(needle.length());
  std::iota(matches.begin(), matches.end(), start_pos);
}

} // namespace fzy
