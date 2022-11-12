// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Host.h"
#include "DolphinQt/TAS/GBATASInputEditorModel.h"
#include "DolphinQt/TAS/GBATASInputEditorView.h"

#include "Core/Core.h"
#include "Core/HotkeyManager.h"
#include "Core/Movie.h"
#include "Core/State.h"
#include "Core/HW/SI/GBAEditorInputs.h"

#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QInputDialog>
#include <QScrollBar>
#include <QKeyEvent>

//#include <Core/State.cpp>

// using namespace State so it would pick the correct Load()&Save() and not qitemview's...
using namespace State;

bool GBATASInputEditorView::auto_scrolling_enabled = true;
bool GBATASInputEditorView::editor_hotkeys_disabled = false;
int GBATASInputEditorView::savestate_layer_value = 0;
bool GBATASInputEditorView::m_mem_watch_is_hidden = false;

GBATASInputEditorView::GBATASInputEditorView(QWidget* parent)
    : QTableView(parent)
{
  verticalHeader()->setVisible(false);
  verticalHeader()->setDefaultSectionSize(verticalHeader()->minimumSectionSize());

  horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  horizontalHeader()->setHighlightSections(false);

  setSelectionBehavior(QAbstractItemView::SelectItems);
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setShowGrid(true);
  setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

  gbaTASInputEditorModel = new GBATASInputEditorModel();
  setModel(gbaTASInputEditorModel);

  // hiding Start columns by default because we rarely (if at all) need them
  setColumnHidden(10, true);
  setColumnHidden(20, true);

  // creating the spinbox here in the view for access reasons
  // is what past me wrote; I am sure there was a reason for it - I forgot
  state_layer_spinbox = new QSpinBox();
  state_layer_spinbox->setMinimum(0);
  state_layer_spinbox->setMaximum(9);
  state_layer_spinbox->setToolTip(
      tr("Shifts Slot Layers. 0 =>"
                  " F1 to F9 correspond to Slots 1 to 9. Layer 1 => F1 to F9 correspond"
                  " to slots 11 to 19, and so on until 91 to 99 for Layer 9."
                  " F11 decrements / F12 increments layer"));

  // this makes it so that when loading a save or frame advancing
  // it calls GBATASInputEditorView::Update()
  connect(Host::GetInstance(), &Host::UpdateDisasmDialog, this, &GBATASInputEditorView::Update);
}

void GBATASInputEditorView::UpdateSavestateLayer()
{
  GBATASInputEditorView::savestate_layer_value = (state_layer_spinbox->value()) * 10;
}

void GBATASInputEditorView::Update()
{
  u64 currentFrame = Movie::GetCurrentFrame();

  // don't really need any checks since data() checks for valid index anyway
  if (currentFrame >= gbaTASInputEditorModel->rowCount(QModelIndex()))
    return;

  // tell view to update so current frame row can be highlighted
  QModelIndex index_l = gbaTASInputEditorModel->index(currentFrame, 0);
  QModelIndex index_r = gbaTASInputEditorModel->index(currentFrame, 21);
  emit dataChanged(index_l, index_r, {Qt::BackgroundRole});

  // scroll to current frame but leave some space above to better track inputs
  if(auto_scrolling_enabled) 
    scrollTo(gbaTASInputEditorModel->index(currentFrame - 10, 0), QAbstractItemView::PositionAtTop);
}

void GBATASInputEditorView::ToggleAutoScrolling()
{
  auto_scrolling_enabled = !auto_scrolling_enabled;
}

void GBATASInputEditorView::ToggleShowStartColumns()
{
  hide_start_columns = !hide_start_columns;

  setColumnHidden(10, hide_start_columns);
  setColumnHidden(20, hide_start_columns);
}

void GBATASInputEditorView::HideFrameColumns(int row_first, int row_last)
{
  int row_count = gbaTASInputEditorModel->rowCount(QModelIndex()) - 1;

  for (row_first; (row_first <= row_last) && (row_first < row_count); row_first++)
    setRowHidden(row_first, true);
}

void GBATASInputEditorView::ShowFrameColumns(int row_first, int row_last)
{
  int row_count = gbaTASInputEditorModel->rowCount(QModelIndex()) - 1;

  for (row_first; (row_first <= row_last) && (row_first < row_count); row_first++)
    setRowHidden(row_first, false);
}

void GBATASInputEditorView::ToggleDisableEditorHotkeys()
{
  editor_hotkeys_disabled = !editor_hotkeys_disabled;
}

void GBATASInputEditorView::JumpToFrameColumn()
{
  bool ok;
  unsigned int row = QInputDialog::getInt(this, tr("Jump to Frame..."), tr("Jump to Frame..."), 0, 0,
                                 1000000, 1, &ok);
  scrollTo(gbaTASInputEditorModel->index(row, 0), QAbstractItemView::PositionAtTop);
}

