// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "InputCommon/GCPadStatus.h"

#include <deque>
#include <vector>

namespace Schneeheide
{
class GBAEditorInputs
{
public:

  GBAEditorInputs();

  static void ApplyGBAEditorInputs(int device_number, GCPadStatus* pad_status);
  static void ToggleInputProcessing();

  static std::deque<std::vector<bool>> frameDeque;
  static bool process_inputs;
};
}  // namespace Schneeheide
