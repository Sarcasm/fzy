#include "fzy.h"

#include <algorithm>
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
  for (auto ch : needle) {
    pos = haystack.find(ch, pos);
    if (pos == std::string_view::npos) {
      return false;
    }
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
  }
}

bool fuzzySearch(std::string_view needle, std::string_view haystack) {
  std::string_view::size_type pos = 0;
  for (auto ch : needle) {
    pos = haystack.find(ch, pos);
    if (pos == std::string_view::npos) {
      return false;
    }
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

  return countWordBeginnings(a) < countWordBeginnings(b);
}

} // unnamed namespace

void fuzzyHighlights(std::string_view needle, std::string_view haystack,
                     std::vector<int> &matches) {
  matches.clear();

  std::vector<std::vector<int>> vecs;
  std::vector<int> it;
  std::string_view::size_type pos = 0;
  for (auto ch : needle) {
    pos = haystack.find(ch, pos);
    if (pos == std::string_view::npos) {
      // FIXME: should not happen if matched initially, could assert?
      return;
    }

    it.push_back(pos);
  }

  vecs.push_back(it);
  it.clear();

  // find max
  for (auto chit = needle.rbegin(); chit != needle.rend(); ++chit) {
    auto ch = *chit;
    // FIXME: no need to look before the last match from previous
    auto fit = std::find(haystack.rbegin(), haystack.rend(), ch);
    // no need to check for rend()
    auto n = haystack.size() - (fit - haystack.rbegin()) - 1;
    it.push_back(n);
  }

  // one match only
  if (it == vecs[0]) {
    matches.swap(vecs[0]);
    return;
  }

  vecs.push_back(it);

  if (fuzzyLess(haystack, vecs[0], vecs[1])) {
    matches = vecs[1];
  } else {
    matches = vecs[0];
  }
}

} // namespace fzy
