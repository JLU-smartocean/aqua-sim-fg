#include "CBXmodel.h"
#include "ns3/simulator.h"
#include "ns3/vector.h"
#include "ns3/log.h"
#include "ns3/command-line.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("CBXModel");
NS_OBJECT_ENSURE_REGISTERED (CBXModel);

TypeId CBXModel::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::CBXModel")
                        .SetParent<MobilityModel> ()
                        .SetGroupName ("Mobility")
                        .AddConstructor<CBXModel> ();
    return tid;
}

CBXModel::CBXModel ()
{
    m_Command = 0;
    m_Velocity = 0.0;
    m_Direction = 0.0;
    m_Distance = 0.0;
    m_Radius = 0.0;
    m_Angle = 0.0;
    m_Event = Simulator::ScheduleNow (&CBXModel::GetCommand, this);
    //m_Helper.pause();
}

void CBXModel::GetCommand(void)
{
    std::cout << std::fixed << std::setprecision(2);
    std::string line;
    std::ifstream configFile("src/model/aqua-sim-fg/config/command.config");
    if (!configFile.is_open())
    {
        NS_LOG_DEBUG("Cannot open the configuration file.");
        return;
    }

    Time pauseTime = Seconds(0);
    Vector position = m_Helper.GetCurrentPosition();
    NS_LOG_DEBUG("Initial position: x = " << position.x << " y = " << position.y << " z = " << position.z);

    // Read the command and ignore the trailing newline character.
    while (getline(configFile, line))
    {
        std::istringstream istr(line);
        istr >> m_Command;

        if (istr.fail())
        {
            NS_LOG_DEBUG("Failed to read the command.");
            configFile.close();
            return;
        }

        switch (m_Command)
        {
        //0 -- Stop
        case 0:
            m_Event = Simulator::ScheduleNow(&CBXModel::Pause, this);
            break;
        //1 -- linear motion
        case 1:
            istr >> m_Velocity >> m_Distance >> m_Direction;
            if (istr.fail())
            {
                NS_LOG_DEBUG("Error reading the linear motion parameters.");
                configFile.close();
                return;
            }
            //DoWalk();

            m_Event = Simulator::ScheduleNow(&CBXModel::DoWalk, this);
            break;
        //2 -- circular motion
        case 2:
            istr >> m_Velocity >> m_Radius >> m_Angle;
            if (istr.fail())
            {
                NS_LOG_DEBUG("Error reading the circular motion parameters.");
                configFile.close();
                return;
            }
            if (m_Angle != 0)
                m_Event = Simulator::ScheduleNow(&CBXModel::DoCircle, this);
            break;
        //3 -- exit
        case 3:
            m_Event = Simulator::ScheduleNow(&CBXModel::Exit, this);
            break;
        //default
        default:
            m_Helper.SetVelocity(Vector(0.0, 0.0, 0.0));
            //m_Event = Simulator::ScheduleNow(&CBXModel::GetCommand, this);
            break;
        }

    }

    configFile.close();
}

void CBXModel::Pause()
{
    NS_LOG_DEBUG("Node stationary");
    Time now = Simulator::Now();
    double t = now.GetSeconds();
    std::cout << t << std::endl;
    Vector position = m_Helper.GetCurrentPosition();
    NS_LOG_DEBUG("Position: x = " << position.x << " y = " << position.y << " z = " << position.z);
    
    return;
}

void CBXModel::DoWalk()
{
    NS_LOG_DEBUG("Node linear motion");
    NS_LOG_DEBUG("Linear velocity: " << m_Velocity << " distance: " << m_Distance << " direction: " << m_Direction << "°");
    //m_Helper.Update();
    m_Direction = m_Direction * M_PI / 180.0;
    double cosD = std::cos(m_Direction);
    double sinD = std::sin(m_Direction);
    m_Helper.SetVelocity(Vector(m_Velocity * cosD, m_Velocity * sinD, 0.0));

    double time = m_Distance / m_Velocity;
    m_Helper.Unpause();
    Time pause = Seconds(time);
    Vector currentPosition = m_Helper.GetCurrentPosition();
    DoWalkEnd(currentPosition);
    //m_Event = Simulator::Schedule(pause, &CBXModel::DoWalkEnd, this, currentPosition);

    NotifyCourseChange();
}

