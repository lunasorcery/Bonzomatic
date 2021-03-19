#pragma once

namespace Capture
{
  void LoadSettings(jsonxx::Object & o);
  bool Open(RendererSettings & settings);
  void CaptureFrame();
  void Close();
}