#include "wave-mobility-model.h"
#include "ns3/simulator.h"

#include "ns3/double.h"
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WaveMobilityModel);

TypeId WaveMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WaveMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<WaveMobilityModel> ()
    .AddAttribute("wspeed",
                          "Wind speed",
                          DoubleValue(5.0),
                          MakeDoubleAccessor(&WaveMobilityModel::m_wspeed),
                          MakeDoubleChecker<double>())
    .AddAttribute("minf",
                          "min frequency",
                          DoubleValue(1.0),
                          MakeDoubleAccessor(&WaveMobilityModel::m_minf),
                          MakeDoubleChecker<double>())
    .AddAttribute("maxf",
                          "max  frequency",
                          DoubleValue(5.0),
                          MakeDoubleAccessor(&WaveMobilityModel::m_maxf),
                          MakeDoubleChecker<double>())
    .AddAttribute("fnum",
                          "frequency Division number",
                          DoubleValue(20.0),
                          MakeDoubleAccessor(&WaveMobilityModel::m_fnum),
                          MakeDoubleChecker<double>())
    .AddAttribute("mina",
                          "min direction angle ",
                          DoubleValue(0.0),
                          MakeDoubleAccessor(&WaveMobilityModel::m_mina),
                          MakeDoubleChecker<double>())
    .AddAttribute("maxa",
                          "max direction angle ",
                          DoubleValue(3.1415926),
                          MakeDoubleAccessor(&WaveMobilityModel::m_maxa),
                          MakeDoubleChecker<double>())
    .AddAttribute("anum",
                          "direction angle Division number",
                          DoubleValue(10.0),
                          MakeDoubleAccessor(&WaveMobilityModel::m_anum),
                          MakeDoubleChecker<double>());
  return tid;
}

WaveMobilityModel::WaveMobilityModel ()
{
}

WaveMobilityModel::~WaveMobilityModel ()
{
}

void
WaveMobilityModel::SetVelocity (const Vector &speed)
{
  m_helper.Setwave( m_wspeed,m_minf,m_maxf,m_fnum,m_mina,m_maxa,m_anum ) ; 
  m_helper.Update ();
  m_helper.SetVelocity (speed);
  m_helper.Unpause ();
  NotifyCourseChange ();
}


Vector
WaveMobilityModel::DoGetPosition (void) const
{

  m_helper.Setwave( m_wspeed,m_minf,m_maxf,m_fnum,m_mina,m_maxa,m_anum); 
  m_helper.Update ();
  return m_helper.GetCurrentPosition ();
}
void 
WaveMobilityModel::DoSetPosition (const Vector &position)
{
  m_helper.SetPosition (position);
  NotifyCourseChange ();
}
Vector
WaveMobilityModel::DoGetVelocity (void) const
{
  return m_helper.GetVelocity ();
}

} // namespace ns3