void CBXModel::DoWalkEnd(Vector currentPosition)
{
    //std::cout << "DoWalkEnd" << std::endl;
    double time = m_Distance / m_Velocity;
    Vector velocity = m_Helper.GetVelocity();
    currentPosition.x += velocity.x * time;
    currentPosition.y += velocity.y * time;
    currentPosition.z += velocity.z * time;
    m_Helper.SetPosition(currentPosition);
    //m_Helper.Update();
    //m_Helper.Pause();
    Vector position = m_Helper.GetCurrentPosition();
    NS_LOG_DEBUG("Position: x = " << position.x << " y = " << position.y << " z = " << position.z);
    
    NotifyCourseChange();
}

void CBXModel::DoCircle()
{
    NS_LOG_DEBUG("Node circular motion");
    NS_LOG_DEBUG("Linear velocity: " << m_Velocity << " radius: " << m_Radius << " angle: " << m_Angle << "°");
    //m_Helper.Update();
    m_Angle *= M_PI / 180.0;
    double angularVelocity = m_Velocity / m_Radius;
    double time = m_Angle / angularVelocity;

    m_Helper.Unpause();
    Time pause = Seconds(time);
    Vector currentPosition = m_Helper.GetCurrentPosition();
    NS_LOG_DEBUG("Current position: x = " << currentPosition.x << " y = " << currentPosition.y << " z = " << currentPosition.z);
    Vector centerPosition = CalculateCenterPosition(currentPosition);
    NS_LOG_DEBUG("centerposition: x = " << centerPosition.x << " y = " << centerPosition.y << " z = " << centerPosition.z);
    DoCircleEnd(currentPosition, centerPosition);
    //m_Event = Simulator::Schedule(pause, &CBXModel::DoCircleEnd, this, currentPosition, centerPosition);

    NotifyCourseChange();
}

Vector CBXModel::CalculateCenterPosition(const Vector &currentPosition)
{
    double x0 = currentPosition.x;
    double y0 = currentPosition.y;
    double r = m_Radius;
    double theta = m_Angle;

    double xc = x0 - r * std::cos(theta);
    double yc = y0 - r * std::sin(theta);

    return Vector(xc, yc, 0.0);
}

void CBXModel::DoCircleEnd(Vector currentPosition , Vector centerPosition)
{
    //sstd::cout << std::cos(m_Angle) << std::sin(m_Angle) << std::endl;
    Vector newPosition;
    newPosition.x = centerPosition.x + m_Radius * std::cos(m_Angle);
    newPosition.y = centerPosition.y + m_Radius * std::sin(m_Angle);
    newPosition.z = 0.0;
    //Vector newPosition = centerPosition + Vector(m_Radius * std::cos(m_Angle), m_Radius * std::sin(m_Angle), 0.0);
    NS_LOG_DEBUG("Newposition: x = " << newPosition.x << " y = " << newPosition.y << " z = " << newPosition.z);
    m_Helper.SetPosition(newPosition);
    m_Helper.Pause();
    //m_Event = Simulator::ScheduleNow(&CBXModel::GetCommand, this);
    Vector position = m_Helper.GetCurrentPosition();
    NS_LOG_DEBUG("Position: x = " << position.x << " y = " << position.y << " z = " << position.z);
    
    NotifyCourseChange();
}

void CBXModel::Exit()
{
    NS_LOG_DEBUG("Node exits motion");
    Vector position = m_Helper.GetCurrentPosition();
    NS_LOG_DEBUG("Position: x = " << position.x << " y = " << position.y << " z = " << position.z);
    Simulator::Remove(m_Event);
}

Vector CBXModel::DoGetPosition (void) const
{
    m_Helper.Update();
    return m_Helper.GetCurrentPosition();
}

void CBXModel::DoSetPosition (const Vector &position)
{
    m_Helper.SetPosition(position);
    Simulator::Remove(m_Event);
    m_Event = Simulator::ScheduleNow(&CBXModel::GetCommand, this);
}

Vector CBXModel::DoGetVelocity (void) const
{
    return m_Helper.GetVelocity();
}

}
