#include "FzyModel.h"

#include <numeric>
#include <vector>

#include <QMetaEnum>

namespace {

bool startsWith(std::string_view prefix, std::string_view str) {
  return !str.compare(0, prefix.length(), prefix);
}

void startsWithHighlights(std::string_view prefix, std::string_view str,
                       std::vector<int> &matches) {
  if (!startsWith(prefix, str))
    return;

  matches.resize(prefix.length());
  std::iota(matches.begin(), matches.end(), 0);
}

} // unnamed namespace

FzyModel::FzyModel(QObject *parent) : QAbstractListModel(parent) {
  m_strings = {
      "upower",
      "usermod",
      "dolphin-emu-qt2",
      "rst2odt.py",
      "pinky",
      "gst-inspect-1.0",
      "thresholder.py",
      "ttf2pk",
      "ldbdel",
      "pdfcrop",
      "sponge",
      "naming.py",
      "kdostartupconfig5",
      "repstopdf",
      "xmode2",
      "cmsutil",
      "ag",
      "gvfs-monitor-file",
      "ninja",
      "symilar",
      "llvm-lib",
      "jay",
      "kshell5",
      "drill",
      "tftopl",
      "tox-quickstart",
      "fuser",
      "gm",
      "filan",
      "vde_autolink",
      "updvitomp",
      "makeglossaries-lite",
      "rst2xetex",
      "ocsptool",
      "gvfs-save",
      "as",
      "weston-editor",
      "emacs",
      "emacsclient",
      "weston-confine",
      "dolphin",
      "combine",
      "sim_server",
      "zvbi-chains",
      "sourceweb",
      "cdda-player",
      "sgml2xml",
      "kpsexpand",
      "opencv_waldboost_detector",
      "ntfscp",
      "ttf2tfm",
      "pdflatexpicscale",
  };

  m_filterView.assign(m_strings.begin(), m_strings.end());
}

void FzyModel::setFilter(const QString &newFilter) {
  if (m_filter == newFilter) {
    return;
  }

  beginResetModel();

  m_filter = newFilter;
  m_stdFilter = m_filter.toStdString();
  emit filterChanged(m_filter);

  m_filterView.clear();
  const QByteArray &ba = m_filter.toUtf8();
  for (const auto &s : m_strings) {
    if (startsWith(m_stdFilter, s)) {
      m_filterView.push_back(s);
    }
  }

  endResetModel();
}

int FzyModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return m_filterView.size();
}

QHash<int, QByteArray> FzyModel::roleNames() const {
  static QHash<int, QByteArray> roles;

  if (roles.isEmpty()) {
    roles = QAbstractListModel::roleNames();

    const auto &roleMeta = QMetaEnum::fromType<Role>();
    for (int i = 0; i < roleMeta.keyCount(); ++i) {
      const auto v = roleMeta.value(i);

      switch (static_cast<Role>(v)) {
      case Role::Value:
        roles[v] = QByteArrayLiteral("value");
        break;

      case Role::Highlights:
        roles[v] = QByteArrayLiteral("highlights");
        break;
      }
    }
  }

  return roles;
}

QVariant FzyModel::data(const QModelIndex &index, int role) const {
  if (index.row() < 0 ||
      static_cast<unsigned>(index.row()) >= m_filterView.size()) {
    return {};
  }

  const std::string_view value = m_filterView[index.row()];

  if (role == Qt::DisplayRole) {
    role = static_cast<int>(Role::Value);
  }

  switch (static_cast<Role>(role)) {
  case Role::Value:
    return QString::fromUtf8(value.data(), value.size());

  case Role::Highlights: {
    std::vector<int> indices;
    startsWithHighlights(m_stdFilter, value, indices);
    QList<QVariant> highlights;
    for (const auto &index : indices) {
      highlights.push_back(index);
    }
    return highlights;
  }
  }

  return {};
}
