// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/TAS/GBATASInputEditor.h"
#include "DolphinQt/Resources.h"
#include "DolphinQt/QtUtils/NonDefaultQPushButton.h"

#include "Core/HW/SI/GBAEditorInputs.h"

#include "DolphinFileDialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>

GBATASInputEditor::GBATASInputEditor(QWidget* widget) : QWidget(widget)
{
  setWindowIcon(Resources::GetAppIcon());
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  
  CreateEditorLayout();
  ConnectWidgets();
}

// re-implementing non-table related hotkeys so they are available for the entire
// Editor window and not just when the tableview itself has focus
// couldn't figure out how to send inputs between the widgets
void GBATASInputEditor::keyPressEvent(QKeyEvent* event)
{
  if (!GBATASInputEditorView::editor_hotkeys_disabled)
  {
    // after some events like toggling inputs the table still has focus
    // so if that's the case we skip these to avoid duplicate inputs
    if (!gbaTASInputEditorView->hasFocus())
    { 
      gbaTASInputEditorView->GBATASInputEditorView::FrameAdvance(event);
      gbaTASInputEditorView->GBATASInputEditorView::ToggleEmulationPaused(event);
      gbaTASInputEditorView->GBATASInputEditorView::HandleSaveStates(event);
      gbaTASInputEditorView->GBATASInputEditorView::ChangeStateLayerHotkeys(event);
    }
  }
}

void GBATASInputEditor::ImportInputFile()
{
  // select input file in GUI
  QString input_file_name = DolphinFileDialog::getOpenFileName(
      this, tr("Select the Input File to Load"), QString(), tr("GBA Editor Inputs (*.gei)"));

  if (input_file_name.isEmpty())
    return;

  m_import_export_file_name = input_file_name;
  // open selected input file
  QFile input_file(input_file_name);
  if (!input_file.open(QIODevice::ReadOnly))
  {
    qCritical() << "Could not open file!";
    qCritical() << input_file.errorString();
    return;
  }

  QTextStream input_stream(&input_file);

  // clears View & Deque
  gbaTASInputEditorView->ExecClearAllRows();

  // parse file until the end
  while (!input_stream.atEnd())
  {
    // one line of inputs, 20 characters long
    // Left, Up, Down, Right, A, B, L, R, Start, Z => Select; for P1, then P2
    QString line = input_stream.readLine();

    std::vector<bool> input_frame_vector = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 20; i++)
    {
      if (line.at(i) == tr("1"))
        input_frame_vector[i] = 1;
    }
    
    // write one line of inputs (= one frame) to deque
    Schneeheide::GBAEditorInputs::frameDeque.push_back(input_frame_vector);
  }
  input_file.close();

  // insert new rows to model
  gbaTASInputEditorView->ExecInsertImportRows();
}

void GBATASInputEditor::ExportInputFile()
{
  QString export_file_name = DolphinFileDialog::getSaveFileName(
      this, tr("Save Inputs As"), m_import_export_file_name, tr("GBA Editor Inputs (*.gei)"));
  if (export_file_name.isEmpty())
    return;

  // export input file
  QFile export_file(export_file_name);
  if (!export_file.open(QIODevice::WriteOnly))
  {
    qCritical() << "Could not export file!";
    return;
  }

  QTextStream export_stream(&export_file);

  u64 input_frames = Schneeheide::GBAEditorInputs::frameDeque.size();

  for (u64 i = 0; i < input_frames - 1; i++)
  {
    std::vector input_line = Schneeheide::GBAEditorInputs::frameDeque.at(i);

    for (int j = 0; j < 20; j++)
      export_stream << input_line.at(j);
    export_stream << "\n";
  }
  // finish last input without new line
  std::vector input_line = Schneeheide::GBAEditorInputs::frameDeque.back();
  for (int j = 0; j < 20; j++)
    export_stream << input_line.at(j);

  export_file.close();

  // store filename for quick saving next time
  m_import_export_file_name = export_file_name;
}

