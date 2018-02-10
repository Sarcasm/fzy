#include "fzy.h"

#include <QtTest/QtTest>

class TestFzy : public QObject {
  Q_OBJECT

private slots:
  void testStartsWith() {
    QVERIFY(fzy::startsWith("", "ab"));
    QVERIFY(fzy::startsWith("a", "ab"));
    QVERIFY(fzy::startsWith("ab", "ab"));
    QVERIFY(!fzy::startsWith("aa", "ab"));
  }

  void testSubstr() {
    QVERIFY(fzy::substrSearch("", "ab"));
    QVERIFY(fzy::substrSearch("a", "ab"));
    QVERIFY(fzy::substrSearch("b", "ab"));
    QVERIFY(fzy::substrSearch("bd", "abde"));
    QVERIFY(!fzy::substrSearch("ae", "abde"));
    QVERIFY(!fzy::substrSearch("x", "abde"));
  }
};

QTEST_MAIN(TestFzy)
#include "tst-fzy.moc"
