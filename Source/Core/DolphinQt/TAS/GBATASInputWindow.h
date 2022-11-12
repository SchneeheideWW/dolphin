// Copyright 2018 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/TAS/TASInputWindow.h"

class QGroupBox;
class QSpinBox;
class TASCheckBox;
struct GCPadStatus;

class GBATASInputWindow : public TASInputWindow
{
  Q_OBJECT
public:
  explicit GBATASInputWindow(QWidget* parent, int num);
  void GetValues(GCPadStatus* pad);

private:
  TASCheckBox* m_a_button;
  TASCheckBox* m_b_button;
  TASCheckBox* m_x_button; //Reset
  TASCheckBox* m_l_button;
  TASCheckBox* m_r_button;
  TASCheckBox* m_start_button;
  TASCheckBox* m_select_button;  // Z button turns to X/Select
  TASCheckBox* m_left_button;
  TASCheckBox* m_up_button;
  TASCheckBox* m_down_button;
  TASCheckBox* m_right_button;
  QGroupBox* m_dpad_box;
  QGroupBox* m_triggers_box;
  QGroupBox* m_buttons_box;
  //u64 compFrame = 200;
};
