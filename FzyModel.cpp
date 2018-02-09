#include "FzyModel.h"

#include <fstream>

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

  m_filter = newFilter;
  emit filterChanged(m_filter);

  beginResetModel();
  m_filterView.clear();
  std::string prefix = m_filter.toUtf8().constData();
  for (const std::string &s : m_strings) {
    if (!s.compare(0, prefix.length(), prefix)) {
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

QVariant FzyModel::headerData(int section, Qt::Orientation orientation,
                              int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Value");

    case 1:
      return tr("MatchIndices");

    default:
      return QVariant();
    }
  }

  return QVariant();
}

QHash<int, QByteArray> FzyModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[static_cast<int>(Role::Value)] = QByteArrayLiteral("value");
  roles[static_cast<int>(Role::MatchIndices)] = QByteArrayLiteral("matchIndices");
  return roles;
}

QVariant FzyModel::data(const QModelIndex &index, int role) const {
  if (index.row() < 0 ||
      static_cast<unsigned>(index.row()) >= m_filterView.size()) {
    return {};
  }

  switch (static_cast<Role>(role)) {
  case Role::Value:
    return QString::fromStdString(std::string(m_filterView[index.row()]));

  case Role::MatchIndices:
    return QString("some indices");
  }

  return {};
}
