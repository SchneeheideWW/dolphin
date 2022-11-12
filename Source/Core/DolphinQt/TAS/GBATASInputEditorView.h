// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QKeyEvent>
#include <QLabel>
#include <QSpinBox>
#include <QTableView>

class QPushButton;
class GBATASInputEditorModel;

class GBATASInputEditorView : public QTableView
{
  Q_OBJECT

public:
  GBATASInputEditorView(QWidget* parent = Q_NULLPTR);

  GBATASInputEditorModel *gbaTASInputEditorModel;

  static int savestate_layer_value;
  static bool m_mem_watch_is_hidden;
  QSpinBox* state_layer_spinbox;

  void UpdateSavestateLayer();
  void Update();
  static void ToggleAutoScrolling();
  void ToggleShowStartColumns();
  void HideFrameColumns(int row_first, int row_last);
  void ShowFrameColumns(int row_first, int row_last);
  void ToggleDisableEditorHotkeys();
  void JumpToFrameColumn();
  //void ScrollToCurrentFrame(u64 currFrame);
  void ExecAppendRows();
  void ExecClearAllRows();
  void ExecInsertImportRows();
  void InsertRowsAt();
  void RemoveRowsAt();
  int GetRowCount();
  QString RollMacro();
  QString FMCMacro();
  void PasteInputs(QString pasted_text);
  void FrameAdvance(QKeyEvent* event);
  void ToggleEmulationPaused(QKeyEvent* event);
  void HandleSaveStates(QKeyEvent* event);
  void ChangeStateLayerHotkeys(QKeyEvent* event);
  void SaveStateToSlot();
  void LoadStateFromSlot();

  static bool auto_scrolling_enabled;
  bool hide_start_columns = true;
  static bool editor_hotkeys_disabled;

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

private:
  int mouseSection;
  int mouseRow;
  int mouseMinRow;
  int mouseMaxRow;
  bool mouseValue;

  int m_row_first;
  int m_row_last;
  bool m_frames_not_hidden = true;
};