void GBATASInputEditorView::keyPressEvent(QKeyEvent* event)
{
  if (!editor_hotkeys_disabled)
  {
    if (event->key() == Qt::Key_R)
    {
      // insert a Roll input followed by a perfect Roll
      if (selectedIndexes().length() == 1)
      {
        if ((selectedIndexes().first().column() == 8) || (selectedIndexes().first().column() == 18))
        {
          //ModifyClipboardRoll();
          PasteInputs(RollMacro());
        }
      }

      // FMC Roll Macro (you can cheese the length check
      // by selecting 3 single cells with control but if that's what you want...)
      else if (selectedIndexes().length() == 3)
      {
        if ((selectedIndexes().first().column() == 6) || (selectedIndexes().first().column() == 16))
        {
          //ModifyClipboardFMC();
          PasteInputs(FMCMacro());
        }
      }
    }

    // Frame Advance
    FrameAdvance(event);

    // Pause / Unpause Emulation
    ToggleEmulationPaused(event);

    // Saving & Loading States from F1 to F9
    HandleSaveStates(event);

    // F11 & F12 to quickly change savestate layer
    ChangeStateLayerHotkeys(event);
  }

  if(!selectedIndexes().isEmpty())
  {
    // Insert / Delete one row
    if (selectedIndexes().first().column() == 0)
    {
      QItemSelectionRange range = selectionModel()->selection().first();
      // insert one row above selected row(s)
      if (event->key() == Qt::Key_Plus)
      {
        gbaTASInputEditorModel->insertOneRow(range.top(), QModelIndex());
      }

      // delete selected row or the one at the top of selected rows
      if (event->key() == Qt::Key_Minus)
      {
        gbaTASInputEditorModel->removeOneRow(range.top(), QModelIndex());
      }
    }

    // Delete inputs (not frame rows) when pressing the Delete Key
    if (event->key() == Qt::Key_Delete)
    {
      QItemSelectionRange del_range = selectionModel()->selection().first();
      for (auto i = del_range.top(); i <= del_range.bottom(); ++i)
      {
        for (auto j = del_range.left(); j <= del_range.right(); ++j)
          gbaTASInputEditorModel->setData(model()->index(i, j), QVariant(0), Qt::EditRole);
      }
    }

    // Copy & Paste Functionality
    // the format would work with excel tables and such I think
    else if (event->matches(QKeySequence::Copy))
    {
      QString text;
      QItemSelectionRange copy_range = selectionModel()->selection().first();
      for (auto i = copy_range.top(); i <= copy_range.bottom(); ++i)
      {
        QStringList rowContents;
        for (auto j = copy_range.left(); j <= copy_range.right(); ++j)
          rowContents << gbaTASInputEditorModel->index(i, j).data().toString();
        text += rowContents.join(tr("\t"));
        text += tr("\n");
      }
      QApplication::clipboard()->setText(text);
    }

    else if (event->matches(QKeySequence::Paste))
    {     
      PasteInputs(QApplication::clipboard()->text());
    }

    else if (event->matches(QKeySequence::Cut))
    {
      QString cut_text;
      QItemSelectionRange cut_range = selectionModel()->selection().first();
      for (auto i = cut_range.top(); i <= cut_range.bottom(); ++i)
      {
        QStringList rowContents;
        for (auto j = cut_range.left(); j <= cut_range.right(); ++j)
          rowContents << gbaTASInputEditorModel->index(i, j).data().toString();
        cut_text += rowContents.join(tr("\t"));
        cut_text += tr("\n");
      }
      QApplication::clipboard()->setText(cut_text);

      // copied values to clipboard, now set everything to 0
      for (auto i = cut_range.top(); i <= cut_range.bottom(); ++i)
      {
        for (auto j = cut_range.left(); j <= cut_range.right(); ++j)
          gbaTASInputEditorModel->setData(model()->index(i, j), QVariant(0), Qt::EditRole);
      }
    }
  }
  else
    QTableView::keyPressEvent(event);
}

void GBATASInputEditorView::FrameAdvance(QKeyEvent* event)
{
  if (event->key() == Qt::Key_F)
    Core::DoFrameStep();
}

void GBATASInputEditorView::ToggleEmulationPaused(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Pause)
  {
    if (Core::GetState() == Core::State::Paused)
      Core::SetState(Core::State::Running);
    else
      Core::SetState(Core::State::Paused);
  }
}

// Decrement / Increment Savestate layer
void GBATASInputEditorView::ChangeStateLayerHotkeys(QKeyEvent* event)
{
  if (event->key() == Qt::Key_F11 && state_layer_spinbox->value() >= 1)
    state_layer_spinbox->setValue(state_layer_spinbox->value() - 1);

  else if (event->key() == Qt::Key_F12 && state_layer_spinbox->value() < 9)
    state_layer_spinbox->setValue(state_layer_spinbox->value() + 1);
}

