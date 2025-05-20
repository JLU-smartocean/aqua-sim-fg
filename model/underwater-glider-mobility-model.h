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

#ifndef UNDERWATER_GLIDER_MOBILITY_MODEL_H
#define UNDERWATER_GLIDER_MOBILITY_MODEL_H

#include "ns3/mobility-model.h"
#include "ns3/nstime.h"

namespace ns3 {

class UnderwaterGlider : public MobilityModel 
{
public:

  /**
   * \ingroup mobility
   * \brief Mobility model for underwater glider
   */
  static TypeId GetTypeId(void);
  
  UnderwaterGlider();
  virtual ~UnderwaterGlider();

  /**
   * Set the initial velocity of the model.
   * When the value of velocity.z is positive, the motion direction is downward. 
   * A negative value indicates the direction is upward.
   * \param velocity the velocity (m/s)
   */
  void SetVelocity(const Vector &velocity);

  /**
   * Set the range for the diving depth of the model.
   * All depths are positive values, and a larger value indicates a greater depth.
   * \param minDepth the minimum depth (m)
   * \param maxDepth the maximum depth (m)
   */
  void SetDepthLimits(double minDepth, double maxDepth);

  /**
   * Update the position of the model.
   */
  void UpdateMovement();



private:
  virtual Vector DoGetPosition(void) const override;
  virtual void DoSetPosition(const Vector &position) override;
  virtual Vector DoGetVelocity(void) const override;

  Vector m_velocity;
  Vector m_position;
  double m_minDepth;
  double m_maxDepth;
  Time m_lastUpdateTime;
  Time m_baseTime; 
};

} // namespace ns3

#endif /* UNDERWATER_GLIDER_MOBILITY_MODEL_H */
