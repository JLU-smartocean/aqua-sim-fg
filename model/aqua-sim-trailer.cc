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

#include "aqua-sim-trailer.h"
#include "aqua-sim-address.h"
#include "ns3/trailer.h"
#include "ns3/buffer.h"
#include "ns3/log.h"
#include "ns3/address-utils.h"
#include "ns3/address.h"

#include <iostream>
#include <bitset>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AquaSimTrailer");
NS_OBJECT_ENSURE_REGISTERED(AquaSimTrailer);

AquaSimTrailer::AquaSimTrailer(void) : 
    m_modeId(1), m_demuxPType(1), bandId(99)
{
    
  m_nextHop = AquaSimAddress(-1);
  m_src.addr = AquaSimAddress((uint16_t)1);
  m_dst.addr = AquaSimAddress(-1);

  NS_LOG_FUNCTION(this);
}

AquaSimTrailer::~AquaSimTrailer(void)
{
    NS_LOG_FUNCTION(this);
}

TypeId
AquaSimTrailer::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::AquaSimTrailer")
        .SetParent<Trailer>()
        .AddConstructor<AquaSimTrailer>()
        .AddAttribute("modeId", "The packet trans mode from 0~5. Default is 1",
            UintegerValue(1),
            MakeIntegerAccessor(&AquaSimTrailer::m_modeId),
            MakeIntegerChecker<uint8_t>())
        .AddAttribute("demuxPType", "The packet demuxPType.",
            UintegerValue(1),
            MakeIntegerAccessor(&AquaSimTrailer::m_demuxPType),
            MakeIntegerChecker<uint8_t>())
    ;
    return tid;
}

TypeId 
AquaSimTrailer::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

uint32_t 
AquaSimTrailer::GetSerializedSize(void) const
{
    return (2+2+2+1+1+1+4*2);
}

void 
AquaSimTrailer::Serialize(Buffer::Iterator start) const
{
    start.Prev(GetSerializedSize());
    Buffer::Iterator i = start;
    i.WriteU16(m_nextHop.GetAsInt());
    i.WriteU16(m_src.addr.GetAsInt());
    i.WriteU16(m_dst.addr.GetAsInt());
    i.WriteU8(m_modeId);
    i.WriteU8(m_demuxPType);
    i.WriteU8(bandId);
    i.WriteU32(uint32_t(sinr * 1000));
    i.WriteU32(uint32_t(routeTxTime * 1000));
}

uint32_t 
AquaSimTrailer::Deserialize(Buffer::Iterator start)
{
    start.Prev(GetSerializedSize());
    Buffer::Iterator i = start;
    m_nextHop  = (AquaSimAddress) i.ReadU16();
    m_src.addr = (AquaSimAddress) i.ReadU16();
    m_dst.addr = (AquaSimAddress) i.ReadU16();
    m_modeId   = i.ReadU8();
    m_demuxPType = i.ReadU8();
    bandId = i.ReadU8();
    sinr = (int)i.ReadU32() / 1000.0;
    routeTxTime = i.ReadU32() / 1000.0;

    return GetSerializedSize();
}

void 
AquaSimTrailer::Print(std::ostream &os) const
{
    os << "Packet info is ";
    os << "modeId = " << (int)m_modeId << " , "
       << "bandId = " << (int)bandId << " , " 
       << "demuxPType = " << (int)m_demuxPType << " . ";
    os << "SenderAddr = " << m_src.addr
       << ", DestAddr = " << m_dst.addr 
       << " , NextHop = " << m_nextHop 
       << "\n";
}

AquaSimAddress 
AquaSimTrailer::GetNextHop()
{
    return m_nextHop;
}

AquaSimAddress 
AquaSimTrailer::GetSAddr()
{
    return m_src.addr;
}

AquaSimAddress 
AquaSimTrailer::GetDAddr()
{
    return m_dst.addr;
}

int32_t 
AquaSimTrailer::GetSPort()
{
    return m_src.port;
}

int32_t 
AquaSimTrailer::GetDPort()
{
    return m_dst.port;
}

uint8_t 
AquaSimTrailer::GetModeId()
{
    return m_modeId;
}

uint8_t 
AquaSimTrailer::GetBandId()
{
    return bandId;
}

uint8_t 
AquaSimTrailer::GetDemuxPType()
{
    return m_demuxPType;
}

void 
AquaSimTrailer::SetNextHop(AquaSimAddress nextHop)
{
    m_nextHop = nextHop;
}

void 
AquaSimTrailer::SetSAddr(AquaSimAddress sAddr)
{
    m_src.addr = sAddr;
}

void 
AquaSimTrailer::SetDAddr(AquaSimAddress dAddr)
{
    m_dst.addr = dAddr;
}

void 
AquaSimTrailer::SetSPort(port32_t sPort)
{
    m_src.port = sPort;
}

void 
AquaSimTrailer::SetDPort(port32_t dPort)
{
    m_dst.port = dPort;
}

void 
AquaSimTrailer::SetModeId(uint8_t modeId)
{
    m_modeId = modeId;
}

void 
AquaSimTrailer::SetBandId(uint8_t bandId)
{
    this->bandId = bandId;
}


void 
AquaSimTrailer::SetDemuxPType(uint8_t demuxPType)
{
    m_demuxPType = demuxPType;
}



double AquaSimTrailer::GetSinr(){
    return sinr;
}

void 
AquaSimTrailer::SetSinr(double sinr){
    this->sinr = sinr;
}


double AquaSimTrailer::GetRouteTxTime(){
    return routeTxTime;
}
void AquaSimTrailer::SetRouteTxTime(Time t){
    routeTxTime = t.GetSeconds();
}

void AquaSimTrailer::SetRouteTxTime(double t){
    routeTxTime = t;
}