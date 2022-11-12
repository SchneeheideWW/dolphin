// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/TAS/GBATASInputEditorView.h"

#include "Core/HW/GBAPadEmu.h"

#include <vector>

#include <QDialog>
#include <QLabel>
#include <QSpinBox>

class QGroupBox;
class QCheckBox;
class QPushButton;
class QSpinBox;
class QLabel;

class GBATASInputEditorView;

class GBATASInputEditor : public QWidget
{
  Q_OBJECT
public:
  explicit GBATASInputEditor(QWidget* parent = nullptr);

  GBATASInputEditorView* gbaTASInputEditorView;

private:
  void ConnectWidgets();
  void ImportInputFile();
  void ExportInputFile();
  void CreateEditorLayout();
  void ToggleHideFrameColumns();
  void CheckSpinBoxValues();
  void keyPressEvent(QKeyEvent* event);

  QGroupBox* mem_watch_box;

  QString m_import_export_file_name = tr("");

  QCheckBox* m_disable_auto_scrolling_checkbox;
  QCheckBox* m_process_inputs_checkbox;
  QCheckBox* m_hide_start_columns_checkbox;
  QCheckBox* m_disable_editor_hotkeys_checkbox;
  QCheckBox* m_hide_rows_checkbox;

  QPushButton* m_load_file_button;
  QPushButton* m_export_file_button;
  QPushButton* m_append_frames_button;
  QPushButton* m_insert_frames_button;
  QPushButton* m_remove_frames_button;
  QPushButton* m_jump_to_frame_button;
  QPushButton* m_save_to_slot_button;
  QPushButton* m_load_from_slot_button;

  QSpinBox* m_hide_frames_start;
  QSpinBox* m_hide_frames_end;

  bool m_frames_hidden = false;
};
