#include "fzy.h"

#include <algorithm>
#include <cassert>
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

bool simpleFuzzySearch(std::string_view needle, std::string_view haystack) {
  std::string_view::size_type pos = 0;

  if (needle.size() > haystack.size()) {
    return false;
  }

  for (auto ch : needle) {
    pos = haystack.find(ch, pos);
    if (pos == std::string_view::npos) {
      return false;
    }

    ++pos;
  }

  return true;
}

void simpleFuzzyHighlights(std::string_view needle, std::string_view haystack,
                           std::vector<int> &matches) {
  matches.clear();
  std::string_view::size_type pos = 0;
  for (auto ch : needle) {
    pos = haystack.find(ch, pos);
    if (pos == std::string_view::npos) {
      matches.clear();
      return;
    }

    matches.push_back(pos);
    ++pos;
  }
}

bool fuzzySearch(std::string_view needle, std::string_view haystack) {
  std::string_view::size_type pos = 0;
  for (auto ch : needle) {
    pos = haystack.find(ch, pos);
    if (pos == std::string_view::npos) {
      return false;
    }

    ++pos;
  }

  return true;
}

namespace {

bool isAlpha(char ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool isAlphaNum(char ch) { return isAlpha(ch) || (ch >= '0' && ch <= '9'); }

std::vector<int> computeCharCategories(std::string_view haystack)
{
  std::vector<int> vec;
  vec.reserve(haystack.size());

  int nthWord = 0;
  bool lastIsSeperator = true;
  for (std::string_view::size_type i = 0, count = haystack.size(); i < count;
       ++i) {
    char ch = haystack[i];
    if (isAlphaNum(ch)) {
      if (lastIsSeperator) {
        ++nthWord;
        lastIsSeperator = false;
      }
      vec.push_back(nthWord);
    } else {
      lastIsSeperator = true;
      vec.push_back(0);
    }
  }

  return vec;
}

bool fuzzyLess(std::string_view haystack, const std::vector<int> &a,
               const std::vector<int> &b) {
  std::vector<int> haystackCharCategories = computeCharCategories(haystack);
  std::vector<bool> startOfWordsLUT;
  startOfWordsLUT.reserve(haystackCharCategories.size());
  int tgtNth = 1;
  for (int nthWord : haystackCharCategories) {
    if (nthWord == tgtNth) {
      ++tgtNth;
      startOfWordsLUT.push_back(true);
    } else {
      startOfWordsLUT.push_back(false);
    }
  }

  auto countWordBeginnings = [&](const auto &v) {
    return std::count_if(v.begin(), v.end(),
                         [&](const auto &idx) { return startOfWordsLUT[idx]; });
  };

  return countWordBeginnings(a) > countWordBeginnings(b);
}

} // unnamed namespace

void fuzzyHighlights(std::string_view needle, std::string_view haystack,
                     std::vector<int> &matches) {
  matches.clear();

  std::vector<int> minMatches;
  std::string_view::size_type pos = 0;
  for (auto ch : needle) {
    pos = haystack.find(ch, pos);
    if (pos == std::string_view::npos) {
      // FIXME: should not happen if matched initially, could assert?
      return;
    }

    minMatches.push_back(pos);
    ++pos;
  }

  std::vector<std::vector<int>> vecs;

  for (int minMatch : minMatches) {
    vecs.push_back({minMatch});
  }

  int maxPos = haystack.size();
  for (unsigned i = vecs.size(); i != 0; --i) {
    std::vector<int> &vec = vecs[i - 1];
    char ch = needle[i - 1];

    for (int j = vec[0] + 1; j < maxPos; ++j) {
      if (haystack[j] == ch) {
        vec.push_back(j);
      }
    }
    maxPos = vec.back();
  }

  std::vector<int> mins(vecs.size());
  std::vector<int> indices(vecs.size());
  bool done = false;

  auto increment = [&mins, &indices, &vecs, &done]() mutable {
    for (int i = vecs.size() - 1; i >= 0; --i) {
      if ((indices[i] + 1) == static_cast<int>(vecs[i].size())) {
        indices[i] = mins[i];
        continue;
      }

      ++indices[i];

      if ((i + 1) != static_cast<int>(vecs.size())) {
        while (vecs[i][indices[i]] > vecs[i + 1][indices[i + 1]]) {
          ++indices[i + 1];
        }
        mins[i + 1] = indices[i + 1];
      }

      return;
    }

    done = true;
  };

  // could be more optimal
  matches = minMatches;

  increment(); // skip minMatches
  for (; !done; increment()) {
    std::vector<int> it;
    for (unsigned i = 0; i < vecs.size(); ++i) {
      it.push_back(vecs[i][indices[i]]);
    }

    if (fuzzyLess(haystack, it, matches)) {
      matches = it;
    }
  }
}

} // namespace fzy
