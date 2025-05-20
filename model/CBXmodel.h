#ifndef CBXMODEL_H
#define CBXMODEL_H

#include "ns3/nstime.h"
#include <stdint.h>
#include "ns3/mobility-model.h"
#include "ns3/constant-velocity-helper.h"
#include "ns3/vector.h"


namespace ns3
{

class CBXModel : public MobilityModel
{
public:
    static TypeId GetTypeId (void);
    CBXModel();
    
private:
    void GetCommand (void);
    void Pause();
    void DoWalk();
    void DoWalkEnd(Vector currentPosition);
    void DoCircle();
    Vector CalculateCenterPosition(const Vector &currentPosition);
    void DoCircleEnd(Vector currentPosition, Vector centerPosition);
    void Exit();

    virtual Vector DoGetPosition (void) const;
    virtual void DoSetPosition (const Vector &position);
    virtual Vector DoGetVelocity (void) const;
    
    ConstantVelocityHelper m_Helper;
    int m_Command;
    double m_Velocity;
    double m_Direction;
    double m_Distance;
    double m_Radius;
    double m_Angle;
    EventId m_Event;

};

}

#endif
