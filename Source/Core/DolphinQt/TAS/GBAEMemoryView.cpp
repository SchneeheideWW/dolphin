// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/TAS/GBAEMemoryModel.h"
#include "DolphinQt/TAS/GBAEMemoryView.h"

#include <QApplication>
#include <QClipboard>

GBAEMemoryView::GBAEMemoryView(QWidget* parent) : QTableView(parent)
{
  verticalHeader()->setVisible(false);
  verticalHeader()->setDefaultSectionSize(verticalHeader()->minimumSectionSize());
  verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  horizontalHeader()->setVisible(false);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  setSelectionBehavior(QAbstractItemView::SelectItems);
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setShowGrid(true);

  gbaEMemoryModel = new GBAEMemoryModel();
  setModel(gbaEMemoryModel);
}

void GBAEMemoryView::keyPressEvent(QKeyEvent* event)
{
  if (event->matches(QKeySequence::Copy))
  {
    QString text;
    QItemSelectionRange copy_range = selectionModel()->selection().first();
    for (auto i = copy_range.top(); i <= copy_range.bottom(); ++i)
    {
      QStringList rowContents;
      for (auto j = copy_range.left(); j <= copy_range.right(); ++j)
        rowContents << gbaEMemoryModel->index(i, j).data().toString();
      text += rowContents.join(tr("\t"));
      text += tr("\n");
    }
    QApplication::clipboard()->setText(text);
  }

  if (event->matches(QKeySequence::SelectAll))
  {
    QItemSelection selection(gbaEMemoryModel->index(0, 0), gbaEMemoryModel->index(7, 2));
    selectionModel()->select(selection, QItemSelectionModel::Select);
  }
}
