// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GBAEMemoryModel.h"

#include "Core/Core.h"
#include "Core/PowerPC/MMU.h"

#include "DolphinQt/Host.h"

std::array<std::string, 8> mem_vector_names = {

    "Green X | Y",
    "Red X | Y",
    "Blue X | Y",
    "Purple X | Y",
    "Gems P1 | P2",
    "Combo P1 | P2",
    "G φ R | G φ P",
    "B φ R | B φ P"
};

GBAEMemoryModel::GBAEMemoryModel(QObject* parent) : QAbstractTableModel(parent)
{
  connect(Host::GetInstance(), &Host::UpdateDisasmDialog, this, &GBAEMemoryModel::UpdateModel);
}

int GBAEMemoryModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 3;
}

int GBAEMemoryModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 8;
}

Qt::ItemFlags GBAEMemoryModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return QAbstractItemModel::flags(index);

  if ((index.column() > 0) &&(index.row() < 6))
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

  return QAbstractItemModel::flags(index);
}

QVariant GBAEMemoryModel::data(const QModelIndex& index, int role) const
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
      if (column == 0)
          return QString(tr(mem_vector_names.at(row).c_str()));
      
      if (Core::IsRunning())
      {
        if (column == 1)
        {                
          if (row == 0)  // Green X
            return gbae_memory_vector[0];
          if (row == 1) // Red X
            return gbae_memory_vector[2];
          if (row == 2) // Blue X
            return gbae_memory_vector[4];
          if (row == 3) // Purple X
            return gbae_memory_vector[6];
          if (row == 4) // P1 Gems
            return gbae_memory_vector[8];
          if (row == 5)  // P1 Combo
            return gbae_memory_vector[10];
          if (row == 6)  // Angle Green-Red
            return gbae_memory_vector[12];
          if (row == 7)  // Angle Blue-Red
            return gbae_memory_vector[14];
        }
        // else (column == 2)
        if (row == 0)  // Green Y
          return gbae_memory_vector[1];
        if (row == 1)  // Red Y
          return gbae_memory_vector[3];
        if (row == 2)  // Blue Y
          return gbae_memory_vector[5];
        if (row == 3)  // Purple Y
          return gbae_memory_vector[7];
        if (row == 4)  // P2 Gems
          return gbae_memory_vector[9];
        if (row == 5)  // P2 Combo
          return gbae_memory_vector[11];
        if (row == 6)  // Angle Green-Purple
          return gbae_memory_vector[13];
        if (row == 7)  // Angle Blue-Purple
          return gbae_memory_vector[15];
      }
      else
        return tr("Offline");
  }

  // distinguish some columns visually by colour
  if (role == Qt::BackgroundRole)
  {
    if (row == 0)  // Green
      return QBrush(QColor(0, 180, 0, 45));
    if (row == 1)  // Red
      return QBrush(QColor(180, 0, 0, 45));
    if (row == 2)  // Blue
      return QBrush(QColor(0, 0, 180, 45));
    if (row == 3)  // Purple
      return QBrush(QColor(180, 0, 180, 45));
    if (row == 4)
      return QBrush(QColor("LightGray"));
    if (row == 5)
      return QBrush(QColor("LightGray"));
    if (row == 6)
    {
      if (column == 0)
        return QBrush(QColor("LightGray"));
      if (column == 1)  // G<->R
      {
        QLinearGradient gradient(0, 0, 80, 0);
        gradient.setColorAt(0, QColor(0, 180, 0, 90));
        gradient.setColorAt(1, QColor(200, 0, 0, 120));
        return QBrush(gradient);
      }
      if (column == 2)  // G<->P
      {
        QLinearGradient gradient(0, 0, 80, 0);
        gradient.setColorAt(0, QColor(0, 180, 0, 90));
        gradient.setColorAt(1, QColor(180, 0, 180, 120));
        return QBrush(gradient);
      }
    }
    if (row == 7)
    {
      if (column == 0)
        return QBrush(QColor("LightGray"));
      if (column == 1)  // B<->R
      {
        QLinearGradient gradient(0, 0, 80, 0);
        gradient.setColorAt(0, QColor(0, 0, 240, 120));
        gradient.setColorAt(1, QColor(200, 0, 0, 120));
        return QBrush(gradient);
      }
      if (column == 2)  // B<->P
      {
        QLinearGradient gradient(0, 0, 80, 0);
        gradient.setColorAt(0, QColor(0, 0, 240, 120));
        gradient.setColorAt(1, QColor(180, 0, 180, 120));
        return QBrush(gradient);
      }
    }
  }
  return QVariant();
}

