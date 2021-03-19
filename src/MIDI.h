#pragma once

namespace MIDI
{
  enum class MIDIMessageType
  {
    NoteOff         = 8,
    NoteOn          = 9,
    PolyphonicKey   = 10,
    ControlChange   = 11,
    ProgramChange   = 12,
    ChannelPressure = 13,
    PitchBend       = 14,
    System          = 15,
  };

  bool Open();
  bool Close();
  float GetCCValue( unsigned char cc );
};