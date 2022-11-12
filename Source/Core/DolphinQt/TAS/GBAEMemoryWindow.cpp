// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/TAS/GBAEMemoryWindow.h"
#include "DolphinQt/Resources.h"

#include <QGridLayout>
#include <QHeaderView>

GBAEMemoryWindow::GBAEMemoryWindow(QWidget* widget) : QWidget(widget)
{
  setWindowIcon(Resources::GetAppIcon());
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setWindowTitle(tr("GBAE Memory Editor"));
  gbaEMemoryView = new GBAEMemoryView;

  auto* layout = new QGridLayout;

  layout->addWidget(gbaEMemoryView);
  setLayout(layout);

  gbaEMemoryView->horizontalHeader()->resizeSection(0, 86);
  gbaEMemoryView->horizontalHeader()->resizeSection(1, 85);
  gbaEMemoryView->horizontalHeader()->resizeSection(2, 85);
  setMinimumSize(280, 216);
}