void GBATASInputEditorView::HandleSaveStates(QKeyEvent* event)
{
  if (event->key() == Qt::Key_F1)
  {
    if (event->modifiers() & Qt::ShiftModifier)
      Save(1 + GBATASInputEditorView::savestate_layer_value);
    else
      Load(1 + GBATASInputEditorView::savestate_layer_value);
  }

  else if (event->key() == Qt::Key_F2)
  {
    if (event->modifiers() & Qt::ShiftModifier)
      Save(2 + GBATASInputEditorView::savestate_layer_value);
    else
      Load(2 + GBATASInputEditorView::savestate_layer_value);
  }

  else if(event->key() == Qt::Key_F3)
  {
    if (event->modifiers() & Qt::ShiftModifier)
      Save(3 + GBATASInputEditorView::savestate_layer_value);
    else
      Load(3 + GBATASInputEditorView::savestate_layer_value);
  }

  else if (event->key() == Qt::Key_F4)
  {
    if (event->modifiers() & Qt::ShiftModifier)
      Save(4 + GBATASInputEditorView::savestate_layer_value);
    else
      Load(4 + GBATASInputEditorView::savestate_layer_value);
  }

  else if (event->key() == Qt::Key_F5)
  {
    if (event->modifiers() & Qt::ShiftModifier)
      Save(5 + GBATASInputEditorView::savestate_layer_value);
    else
      Load(5 + GBATASInputEditorView::savestate_layer_value);
  }

  else if (event->key() == Qt::Key_F6)
  {
    if (event->modifiers() & Qt::ShiftModifier)
      Save(6 + GBATASInputEditorView::savestate_layer_value);
    else
      Load(6 + GBATASInputEditorView::savestate_layer_value);
  }

  else if (event->key() == Qt::Key_F7)
  {
    if (event->modifiers() & Qt::ShiftModifier)
      Save(7 + GBATASInputEditorView::savestate_layer_value);
    else
      Load(7 + GBATASInputEditorView::savestate_layer_value);
  }

  else if (event->key() == Qt::Key_F8)
  {
    if (event->modifiers() & Qt::ShiftModifier)
      Save(8 + GBATASInputEditorView::savestate_layer_value);
    else
      Load(8 + GBATASInputEditorView::savestate_layer_value);
  }

  else if (event->key() == Qt::Key_F9)
  {
    if (event->modifiers() & Qt::ShiftModifier)
      Save(9 + GBATASInputEditorView::savestate_layer_value);
    else
      Load(9 + GBATASInputEditorView::savestate_layer_value);
  }
}

QString GBATASInputEditorView::RollMacro()
{
  return tr("R\n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \nR");
}

QString GBATASInputEditorView::FMCMacro()
{
  return tr(" \t \tR\n"
            " \t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \tL\t \n"
            "B\t \t \n"
            " \t \t \n"
            " \t \t \n"
            " \t \tR\n");
}

void GBATASInputEditorView::PasteInputs(QString pasted_text)
{
  //QString text = QApplication::clipboard()->text();
  QString text = pasted_text;
  QStringList rowContents = text.split(tr("\n"), Qt::SkipEmptyParts);

  QModelIndex initIndex = selectedIndexes().at(0);
  auto initRow = initIndex.row();
  auto initCol = initIndex.column();

  for (auto i = 0; i < rowContents.size(); ++i)
  {
    QStringList columnContents = rowContents.at(i).split(tr("\t"));
    for (auto j = 0; j < columnContents.size(); ++j)
    {
      if (columnContents.at(j) == tr(" "))
        gbaTASInputEditorModel->setData(model()->index(initRow + i, initCol + j), QVariant(0),
                                        Qt::EditRole);
      else
        gbaTASInputEditorModel->setData(model()->index(initRow + i, initCol + j), QVariant(1),
                                        Qt::EditRole);
    }
  }
}

void GBATASInputEditorView::mousePressEvent(QMouseEvent* event)
{
  mouseSection = -1;

  if (event->button() != Qt::RightButton)
  {
    return QTableView::mousePressEvent(event);
  }

  // Get the table cell under the mouse position 
  const QModelIndex index = indexAt(event->pos());
  if (!index.isValid())
  {
    return QTableView::mousePressEvent(event);
  }

  // Frame Column shall not be edited
  if (index.column() == 0)
    return QTableView::mousePressEvent(event);


  selectionModel()->clear();
  mouseSection = index.column();
  mouseRow = index.row();
  mouseMinRow = mouseRow;
  mouseMaxRow = mouseRow;

  mouseValue = gbaTASInputEditorModel->toggleInput(index);

  event->accept();
}

