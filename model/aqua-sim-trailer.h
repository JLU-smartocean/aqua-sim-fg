/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: An Baining <anbn2118@mails.jlu.edu.cn>
 */

#ifndef AQUA_SIM_TRAILER_H
#define AQUA_SIM_TRAILER_H


#include "ns3/address.h"
#include "ns3/trailer.h"
#include "ns3/nstime.h"

#include "aqua-sim-address.h"

namespace ns3
{

class Packet;

/* Structure describing an socket address */
typedef int32_t port32_t;
struct s_addr_t
{
    AquaSimAddress addr;    /* underwater netdevice address */
    port32_t port;      /* port address. Currently not used */
};

/**
 * \ingroup aqua-sim-ng
 *
 * \brief Generic Aqua-Sim trailer.
 * Used to store the parameters that each packet passes across layers.
 */
class AquaSimTrailer : public Trailer
{
public:
    AquaSimTrailer();
    virtual ~AquaSimTrailer();
    static TypeId GetTypeId(void);

    // Getters
    AquaSimAddress GetNextHop();	// next hop for this packet
    AquaSimAddress GetSAddr();
    AquaSimAddress GetDAddr();
    int32_t GetSPort();
    int32_t GetDPort();
    uint8_t GetModeId();
    uint8_t GetDemuxPType();
    uint8_t GetBandId();

    // Setters
    void SetNextHop(AquaSimAddress nextHop);
    void SetSAddr(AquaSimAddress sAddr);
    void SetDAddr(AquaSimAddress dAddr);
    void SetSPort(port32_t sPort);
    void SetDPort(port32_t dPort);
    void SetModeId(uint8_t modeId);
    void SetBandId(uint8_t modeId);
    void SetDemuxPType(uint8_t demuxPType);

    double GetSinr();
    void SetSinr(double sinr);
    double GetRouteTxTime();
    void SetRouteTxTime(Time t);
    void SetRouteTxTime(double t);



    //inherited by Trailer class
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream &os) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);
    virtual uint32_t GetSerializedSize(void) const;

private:
    AquaSimAddress m_nextHop;   //<! the next-hop for this packet
    s_addr_t m_src;             //<! Packet source address
    s_addr_t m_dst;             //<! Packet destination address
    uint8_t m_modeId;           //<! Packet transmission mode
    uint8_t m_demuxPType;       //<! 
    uint8_t bandId;  //FDM bandId, 99 is phy select, 255 is all band.
    double sinr;
    double routeTxTime;
}; //class AquaSimTrailer

} // namespace ns3




#endif /* AQUA_SIM_TRAILER_H */