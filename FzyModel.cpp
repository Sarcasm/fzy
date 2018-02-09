#include "FzyModel.h"

#include <fstream>

FzyModel::FzyModel(QObject *parent) : QAbstractListModel(parent) {
  m_strings = {
      "upower",
      "usermod",
  };

  m_filterView.assign(m_strings.begin(), m_strings.end());
}

void FzyModel::setFilter(const QString &newFilter) {
  if (m_filter == newFilter) {
    return;
  }

  m_filter = newFilter;

  if (m_filter.isEmpty()) {
    beginRemoveRows(QModelIndex(), 2, 2);
    m_strings.resize(2);
    m_filterView.assign(m_strings.begin(), m_strings.end());
    endRemoveRows();
  } else if (m_strings.size() == 2) {
    beginInsertRows(QModelIndex(), m_strings.size(), m_strings.size());
    m_strings.resize(3);
    m_strings[2] = m_filter.toUtf8().constData();
    m_filterView.assign(m_strings.begin(), m_strings.end());
    endInsertRows();
  } else {
    m_strings[2] = m_filter.toUtf8().constData();
    m_filterView.assign(m_strings.begin(), m_strings.end());
    const QModelIndex idx = index(2);
    emit dataChanged(idx, idx);
  }


  emit filterChanged(m_filter);
}

int FzyModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return m_filterView.size();
}

// https://forum.qt.io/topic/78406/how-to-update-qabstractitemmodel-view-when-a-data-is-updated/5
QVariant FzyModel::data(const QModelIndex &index, int role) const {
  // necessary?
  if (!index.isValid()) {
    return QVariant();
  }
  Q_ASSERT(index.model() == this);
  // end necessary?

  if (index.row() < 0 ||
      static_cast<unsigned>(index.row()) >= m_filterView.size()) {
    return QVariant();
  }

  switch (role) {
  case Qt::DisplayRole:
    return QString::fromStdString(std::string(m_filterView[index.row()]));

  default:
    return QVariant();
  }
}

QVariant FzyModel::headerData(int section, Qt::Orientation orientation,
                              int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Element");

    // TODO: match characters
    // case 1:
    //   return tr("Address");

    default:
      return QVariant();
    }
  }

  return QVariant();
}
