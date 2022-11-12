#include "GBATASInputEditorModel.h"
#include "Core/HW/SI/GBAEditorInputs.h"
#include "Core/Core.h"
#include "Core/Movie.h"

std::array<std::string, 20> header_names = {

    "Left", "Up", "Down", "Right", "A", "B", "L", "R", "Select", "Start", 
    "Left", "Up", "Down", "Right", "A", "B", "L", "R", "Select", "Start",
};

std::vector<bool> empty_input = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

GBATASInputEditorModel::GBATASInputEditorModel(QObject* parent) : QAbstractTableModel(parent)
{
  Schneeheide::GBAEditorInputs::frameDeque.assign(30, empty_input);
}

int GBATASInputEditorModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 21;
}

int GBATASInputEditorModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  u64 newRowCount = Schneeheide::GBAEditorInputs::frameDeque.size();
  return newRowCount;
}

Qt::ItemFlags GBATASInputEditorModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return QAbstractItemModel::flags(index);

  return QAbstractItemModel::flags(index);
}

QVariant GBATASInputEditorModel::data(const QModelIndex& index, int role) const
{
  unsigned int row = index.row();
  unsigned int column = index.column();

  if (!index.isValid())
    return QVariant();

  // align data-inputs to center
  if (role == Qt::TextAlignmentRole)
    return Qt::AlignCenter;

  if (role == Qt::DisplayRole)
  {
    // Framecount
    if (column == 0)
      return (row);

    // display header names as input if input is set
    if (row < Schneeheide::GBAEditorInputs::frameDeque.size())
    {
      if (Schneeheide::GBAEditorInputs::frameDeque.at(row).at(column - 1))
        return QString(tr(header_names.at(column - 1).c_str()));
      else
        return QString(tr(" "));
    }
  }

  // distinguish P1 & P2 input column visually by colour
  if (role == Qt::BackgroundRole)
  {
    u64 currFrame = Movie::GetCurrentFrame() - 1;
    if (row == currFrame)
      return QBrush(QColor(0, 0, 145, 120));

    if (column == 0)  // Frame Column
      return QBrush(QColor("LightGray"));

    if (column >= 1 && column <= 10)  // P1
    {
      if (Schneeheide::GBAEditorInputs::frameDeque.at(row).at(column - 1))
        return QBrush(QColor(45, 180, 45, 90));  // darken input cells

      return QBrush(QColor(0, 180, 0, 45)); // empty inputs
    }

    // P2, remaining columns
    if (Schneeheide::GBAEditorInputs::frameDeque.at(row).at(column - 1))
      return QBrush(QColor(225, 45, 245, 90));  // darken input cells

    return QBrush(QColor(180, 0, 200, 45)); // empty inputs
  }
  return QVariant();
}

bool GBATASInputEditorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && role == Qt::EditRole)
  {
    unsigned int row = index.row();
    unsigned int column = index.column();

    if (column < 1)
      return false;

    Schneeheide::GBAEditorInputs::frameDeque.at(row).at(column - 1) = value.toInt();
    emit dataChanged(index, index, {role});
    return true;
  }
  return false;
}

QVariant GBATASInputEditorModel::headerData(int section, Qt::Orientation orientation,
                                            int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal)
  {
    if (section == 0)
      return tr("Frame");
    else
      return tr(header_names.at(section - 1).c_str());
  }

  return section + 1;
}


bool GBATASInputEditorModel::insertRows(int row, int count, const QModelIndex& parent)
{
  if (row >= Schneeheide::GBAEditorInputs::frameDeque.size())
  {
    Core::DisplayMessage("Index out of bounds. Insertion was not executed.", 3000);
    return false;
  }
    
  beginInsertRows(parent, row, row + count - 1);

  for (int i = 0; i < count; i++) Schneeheide::GBAEditorInputs::frameDeque.insert(
      Schneeheide::GBAEditorInputs::frameDeque.begin() + row, empty_input);

  endInsertRows();

  return true;
}

bool GBATASInputEditorModel::insertOneRow(int row, const QModelIndex& parent)
{
  if (row >= Schneeheide::GBAEditorInputs::frameDeque.size())
  {
    Core::DisplayMessage("Index out of bounds. Insertion was not executed.", 3000);
    return false;
  }

  beginInsertRows(parent, row, row);

    Schneeheide::GBAEditorInputs::frameDeque.insert(
        Schneeheide::GBAEditorInputs::frameDeque.begin() + row, empty_input);

  endInsertRows();

  return true;
}

bool GBATASInputEditorModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if (row >= Schneeheide::GBAEditorInputs::frameDeque.size() ||
      row + count > Schneeheide::GBAEditorInputs::frameDeque.size())
  {
    Core::DisplayMessage("Index out of bounds. Removal was not executed.", 3000);
    return false;
  }
    
  beginRemoveRows(parent, row, row + count - 1);

  for (int i = 0; i < count; i++)
  {
    Schneeheide::GBAEditorInputs::frameDeque.erase(
        Schneeheide::GBAEditorInputs::frameDeque.begin() + row);
  }

  endRemoveRows();

  return true;
}

bool GBATASInputEditorModel::removeOneRow(int row, const QModelIndex& parent)
{
  if (row >= Schneeheide::GBAEditorInputs::frameDeque.size())
  {
    Core::DisplayMessage("Index out of bounds. Insertion was not executed.", 3000);
    return false;
  }

  beginRemoveRows(parent, row, row);

  Schneeheide::GBAEditorInputs::frameDeque.erase(
      Schneeheide::GBAEditorInputs::frameDeque.begin() + row);

  endRemoveRows();

  return true;
}

bool GBATASInputEditorModel::clearAllRows(int count, const QModelIndex& parent)
{
  beginRemoveRows(parent, 0, count - 1);
  Schneeheide::GBAEditorInputs::frameDeque.clear();
  endRemoveRows();

  return true;
}

bool GBATASInputEditorModel::insertImportRows(int count, const QModelIndex& parent)
{
  beginInsertRows(parent, 0, count - 1);
  endInsertRows();

  return true;
}

bool GBATASInputEditorModel::appendRows(int count, const QModelIndex& parent)
{
  u64 lastIndex = Schneeheide::GBAEditorInputs::frameDeque.size() - 1;

  beginInsertRows(parent, lastIndex, lastIndex+count-1);

  for (int i = 0; i < count; i++)
    Schneeheide::GBAEditorInputs::frameDeque.push_back(empty_input);

  endInsertRows();

  return true;
}

bool GBATASInputEditorModel::toggleInput(const QModelIndex& index)
{
  unsigned int row = index.row();
  unsigned int column = index.column();

  if (row >= Schneeheide::GBAEditorInputs::frameDeque.size())
    return false;

  if (column < 1)
    return false;

  bool value = Schneeheide::GBAEditorInputs::frameDeque.at(row).at(column - 1);

  Schneeheide::GBAEditorInputs::frameDeque.at(row).at(column - 1) = !value;
  emit dataChanged(index, index);
  return !value;
}
