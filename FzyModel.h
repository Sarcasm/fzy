#ifndef FZY_FZYMODEL_H
#define FZY_FZYMODEL_H

#include <string>
#include <string_view>
#include <vector>

#include <QAbstractListModel>

class FzyModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(
      QString filter MEMBER m_filter WRITE setFilter NOTIFY filterChanged)

public:
  enum class Role : int {
    Value = Qt::UserRole + 1,
    MatchIndices,
  };
  Q_ENUM(Role);

  FzyModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

signals:
  void filterChanged(const QString &newFilter);

private:
  void setFilter(const QString &newFilter);

private:
  QString m_filter;
  std::vector<std::string> m_strings;
  std::vector<std::string_view> m_filterView;
};

#endif // FZY_FZYMODEL_H
