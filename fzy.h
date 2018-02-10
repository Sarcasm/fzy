#ifndef FZY_H
#define FZY_H

#include <string_view>
#include <vector>

namespace fzy {

bool startsWith(std::string_view prefix, std::string_view str);
void startsWithHighlights(std::string_view prefix, std::string_view str,
                          std::vector<int> &matches);

bool substrSearch(std::string_view needle, std::string_view haystack);
void substrHighlights(std::string_view needle, std::string_view haystack,
                      std::vector<int> &matches);

} // namespace fzy

#endif // FZY_H
