// Copyright 2018 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/TAS/GBATASInputWindow.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QSpinBox>
#include <QVBoxLayout>

#include "Common/CommonTypes.h"

#include "DolphinQt/TAS/TASCheckBox.h"

#include "InputCommon/GCPadStatus.h"

#include "Core/Movie.h"

GBATASInputWindow::GBATASInputWindow(QWidget* parent, int num) : TASInputWindow(parent)
{
  setWindowTitle(tr("GBA TAS Input %1").arg(num + 1));

  m_a_button = CreateButton(QStringLiteral("&A"));
  m_b_button = CreateButton(QStringLiteral("&B"));
  m_l_button = CreateButton(QStringLiteral("&L"));
  m_r_button = CreateButton(QStringLiteral("&R"));
  m_x_button = CreateButton(QStringLiteral("&Reset"));
  m_start_button = CreateButton(QStringLiteral("&START"));
  m_select_button = CreateButton(QStringLiteral("&SELECT"));

  m_left_button = CreateButton(QStringLiteral("L&eft"));
  m_up_button = CreateButton(QStringLiteral("&Up"));
  m_down_button = CreateButton(QStringLiteral("&Down"));
  m_right_button = CreateButton(QStringLiteral("R&ight"));

  auto* buttons_layout = new QGridLayout;
  buttons_layout->addWidget(m_a_button, 0, 0);
  buttons_layout->addWidget(m_b_button, 0, 1);
  buttons_layout->addWidget(m_l_button, 0, 2);
  buttons_layout->addWidget(m_r_button, 0, 3);
  buttons_layout->addWidget(m_start_button, 1, 0);
  buttons_layout->addWidget(m_select_button, 1, 1);
  buttons_layout->addWidget(m_x_button, 1, 2);

  buttons_layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding), 0, 3);

  auto* dpad_layout = new QGridLayout;
  dpad_layout->addWidget(m_left_button, 0, 0);
  dpad_layout->addWidget(m_up_button, 0, 1);
  dpad_layout->addWidget(m_down_button, 1, 0);
  dpad_layout->addWidget(m_right_button, 1, 1);
  dpad_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 3);
  dpad_layout->addItem(new QSpacerItem(0, 1, QSizePolicy::Expanding), 0, 3);

  m_dpad_box = new QGroupBox(tr("D-Pad"));
  m_dpad_box->setLayout(dpad_layout);
  m_buttons_box = new QGroupBox(tr("Buttons"));
  m_buttons_box->setLayout(buttons_layout);

  auto* layout = new QVBoxLayout;
  layout->addWidget(m_dpad_box);
  layout->addWidget(m_buttons_box);
  //layout->addWidget(m_settings_box);

  setLayout(layout);
}

void GBATASInputWindow::GetValues(GCPadStatus* pad)
{
  if (!isVisible())
    return;

  GetButton<u16>(m_a_button, pad->button, PAD_BUTTON_A);
  GetButton<u16>(m_b_button, pad->button, PAD_BUTTON_B);
  GetButton<u16>(m_x_button, pad->button, PAD_BUTTON_X);        // Reset
  GetButton<u16>(m_select_button, pad->button, PAD_TRIGGER_Z);  // Select
  GetButton<u16>(m_l_button, pad->button, PAD_TRIGGER_L);
  GetButton<u16>(m_r_button, pad->button, PAD_TRIGGER_R);
  GetButton<u16>(m_left_button, pad->button, PAD_BUTTON_LEFT);
  GetButton<u16>(m_up_button, pad->button, PAD_BUTTON_UP);
  GetButton<u16>(m_down_button, pad->button, PAD_BUTTON_DOWN);
  GetButton<u16>(m_right_button, pad->button, PAD_BUTTON_RIGHT);
  GetButton<u16>(m_start_button, pad->button, PAD_BUTTON_START);

  if (m_a_button->isChecked())
    pad->analogA = 0xFF;
  else
    pad->analogA = 0x00;

  if (m_b_button->isChecked())
    pad->analogB = 0xFF;
  else
    pad->analogB = 0x00;

  // Center sticks so they don't clog up the input display
  pad->stickX = 128;
  pad->stickY = 128;
  pad->substickX = 128;
  pad->substickY = 128;

  // Test
  //if (Movie::GetCurrentFrame() == compFrame)
  //  pad->button = 0x009;
}