bool GBAEMemoryModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && role == Qt::EditRole)
  {
    unsigned int column = index.column();
    unsigned int row = index.row();

    if (column < 1)
      return false;

    if (Core::GetState() == Core::State::Uninitialized)
      return false;

    if (column == 1)
    {
      if (row == 0) // Green X
      {
        PowerPC::HostWrite_F32(value.toFloat(),
                               PowerPC::HostRead_U32(PowerPC::HostRead_U32(m_mem_base)
                               + 0x04) + 0x08);
        gbae_memory_vector[0] = value.toFloat(); // seems unnecessary but is for updating table right away
      }
      if (row == 1)  // Red X
      {     
        PowerPC::HostWrite_F32(value.toFloat(),
                               PowerPC::HostRead_U32(PowerPC::HostRead_U32(m_mem_base)
                               + 0x08) + 0x08);
        gbae_memory_vector[2] = value.toFloat();
      }
      if (row == 2)  // Blue X
      {
        PowerPC::HostWrite_F32(value.toFloat(),
                               PowerPC::HostRead_U32(PowerPC::HostRead_U32(m_mem_base)
                               + 0x0C) + 0x08);
        gbae_memory_vector[4] = value.toFloat();
      }
      if (row == 3)  // Purple X
      {
        PowerPC::HostWrite_F32(value.toFloat(),
                               PowerPC::HostRead_U32(PowerPC::HostRead_U32(m_mem_base)
                               + 0x10) + 0x08);
        gbae_memory_vector[6] = value.toFloat();
      }
      if (row == 4)  // Gems P1
      {
        PowerPC::HostWrite_U16(value.toInt(), PowerPC::HostRead_U32(m_mem_base) + 0xC8A);
        gbae_memory_vector[8] = value.toInt();
      }
      if (row == 5)  // Combo P1
      {
        PowerPC::HostWrite_U16(value.toInt(), PowerPC::HostRead_U32(m_mem_base) + 0xCFB);
        gbae_memory_vector[10] = value.toInt();
      }
    }
    else // else column == 2
    {
      if (row == 0)  // Green Y
      {
        PowerPC::HostWrite_F32(value.toFloat(),
                               PowerPC::HostRead_U32(PowerPC::HostRead_U32(m_mem_base)
                               + 0x04) + 0x0C);
        gbae_memory_vector[1] = value.toFloat();
      }
      if (row == 1)  // Red Y
      {
        PowerPC::HostWrite_F32(value.toFloat(),
                               PowerPC::HostRead_U32(PowerPC::HostRead_U32(m_mem_base)
                               + 0x08) + 0x0C);
        gbae_memory_vector[3] = value.toFloat();
      }
      if (row == 2)  // Blue Y
      {
        PowerPC::HostWrite_F32(value.toFloat(),
                               PowerPC::HostRead_U32(PowerPC::HostRead_U32(m_mem_base)
                               + 0x0C) + 0x0C);
        gbae_memory_vector[5] = value.toFloat();
      }
      if (row == 3)  // Purple Y
      {
        PowerPC::HostWrite_F32(value.toFloat(),
                               PowerPC::HostRead_U32(PowerPC::HostRead_U32(m_mem_base)
                               + 0x10) + 0x0C);
        gbae_memory_vector[7] = value.toFloat();
      }
      if (row == 4)  // Gems P2
      {
        PowerPC::HostWrite_U8(value.toInt(), PowerPC::HostRead_U32(m_mem_base) + 0x2556);
        gbae_memory_vector[9] = value.toInt();
      }
      if (row == 5)  // Combo P2
      {
        PowerPC::HostWrite_U8(value.toDouble(), PowerPC::HostRead_U32(m_mem_base) + 0x25C7);
        gbae_memory_vector[11] = value.toInt();
      }
    }
    emit dataChanged(index, index, {role});
    return true;
  }
  return false;
}

