#ifndef WAVE_MOBILITY_MODEL_H
#define WAVE_MOBILITY_MODEL_H

#include <stdint.h>
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"
#include "wave-helper.h"

namespace ns3 {

class WaveMobilityModel : public MobilityModel 
{
public:
  
  static TypeId GetTypeId (void);
  
  WaveMobilityModel ();

  virtual ~WaveMobilityModel ();

  void SetVelocity (const Vector &speed);

private:
  mutable double m_wspeed;//Wind speed
  mutable double m_minf;// min frequency
  mutable double m_maxf;//max  frequency
  mutable double m_fnum;//frequency Division number
  mutable double m_mina;//min direction angle 
  mutable double m_maxa;//max direction angle 
  mutable double m_anum;//direction angle Division number

  virtual Vector DoGetPosition (void) const;
  virtual void DoSetPosition (const Vector &position);//
  virtual Vector DoGetVelocity (void) const;//
  WaveHelper m_helper;  
};

} // namespace ns3

#endif 
