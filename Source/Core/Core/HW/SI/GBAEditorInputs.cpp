// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Core/Movie.h"
#include "Core/HW/SI/GBAEditorInputs.h"

namespace Schneeheide
{

std::deque<std::vector<bool>> GBAEditorInputs::frameDeque;
bool GBAEditorInputs::process_inputs = true;

std::vector<u16> input_template = {

    0x0001,  // Left
    0x0008,  // Up
    0x0004,  // Down
    0x0002,  // Right
    0x0100,  // A
    0x0200,  // B
    0x0040,  // L
    0x0020,  // R
    0x0010,  // Z => Select
    0x1000,  // Start
};

GBAEditorInputs::GBAEditorInputs()
{
}

// Grab input from deque on current frame and set pads accordingly
// We'll call this function in SI_DeviceGBAEmu.cpp
void GBAEditorInputs::ApplyGBAEditorInputs(int device_number, GCPadStatus* pad_status)
{
  // because we overwrite any input from devices or TASinput, if we want to
  // take control back for manual inputs, we can stop right here
  // and just not overwrite them
  if (!process_inputs)
    return;

  // due to the -1 offset below, we skip frame 0
  if (Movie::GetCurrentFrame() == 0)
    return;

  // -1 for offset reasons to align table with framecount
  u64 currentFrame = Movie::GetCurrentFrame() - 1;

  // prevent out of bounds
  if (currentFrame >= frameDeque.size())
    return;

  // grab input bits for current frame (contains both players)
  std::vector<bool> inputFrame = frameDeque.at(currentFrame);

  // bits 0-9 are P1, 10-19 are P2
  int iterator = 0;

  if (device_number == 1) // P2, don't need a check for P1. (goodbye P3 & 4)
    iterator = 10;

  u16 input = 0x0000;

  for (int i = 0; i < 10; i++)
  {
    input = input + (inputFrame[iterator] * input_template[i]);
    iterator++;
  }
  // inputs don't appear on the inputdisplay
  pad_status->button = input;
}

void GBAEditorInputs::ToggleInputProcessing()
{
  process_inputs = !process_inputs;
}

}  // namespace Schneeheide