void GBATASInputEditorView::mouseMoveEvent(QMouseEvent* event)
{
  // Check if the mouse press event was valid 
  if (mouseSection < 0)
  {
    return QTableView::mouseMoveEvent(event);
  }

  if (!(event->buttons() & Qt::RightButton))
  {
    return QTableView::mouseMoveEvent(event);
  }

  // Get the table cell under the mouse position 
  const QModelIndex index = indexAt(event->pos());
  if (!index.isValid())
  {
    return QTableView::mouseMoveEvent(event);
  }

  event->accept();

  // Check if we need to toggle this input 
  if ((index.row() >= mouseMinRow) && (index.row() <= mouseMaxRow))
    return;

  bool newMouseValue = mouseValue;

  // Toggle all cells from mouse to minRow-1, or maxRow+1 to mouse
  int minLoop, maxLoop;
  if (index.row() < mouseMinRow)
  {
    minLoop = index.row();
    maxLoop = mouseMinRow - 1;
    mouseMinRow = index.row();
  }
  else
  {
    minLoop = mouseMaxRow + 1;
    maxLoop = index.row();
    mouseMaxRow = index.row();
  }

  for (int i = minLoop; i <= maxLoop; i++)
  {
    // drag + control to toggle every 2nd frame
    if (event->modifiers() & Qt::ControlModifier)
    {
      if ((i - mouseRow) % 2)
        newMouseValue = !newMouseValue;
    }
      
    // drag + shift to delete inputs
    //if (event->modifiers() & Qt::ShiftModifier)
    //  newMouseValue = 0;

    // Toggle the cell with the same row as the cell under the mouse 
    QModelIndex toggle_index = gbaTASInputEditorModel->index(i, mouseSection);
    gbaTASInputEditorModel->setData(toggle_index, QVariant(newMouseValue), Qt::EditRole);
  }
}

void GBATASInputEditorView::ExecAppendRows()
{
  bool ok;
  int nbFrames = QInputDialog::getInt(this, tr("Append frames"), tr("Number of frames to append: "),
                                      1, 1, 100000, 1, &ok);
  if (ok)
  {
    gbaTASInputEditorModel->appendRows(nbFrames, QModelIndex());
  }
}

void GBATASInputEditorView::ExecClearAllRows()
{
  u64 count = Schneeheide::GBAEditorInputs::frameDeque.size();
  gbaTASInputEditorModel->clearAllRows(count);
}

void GBATASInputEditorView::ExecInsertImportRows()
{
  u64 count = Schneeheide::GBAEditorInputs::frameDeque.size();
  gbaTASInputEditorModel->insertImportRows(count);
}

void GBATASInputEditorView::InsertRowsAt()
{
  bool ok;
  int row = QInputDialog::getInt(this, tr("Insert frame(s) at..."), tr("Insert at frame: "),
                                 Movie::GetCurrentFrame() - 1, 0,
                                 gbaTASInputEditorModel->rowCount(QModelIndex()) - 1, 1, &ok);
  if (ok)
  {
    bool oki;
    int count = QInputDialog::getInt(this, tr("Insert frame(s) at ") + QString::number(row),
                                     tr("Number of frames to insert: "), 1, 1,
                                     100000, 1, &oki);
    if (oki)
      gbaTASInputEditorModel->insertRows(row, count, QModelIndex());
  }
}

void GBATASInputEditorView::RemoveRowsAt()
{
  bool ok;
  int row = QInputDialog::getInt(this, tr("Remove frame(s) at..."), tr("Remove at frame: "),
                                 Movie::GetCurrentFrame() - 1, 0,
                                 GetRowCount() - 1, 1, &ok);
  if (ok)
  {
    bool oki;
    int count = QInputDialog::getInt(this, tr("Remove frame(s) at ") + QString::number(row),
                                     tr("Number of frames to remove: "), 1, 1, GetRowCount() - row,
                                     1, &oki);
    if (oki)
      gbaTASInputEditorModel->removeRows(row, count, QModelIndex());
  }
}

int GBATASInputEditorView::GetRowCount()
{
  return gbaTASInputEditorModel->rowCount(QModelIndex());
}

void GBATASInputEditorView::SaveStateToSlot()
{
  bool ok;
  int save_slot = QInputDialog::getInt(this, tr("Save State to Slot"), tr("Save-Slot: "), 0, 0,
                                 99,1, &ok);
  if (ok)
  {
    Save(save_slot);
  }
}

void GBATASInputEditorView::LoadStateFromSlot()
{
  bool ok;
  int load_slot =
      QInputDialog::getInt(this, tr("Load State from Slot"), tr("Load-Slot: "), 0, 0, 99, 1, &ok);
  if (ok)
  {
    Load(load_slot);
  }
}
