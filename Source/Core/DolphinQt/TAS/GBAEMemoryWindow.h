// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/TAS/GBAEMemoryView.h"

class GBAEMemoryWindow : public QWidget
{
  Q_OBJECT
public:
  explicit GBAEMemoryWindow(QWidget* parent = nullptr);

  GBAEMemoryView* gbaEMemoryView;
};
