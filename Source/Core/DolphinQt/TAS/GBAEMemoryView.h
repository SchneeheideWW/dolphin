// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QHeaderView>
#include <QKeyEvent>
#include <QTableView>

class GBAEMemoryModel;

class GBAEMemoryView : public QTableView
{
  Q_OBJECT

public:
  GBAEMemoryView(QWidget* parent = Q_NULLPTR);

  GBAEMemoryModel* gbaEMemoryModel;

protected:
void keyPressEvent(QKeyEvent* event) override;

};