void GBATASInputEditor::ConnectWidgets()
{
  connect(m_load_file_button, &QPushButton::clicked, this, &GBATASInputEditor::ImportInputFile);
  connect(m_export_file_button, &QPushButton::clicked, this, &GBATASInputEditor::ExportInputFile);
  connect(m_append_frames_button, &QPushButton::clicked, gbaTASInputEditorView, &GBATASInputEditorView::ExecAppendRows);
  connect(m_insert_frames_button, &QPushButton::clicked, gbaTASInputEditorView, &GBATASInputEditorView::InsertRowsAt);
  connect(m_remove_frames_button, &QPushButton::clicked, gbaTASInputEditorView, &GBATASInputEditorView::RemoveRowsAt);
  connect(m_jump_to_frame_button, &QPushButton::clicked, gbaTASInputEditorView, &GBATASInputEditorView::JumpToFrameColumn);

  connect(m_disable_auto_scrolling_checkbox, &QCheckBox::stateChanged, gbaTASInputEditorView,
          &GBATASInputEditorView::ToggleAutoScrolling);
  connect(m_process_inputs_checkbox, &QCheckBox::stateChanged, this,
          &Schneeheide::GBAEditorInputs::ToggleInputProcessing);
  connect(m_hide_start_columns_checkbox, &QCheckBox::stateChanged, gbaTASInputEditorView,
          &GBATASInputEditorView::ToggleShowStartColumns);
  connect(m_disable_editor_hotkeys_checkbox, &QCheckBox::stateChanged, gbaTASInputEditorView,
          &GBATASInputEditorView::ToggleDisableEditorHotkeys);
  connect(m_hide_rows_checkbox, &QCheckBox::stateChanged, this,
          &GBATASInputEditor::ToggleHideFrameColumns);
  connect(m_hide_frames_start, &QSpinBox::valueChanged, this,
          &GBATASInputEditor::CheckSpinBoxValues);
  connect(m_hide_frames_end, &QSpinBox::valueChanged, this,
          &GBATASInputEditor::CheckSpinBoxValues);
  connect(m_save_to_slot_button, &QPushButton::clicked, gbaTASInputEditorView,
          &GBATASInputEditorView::SaveStateToSlot);
  connect(m_load_from_slot_button, &QPushButton::clicked, gbaTASInputEditorView,
          &GBATASInputEditorView::LoadStateFromSlot);
  connect(gbaTASInputEditorView->state_layer_spinbox, &QSpinBox::valueChanged,
          gbaTASInputEditorView, &GBATASInputEditorView::UpdateSavestateLayer);
}

void GBATASInputEditor::CheckSpinBoxValues()
{
  int start_value = m_hide_frames_start->value();
  int end_value = m_hide_frames_end->value();

  if (start_value > end_value)
    m_hide_frames_start->setValue(end_value);
}

void GBATASInputEditor::ToggleHideFrameColumns()
{
  int start_value = m_hide_frames_start->value();
  int end_value = m_hide_frames_end->value();

  if (!m_frames_hidden)
  {
    m_hide_frames_start->setReadOnly(true);
    m_hide_frames_end->setReadOnly(true);
    gbaTASInputEditorView->GBATASInputEditorView::HideFrameColumns(start_value, end_value);
    m_frames_hidden = true;
  }
  else
  {
    m_hide_frames_start->setReadOnly(false);
    m_hide_frames_end->setReadOnly(false);
    gbaTASInputEditorView->GBATASInputEditorView::ShowFrameColumns(start_value, end_value);
    m_frames_hidden = false;
  }
}

