#ifndef __IPLUGOPENGL__
#define __IPLUGOPENGL__

#include "IPlug_include_in_plug_hdr.h"

class IPlugOpenGL : public IPlug
{
public:

  IPlugOpenGL(IPlugInstanceInfo instanceInfo);
  ~IPlugOpenGL();

  void Reset();
  void OnParamChange(int paramIdx);

  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
};

#endif
