
#ifndef WAVE_HELPER_H
#define WAVE_HELPER_H

#include "ns3/nstime.h"
#include "ns3/vector.h"

namespace ns3 {

class WaveHelper
{
public:
  WaveHelper ();

  void Setwave(double a,double b,double c,double d,double e,double f,double g) const;

  void SetPosition (const Vector &position);
  
  Vector GetCurrentPosition (void) const;
  
  Vector GetVelocity (void) const;
  
  void SetVelocity (const Vector &vel);
  
  void Pause (void);
  
  void Unpause (void);

  void Update (void) const;
private:
  mutable double m_wspeed;//Wind speed
  mutable double m_minf;// min frequency
  mutable double m_maxf;//max  frequency
  mutable double m_fnum;//frequency Division number
  mutable double m_mina;//min direction angle 
  mutable double m_maxa;//max direction angle 
  mutable double m_anum;//direction angle Division number
  
  mutable Time m_lastUpdate; //!< time of last update
  mutable Vector m_position; //!< state variable for current position
  Vector m_velocity; //!< state variable for velocity
  bool m_paused;  //!< state variable for paused
};

} // namespace ns3

#endif 