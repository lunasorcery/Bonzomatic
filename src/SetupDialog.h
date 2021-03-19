#pragma once

#include "FFT.h"
#include "Renderer.h"

namespace SetupDialog
{
  struct Settings
  {
    RendererSettings sRenderer;
    FFTSettings sFFT;
  };

  bool Open( Settings * pSettings );
}
