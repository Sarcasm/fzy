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

    using namespace std::literals;
    QTest::newRow("all empty") << ""sv
                               << ""sv << true << std::vector<int>{};
    QTest::newRow("empty needle") << ""sv
                                  << "ab"sv << true << std::vector<int>{};
    QTest::newRow("front") << "a"sv
                           << "ab"sv << true << std::vector<int>{0};
    QTest::newRow("back") << "b"sv
                          << "ab"sv << true << std::vector<int>{1};
    QTest::newRow("middle") << "bd"sv
                            << "abde"sv << true << std::vector<int>{1, 2};
    QTest::newRow("full match") << "ab"sv
                                << "ab"sv << true << std::vector<int>{0, 1};
    QTest::newRow("no match") << "ae"sv
                              << "abde"sv << false << std::vector<int>{};
    QTest::newRow("ovni needle") << "x"sv
                                 << "ab"sv << false << std::vector<int>{};
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
};

QTEST_MAIN(TestFzy)
#include "tst-fzy.moc"