u32 GBAEMemoryModel::ReturnLevelTwoAddress(u32 base, u32 p1, u32 p2)
{
  return PowerPC::HostRead_U32(PowerPC::HostRead_U32(base) + p1) + p2;
}

u32 GBAEMemoryModel::ReturnLevelOneAddress(u32 base, u32 p1)
{
  return PowerPC::HostRead_U32(base) + p1;
}

void GBAEMemoryModel::UpdateModel()
{
  gbae_memory_vector[0] =  // Green X
      PowerPC::HostRead_F32(ReturnLevelTwoAddress(0x80531948, 0x04, 0x08));
  gbae_memory_vector[1] =  // Green Y
      PowerPC::HostRead_F32(ReturnLevelTwoAddress(0x80531948, 0x04, 0x0C));
  gbae_memory_vector[2] =  // Red X
      PowerPC::HostRead_F32(ReturnLevelTwoAddress(0x80531948, 0x08, 0x08));
  gbae_memory_vector[3] =  // Red Y
      PowerPC::HostRead_F32(ReturnLevelTwoAddress(0x80531948, 0x08, 0x0C));
  gbae_memory_vector[4] =  // Blue X
      PowerPC::HostRead_F32(ReturnLevelTwoAddress(0x80531948, 0x0C, 0x08));
  gbae_memory_vector[5] =  // Blue Y
      PowerPC::HostRead_F32(ReturnLevelTwoAddress(0x80531948, 0x0C, 0x0C));
  gbae_memory_vector[6] =  // Purple X
      PowerPC::HostRead_F32(ReturnLevelTwoAddress(0x80531948, 0x10, 0x08));
  gbae_memory_vector[7] =  // Purple Y
      PowerPC::HostRead_F32(ReturnLevelTwoAddress(0x80531948, 0x10, 0x0C));
  gbae_memory_vector[8] =  // Gems P1
      PowerPC::HostRead_U16(ReturnLevelOneAddress(0x80531948, 0xC8A));
  gbae_memory_vector[9] =  // Gems P2
      PowerPC::HostRead_U16(ReturnLevelOneAddress(0x80531948, 0x2556));
  gbae_memory_vector[10] =  // Combo P1
      PowerPC::HostRead_U8(ReturnLevelOneAddress(0x80531948, 0xCFB));
  gbae_memory_vector[11] =  // Combo P2
      PowerPC::HostRead_U8(ReturnLevelOneAddress(0x80531948, 0x25C7));
  gbae_memory_vector[12] =  // Angle Green-Red
      CalculateAngle(gbae_memory_vector[0], gbae_memory_vector[1],
                     gbae_memory_vector[2], gbae_memory_vector[3]);
  gbae_memory_vector[13] =  // Angle Green-Purple
      CalculateAngle(gbae_memory_vector[0], gbae_memory_vector[1],
                     gbae_memory_vector[6], gbae_memory_vector[7]);
  gbae_memory_vector[14] =  // Angle Blue-Red
      CalculateAngle(gbae_memory_vector[4], gbae_memory_vector[5],
                     gbae_memory_vector[2], gbae_memory_vector[3]);
  gbae_memory_vector[15] =  // Angle Blue-Purple
      CalculateAngle(gbae_memory_vector[4], gbae_memory_vector[5],
                     gbae_memory_vector[6], gbae_memory_vector[7]);

  emit dataChanged(index(0, 0), index(8, 2), {Qt::DisplayRole});
}

float GBAEMemoryModel::CalculateAngle(float linkOne_x, float linkOne_y,
                                      float linkTwo_x,float linkTwo_y)
{
  float delta_x = linkOne_x - linkTwo_x;
  float delta_y = linkOne_y - linkTwo_y;

  if (delta_x < 0)
    delta_x = delta_x * -1;

  if (delta_y < 0)
    delta_y = delta_y * -1;

  if (delta_x == 0 || delta_y == 0)
    return 0;
  else
  {
    float angle = qRadiansToDegrees(qAtan(delta_y / delta_x));
    if (angle > 45)
      return (90 - angle);
    else
      return angle;
  }
}