void GBATASInputEditor::CreateEditorLayout()
{
  setWindowTitle(tr("GBA TAS Input Editor"));
  gbaTASInputEditorView = new GBATASInputEditorView;

  auto* layout = new QGridLayout;

  QGroupBox* table_box = new QGroupBox;
  QVBoxLayout* table_layout = new QVBoxLayout;

  QGroupBox* side_box = new QGroupBox;
  QVBoxLayout* side_layout = new QVBoxLayout;

  QGroupBox* io_buttons_box = new QGroupBox(tr("File Management"));
  QVBoxLayout* io_buttons_layout = new QVBoxLayout;

  QGroupBox* table_buttons_box = new QGroupBox(tr("Table Modifiers"));
  QVBoxLayout* table_buttons_layout = new QVBoxLayout;

  QGroupBox* spinbox_box = new QGroupBox(tr("Hide range of inputs"));
  QGridLayout* spinbox_layout = new QGridLayout;

  QGroupBox* settings_box = new QGroupBox(tr("Settings"));
  QVBoxLayout* settings_layout = new QVBoxLayout;

  QGroupBox* states_box = new QGroupBox(tr("Savestate Management"));
  QGridLayout* states_layout = new QGridLayout;

  QGroupBox* empty_box = new QGroupBox();

  m_load_file_button = new NonDefaultQPushButton(tr("Import Inputs..."));
  m_export_file_button = new NonDefaultQPushButton(tr("Export Inputs..."));
  m_append_frames_button = new NonDefaultQPushButton(tr("Append Frame(s)"));
  m_append_frames_button->setToolTip(tr("Insert empty frames at the bottom of the table."));
  m_insert_frames_button = new NonDefaultQPushButton(tr("Insert Frame(s) at..."));
  m_remove_frames_button = new NonDefaultQPushButton(tr("Remove Frame(s) at..."));
  m_jump_to_frame_button = new NonDefaultQPushButton(tr("Jump to Frame..."));

  m_disable_auto_scrolling_checkbox = new QCheckBox(tr("Disable Auto-Scrolling"));
  m_process_inputs_checkbox = new QCheckBox(tr("Disable Editor-Inputs"));
  m_process_inputs_checkbox->setToolTip(tr("Inputs from other sources are overwritten by the editor"
  " until it runs out of input rows. Check this if you need to take back control."));

  m_hide_start_columns_checkbox = new QCheckBox(tr("Hide Start-Columns"));
  m_hide_start_columns_checkbox->setChecked(true);
  m_disable_editor_hotkeys_checkbox = new QCheckBox(tr("Disable Editor-Hotkeys"));
  m_hide_rows_checkbox = new QCheckBox(tr("Hide Input-Rows"));
  m_hide_rows_checkbox->setToolTip(tr("Does not hide last Input-Row."));

  QLabel* hide_frames_label_one = new QLabel(tr("First Row:"));
  m_hide_frames_start = new QSpinBox();
  m_hide_frames_start->setMinimum(0);
  m_hide_frames_start->setMaximum(1000000);
  QLabel* hide_frames_label_two = new QLabel(tr("Last Row:"));
  m_hide_frames_end = new QSpinBox();
  m_hide_frames_end->setMinimum(0);
  m_hide_frames_end->setMaximum(1000000);

  m_save_to_slot_button = new NonDefaultQPushButton(tr("Save State to Slot.."));
  m_load_from_slot_button = new NonDefaultQPushButton(tr("Load State from Slot.."));

  table_layout->addWidget(gbaTASInputEditorView);
  table_box->setLayout(table_layout);

  io_buttons_layout->addWidget(m_load_file_button);
  io_buttons_layout->addWidget(m_export_file_button);
  io_buttons_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  io_buttons_box->setLayout(io_buttons_layout);
  side_layout->addWidget(io_buttons_box);

  table_buttons_layout->addWidget(m_append_frames_button);
  table_buttons_layout->addWidget(m_insert_frames_button);
  table_buttons_layout->addWidget(m_remove_frames_button);
  table_buttons_layout->addWidget(m_jump_to_frame_button);
  table_buttons_layout->addWidget(m_hide_start_columns_checkbox);

  spinbox_layout->addWidget(hide_frames_label_one, 0, 0);
  spinbox_layout->addWidget(m_hide_frames_start, 0, 1);
  spinbox_layout->addWidget(hide_frames_label_two, 1, 0);
  spinbox_layout->addWidget(m_hide_frames_end, 1, 1);
  spinbox_layout->addWidget(m_hide_rows_checkbox, 2, 0, 1, 0);

  table_buttons_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  table_buttons_box->setLayout(table_buttons_layout);
  spinbox_box->setLayout(spinbox_layout);
  spinbox_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  side_layout->addWidget(table_buttons_box);
  side_layout->addWidget(spinbox_box);

  settings_layout->addWidget(m_disable_auto_scrolling_checkbox);
  settings_layout->addWidget(m_process_inputs_checkbox);
  settings_layout->addWidget(m_disable_editor_hotkeys_checkbox);
  settings_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  settings_box->setLayout(settings_layout);
  side_layout->addWidget(settings_box);

  states_layout->addWidget(m_load_from_slot_button, 0, 0, 1, 0);
  states_layout->addWidget(m_save_to_slot_button, 1, 0, 1, 0);
  QLabel* state_layer_label = new QLabel(tr("Savestate Layer"));
  state_layer_label->setToolTip(
      tr("Shifts Slot Layers. 0 =>"
         " F1 to F9 correspond to Slots 1 to 9. Layer 1 => F1 to F9 correspond"
         " to slots 11 to 19, and so on until 91 to 99 for Layer 9."
         " F11 decrements / F12 increments layer"));

  states_layout->addWidget(state_layer_label, 2, 0);
  states_layout->addWidget(gbaTASInputEditorView->state_layer_spinbox, 2, 1);
  states_box->setLayout(states_layout);
  states_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  side_layout->addWidget(states_box);
  side_layout->addWidget(empty_box);

  side_layout->setSpacing(0.5);
  side_box->setMaximumWidth(186);
  side_box->setLayout(side_layout);

  layout->addWidget(table_box, 0, 0);
  layout->addWidget(side_box, 0, 1);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  setLayout(layout);
}
