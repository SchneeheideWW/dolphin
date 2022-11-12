// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractTableModel>
#include <QBrush>

class GBAEMemoryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    GBAEMemoryModel(QObject* parent = Q_NULLPTR);

    int columnCount(const QModelIndex& parent) const;
    int rowCount(const QModelIndex& parent) const;

    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);

    u32 m_mem_base = 0x80531948;
    std::vector<float> gbae_memory_vector = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    void UpdateModel();
    u32 ReturnLevelTwoAddress(u32 base, u32 p1, u32 p2);
    u32 ReturnLevelOneAddress(u32 base, u32 p1);
    float CalculateAngle(float linkOne_x, float linkOne_y, float linkTwo_x, float linkTwo_y);
};
