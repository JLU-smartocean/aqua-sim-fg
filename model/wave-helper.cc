
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "wave-helper.h"
#include <cmath> // 包含数学函数
#include <iostream>
#include <random>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("WaveHelper1");

WaveHelper::WaveHelper ()
  : m_paused (true)
{
  NS_LOG_FUNCTION (this);
}

void 
WaveHelper::Setwave(double a,double b,double c,double d,double e,double f,double g) const{
  m_wspeed=a;//Wind speed
  m_minf=b;// min frequency
  m_maxf=c;//max  frequency
  m_fnum=d;//frequency Division number
  m_mina=e;//min direction angle 
  m_maxa=f;//max direction angle 
  m_anum=g;//direction angle Division number
  
}

void
WaveHelper::SetPosition (const Vector &position)
{
  NS_LOG_FUNCTION (this << position);
  m_position = position;
  m_velocity = Vector (0.0, 0.0, 0.0);
  m_lastUpdate = Simulator::Now ();
}

Vector
WaveHelper::GetCurrentPosition (void) const
{
  NS_LOG_FUNCTION (this);
  return m_position;
}

Vector 
WaveHelper::GetVelocity (void) const
{
  NS_LOG_FUNCTION (this);
  return m_paused ? Vector (0.0, 0.0, 0.0) : m_velocity;
}
void 
WaveHelper::SetVelocity (const Vector &vel)
{
  NS_LOG_FUNCTION (this << vel);
  m_velocity = vel;
  m_lastUpdate = Simulator::Now ();
}

void
WaveHelper::Update (void) const
{
  NS_LOG_FUNCTION (this);
  Time now = Simulator::Now ();
  NS_ASSERT (m_lastUpdate <= now);
  Time deltaTime = now - m_lastUpdate;
  m_lastUpdate = now;
  if (m_paused)
    {
      return;
    }
  double deltaS = deltaTime.GetSeconds ();

  
  double t = now.GetSeconds ();



  m_position.x += m_velocity.x * deltaS;
  m_position.y += m_velocity.y * deltaS;

  double qq=0;
  double a=0;
  double g=9.8;
  double w=m_minf;//now frequency
  double o=m_mina;//now  direction angle 

  std::random_device rd;  // 用于获取随机种子
  std::mt19937 gen(rd()); // 标准梅森旋转算法，也称为Mersenne Twister
  std::uniform_real_distribution<> dis(0.0, 2 * 3.1415926); // 定义一个在 [0, 2π] 区间的分布
  double ep = dis(gen); // 生成在 [0, 2π] 区间的随机初始相位

 // double ep=0;//初始相位0-2Π随机取值
  for(int i=0;i<=m_fnum;i++){
       w=w+(m_maxf-m_minf)/m_fnum*i;
      for(int j=0;j<=m_anum;j++){
        o=o+(m_maxa-m_mina)/m_anum*j;
        a=std::sqrt(2*0.0081*g*g/std::pow(w,5)*std::exp(-0.74*std::pow(g/w/m_wspeed,4))*2/3.1415926*std::pow(std::cos(o),2)*(m_maxf-m_minf)/m_fnum*(m_maxa-m_mina)/m_anum);
        qq+=a*std::cos(w*w/g*m_position.x*std::cos(o)+w*w/g* m_position.y*std::sin(o)-w*t-ep);

      }

  }

  m_position.z =qq; 
  
  //cout << m_position.x << m_position.y << m_position.z << t << endl;
}

void 
WaveHelper::Pause (void)
{
  NS_LOG_FUNCTION (this);
  m_paused = true;
}

void 
WaveHelper::Unpause (void)
{
  NS_LOG_FUNCTION (this);
  m_paused = false;
}

} // namespace ns3
