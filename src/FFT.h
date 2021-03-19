#pragma once

#define FFT_SIZE 1024

struct FFTSettings
{
  bool bUseRecordingDevice;
  void * pDeviceID;
};

namespace FFT
{
  typedef void (*FFTEnumerateFunc)( const bool bIsCaptureDevice, const char * szDeviceName, void * pDeviceID, void * pUserContext );

  extern float fAmplification;

  void EnumerateDevices( FFTEnumerateFunc pEnumerationFunction, void * pUserContext );

  bool Create();
  bool Destroy();
  bool Open( FFTSettings * pSettings );
  bool GetFFT( float * _samples );
  void Close();
}