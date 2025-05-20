/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 *
 * Date:2024.10.10
 * Author: Yanhai Huang <yhhuang24@mails.jlu.edu.cn>
 */

#include "underwater-glider-mobility-model.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(UnderwaterGlider);

TypeId UnderwaterGlider::GetTypeId(void)
{
  static TypeId tid = TypeId("ns3::UnderwaterGlider")
    .SetParent<MobilityModel>()
    .SetGroupName("Mobility")
    .AddConstructor<UnderwaterGlider>();
  return tid;
}

UnderwaterGlider::UnderwaterGlider()
  : m_velocity(0, 0, 0), m_minDepth(0), m_maxDepth(100), m_lastUpdateTime(Simulator::Now())
{
}

UnderwaterGlider::~UnderwaterGlider() {}

void UnderwaterGlider::SetVelocity(const Vector &velocity)
{
  m_velocity = velocity;
}

void UnderwaterGlider::SetDepthLimits(double minDepth, double maxDepth)
{
  m_minDepth = minDepth;
  m_maxDepth = maxDepth;
}

Vector UnderwaterGlider::DoGetPosition(void) const
{
  return m_position;
}

void UnderwaterGlider::DoSetPosition(const Vector &position)
{
  m_baseTime = Simulator::Now ();
  m_position = position;
}

Vector UnderwaterGlider::DoGetVelocity(void) const
{
  return m_velocity;
}

void UnderwaterGlider::UpdateMovement()
{
    Time currentTime = Simulator::Now();
    double dt = (currentTime - m_lastUpdateTime).GetSeconds();
    m_lastUpdateTime = currentTime;

    // update the position
    m_position.x += m_velocity.x * dt;
    m_position.y += m_velocity.y * dt;
    
    // update the depth
    double newDepth = m_position.z + m_velocity.z * dt;
    
    if((currentTime-m_baseTime).GetSeconds()<double(m_maxDepth/fabs(m_velocity.z)))
    {
      m_position.z= newDepth;
    }
    else if (newDepth < m_minDepth)
    {
        m_position.z = m_minDepth; // limit minimum depth
        m_velocity.z = -m_velocity.z; // reverse the velocity
    } 
    else if (newDepth > m_maxDepth) 
    {
        m_position.z = m_maxDepth; // limit maximum depth
        m_velocity.z = -m_velocity.z; // reverse the velocity
    } 
    else 
    {
        m_position.z = newDepth; // update the velocity 
    }

    // notify course change
    NotifyCourseChange();
}




} // namespace ns3
