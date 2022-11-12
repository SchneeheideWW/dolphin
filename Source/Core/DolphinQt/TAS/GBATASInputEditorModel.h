// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractTableModel>
#include <QBrush>


class GBATASInputEditorModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    GBATASInputEditorModel(QObject* parent = Q_NULLPTR);

    int columnCount(const QModelIndex& parent) const;
    int rowCount(const QModelIndex& parent) const;

    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool insertOneRow(int row, const QModelIndex& parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeOneRow(int row, const QModelIndex& parent = QModelIndex());
    bool clearAllRows(int count, const QModelIndex& parent = QModelIndex());
    bool insertImportRows(int count, const QModelIndex& parent = QModelIndex());
    bool appendRows(int count, const QModelIndex& parent);
    bool toggleInput(const QModelIndex& index);
};
