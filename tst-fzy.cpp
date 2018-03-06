#include "fzy.h"

#include <QtTest/QtTest>

Q_DECLARE_METATYPE(std::string_view);

class TestFzy : public QObject {
  Q_OBJECT

private slots:
  void startsWith() {
    QVERIFY(fzy::startsWith("", ""));
    QVERIFY(fzy::startsWith("", "ab"));
    QVERIFY(fzy::startsWith("a", "ab"));
    QVERIFY(fzy::startsWith("ab", "ab"));
    QVERIFY(!fzy::startsWith("aa", "ab"));
  }

  void startsWithHighlights() {
    auto highlights = [](auto &&... args) {
      std::vector<int> matches;
      fzy::startsWithHighlights(args..., matches);
      return matches;
    };

    QVERIFY(highlights("", "ab").empty());
    QCOMPARE(highlights("a", "ab"), std::vector<int>{0});
    QCOMPARE(highlights("ab", "ab"), (std::vector<int>{0, 1}));
    QVERIFY(highlights("aa", "ab").empty());

    std::vector<int> matches = {1, 2, 3};
    fzy::startsWithHighlights("abc", "def", matches);
    QVERIFY(matches.empty());
  }

  void substr_data() {
    QTest::addColumn<std::string_view>("needle");
    QTest::addColumn<std::string_view>("haystack");
    QTest::addColumn<bool>("found");
    QTest::addColumn<std::vector<int>>("highlights");

    auto row = [this](auto name, std::string_view needle,
                      std::string_view haystack, bool found,
                      const std::vector<int> &highlights) {
      QTest::newRow(name) << needle << haystack << found << highlights;
    };

    row("all empty", "", "", true, {});
    row("empty needle", "", "ab", true, {});
    row("front", "a", "ab", true, {0});
    row("back", "b", "ab", true, {1});
    row("middle", "bd", "abde", true, {1, 2});
    row("full match", "ab", "ab", true, {0, 1});
    row("front and back", "ae", "abde", false, {});
    row("ovni needle", "x", "ab", false, {});
  }

  void substr() {
    QFETCH(std::string_view, needle);
    QFETCH(std::string_view, haystack);
    QFETCH(bool, found);
    QFETCH(std::vector<int>, highlights);

    QCOMPARE(fzy::substrSearch(needle, haystack), found);

    std::vector<int> matchSet = {1, 2, 3};
    fzy::substrHighlights(needle, haystack, matchSet);
    QCOMPARE(matchSet, highlights);
  }

  void simpleFuzzyMatch_data() {
    QTest::addColumn<std::string_view>("needle");
    QTest::addColumn<std::string_view>("haystack");
    QTest::addColumn<bool>("found");
    QTest::addColumn<std::vector<int>>("highlights");

    auto row = [this](auto name, std::string_view needle,
                      std::string_view haystack, bool found,
                      const std::vector<int> &highlights) {
      QTest::newRow(name) << needle << haystack << found << highlights;
    };

    row("all empty", "", "", true, {});
    row("empty needle", "", "ab", true, {});
    row("front", "a", "ab", true, {0});
    row("back", "b", "ab", true, {1});
    row("middle", "bd", "abde", true, {1, 2});
    row("full match", "ab", "ab", true, {0, 1});
    row("front and back", "ae", "abde", true, {0, 3});
    row("reversed", "ba", "ab", false, {});
    row("ovni needle", "x", "ab", false, {});
  }

  void simpleFuzzyMatch() {
    QFETCH(std::string_view, needle);
    QFETCH(std::string_view, haystack);
    QFETCH(bool, found);
    QFETCH(std::vector<int>, highlights);

    QCOMPARE(fzy::simpleFuzzySearch(needle, haystack), found);

    std::vector<int> matchSet = {1, 2, 3};
    fzy::simpleFuzzyHighlights(needle, haystack, matchSet);
    QCOMPARE(matchSet, highlights);
  }

  void fuzzyMatch_data() {
    QTest::addColumn<std::string_view>("needle");
    QTest::addColumn<std::string_view>("highlightedHaystack");

    using namespace std::literals;

    QTest::newRow("test1") << "abc"sv
                           << "[abc]abc"sv;
    QTest::newRow("test2") << "abc"sv
                           << "[a]a[b]b[c]c"sv;
  }

  void fuzzyMatch() {
    QFETCH(std::string_view, needle);
    QFETCH(std::string_view, highlightedHaystack);

    std::string haystack;
    for (char ch : highlightedHaystack) {
      if (ch != '[' && ch != ']') {
        haystack += ch;
      }
    }

    bool found = highlightedHaystack.find('[') != std::string_view::npos;
    QCOMPARE(fzy::fuzzySearch(needle, haystack), found);

    std::vector<int> matchSet = {1, 2, 3};
    fzy::fuzzyHighlights(needle, haystack, matchSet);

    std::string highlightedResult;
    bool inHighlight = false;
    for (unsigned i = 0; i < haystack.size(); ++i) {
      if (std::find(matchSet.begin(), matchSet.end(), i) != matchSet.end()) {
        if (!inHighlight) {
          highlightedResult += '[';
        }
        inHighlight = true;
      } else {
        if (inHighlight) {
          highlightedResult += ']';
        }
        inHighlight = false;
      }

      highlightedResult += haystack[i];
    }

    // convert to QString so comparison failures show the characters

    auto hlActual = QString::fromUtf8(QByteArray::fromRawData(
        highlightedResult.data(), highlightedResult.size()));

    auto hlExpected = QString::fromUtf8(QByteArray::fromRawData(
        highlightedHaystack.data(), highlightedHaystack.size()));

    QCOMPARE(hlActual, hlExpected);
  }
  }
};

QTEST_MAIN(TestFzy)
#include "tst-fzy.moc"
