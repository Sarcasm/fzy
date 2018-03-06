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
    QTest::addColumn<std::string_view>("haystack");
    QTest::addColumn<bool>("found");
    QTest::addColumn<std::vector<int>>("highlights");

    auto row = [this](auto name, std::string_view needle,
                      std::string_view haystack, bool found,
                      const std::vector<int> &highlights) {
      QTest::newRow(name) << needle << haystack << found << highlights;
    };

    row("test1", "abc", "abcabc", true, {0, 1, 2});
    row("test2", "abc", "aabbcc", true, {0, 2, 4});
  }

  void fuzzyMatch() {
    QFETCH(std::string_view, needle);
    QFETCH(std::string_view, haystack);
    QFETCH(bool, found);
    QFETCH(std::vector<int>, highlights);

    QCOMPARE(fzy::fuzzySearch(needle, haystack), found);

    std::vector<int> matchSet = {1, 2, 3};
    fzy::fuzzyHighlights(needle, haystack, matchSet);
    QCOMPARE(matchSet, highlights);
  }
};

QTEST_MAIN(TestFzy)
#include "tst-fzy.moc"
