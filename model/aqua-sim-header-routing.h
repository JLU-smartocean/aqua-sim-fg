/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 University of Connecticut
 *
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
 * Author: Robert Martin <robert.martin@engr.uconn.edu>
 */


#ifndef AQUA_SIM_HEADER_ROUTING_H
#define AQUA_SIM_HEADER_ROUTING_H

//#include <string>
#include <iostream>

#include "ns3/header.h"
//#include "ns3/nstime.h"
#include "ns3/vector.h"

#include "aqua-sim-address.h"
//#include "aqua-sim-routing-buffer.h"
#include "aqua-sim-datastructure.h"

#define	DBRH_DATA_GREEDY	0
#define	DBRH_DATA_RECOVER	1
#define	DBRH_BEACON	    	2

namespace ns3 {

class RoutingHeader : public Header
{
public:
  RoutingHeader(/* args */);
  virtual ~RoutingHeader();
  static TypeId GetTypeId();
  static int GetSize();

  // Setters
  void SetSA(AquaSimAddress sa);
  void SetDA(AquaSimAddress da);
  void SetNumForwards(uint16_t numForwards);

  // Getters
  AquaSimAddress GetSA();
  AquaSimAddress GetDA();
  uint16_t GetNumForwards();

  // inherited methods
  virtual uint32_t GetSerializedSize(void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  virtual TypeId GetInstanceTypeId(void) const;

private:
  private:
  uint16_t m_numForwards;   //待定
  AquaSimAddress  m_sa; // Node which originated this packet
  AquaSimAddress  m_da; // Node which is the receiver
};// class RoutingHeader


 /**
  * \ingroup aqua-sim-ng
  *
  * \brief Dynamic routing header
  */
class DRoutingHeader : public Header
{
public:
  DRoutingHeader();
  virtual ~DRoutingHeader();
  static TypeId GetTypeId();

  AquaSimAddress GetPktSrc();
  AquaSimAddress GetPktDst();//xj
  uint16_t GetNumForwards();
  uint16_t GetPktLen();
  uint16_t GetPktSeqNum();
  uint32_t GetEntryNum();
  void SetPktSrc(AquaSimAddress pktSrc);
  void SetPktDst(AquaSimAddress pktDst);//xj
  void SetNumForwards(uint16_t number);
  void SetPktLen(uint16_t pktLen);
  void SetPktSeqNum(uint16_t pktSeqNum);
  void SetEntryNum(uint32_t entryNum);

  //inherited methods
  virtual uint32_t GetSerializedSize(void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  virtual TypeId GetInstanceTypeId(void) const;

private:
  AquaSimAddress  m_pktSrc; // Node which originated this packet
  AquaSimAddress m_pktDst;//xj destination
  uint16_t numForward;
  uint16_t m_pktLen; // Packet length (in bytes)
  uint16_t  m_pktSeqNum; // Packet sequence number
  uint32_t m_entryNum; // Packet length (in bytes)
  //add by jun-------routing table
}; // class DRoutingHeader

 /**
  * \brief Vector Based routing header
  * Protocol paper: engr.uconn.edu/~jcui/UWSN_papers/vbf_networking2006.pdf
  */
class VBHeader : public Header
{
public:
  VBHeader();
  virtual ~VBHeader();
  static TypeId GetTypeId();

  //Setters
  void SetMessType(uint8_t messType);
  void SetPkNum(uint32_t pkNum);
  void SetTargetAddr(AquaSimAddress targetAddr);
  void SetSenderAddr(AquaSimAddress senderAddr);
  void SetForwardAddr(AquaSimAddress forwardAddr);
  void SetDataType(uint8_t dataType);
  void SetNumForwards(uint16_t numforwards);
  void SetOriginalSource(Vector originalSource);
  void SetToken(uint32_t token);
  void SetTs(uint32_t ts);
  void SetRange(uint32_t range);
  void SetExtraInfo(uw_extra_info info);
  void SetExtraInfo_o(Vector position_o);
  void SetExtraInfo_f(Vector position_f);
  void SetExtraInfo_t(Vector position_t);
  void SetExtraInfo_d(Vector position_d);

  //Getters
  uint8_t GetMessType();
  uint32_t GetPkNum();
  AquaSimAddress GetTargetAddr();
  AquaSimAddress GetSenderAddr();
  AquaSimAddress GetForwardAddr();
  uint8_t GetDataType();
  uint16_t GetNumForwards();
  Vector GetOriginalSource();
  uint32_t GetToken();
  uint32_t GetTs();
  uint32_t GetRange();
  uw_extra_info GetExtraInfo();

  //inherited methods
  virtual uint32_t GetSerializedSize(void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  virtual TypeId GetInstanceTypeId(void) const;

private:
  uint8_t m_messType;  //message type
  uint32_t m_pkNum;   //packet sequence num
  AquaSimAddress m_targetAddr; // the target addr of this data
  AquaSimAddress m_senderAddr;  //original sender addr
  AquaSimAddress m_forwardAddr;// the forwarder addr
  // AquaSimAddress next_nodes[MAX_NEIGHBORS];
  //int num_next;
  uint8_t m_dataType; //what is this for?
  uint16_t m_numForwards;
  Vector m_originalSource;
  uint32_t m_token;
  uint32_t m_ts;      // Timestamp when pkt is generated.
  uint32_t m_range;    // target range
  struct uw_extra_info m_info;
  //int report_rate;               // For simple diffusion only.
  //int attr[MAX_ATTRIBUTE];
};  // class VBHeader


 /**
  * \brief Depth Based routing header
  * Protocol paper:  http://www.cse.uconn.edu/~jcui/UWSN_papers/Networking08_DBR.pdf
  */
class DBRHeader : public Header
{
public:
  DBRHeader();
  virtual ~DBRHeader();
  static TypeId GetTypeId();

  int Size();

  //Setters
  void SetPosition(Vector position);
  void SetPacketID(uint32_t packetID);
  void SetMode(uint8_t mode);
  void SetNHops(uint16_t hops);
  void SetPrevHop(AquaSimAddress prevHop);
  void SetTarget(AquaSimAddress target);
  void SetNumForward(uint16_t numforward);
  void SetOwner(AquaSimAddress owner);
  void SetDepth(double depth);

  //Getters
  Vector GetPosition();
  uint32_t GetPacketID();
  uint8_t GetMode();
  uint16_t GetNHops();
  AquaSimAddress GetPrevHop();
  AquaSimAddress GetTarget();
  uint16_t GetNumForward();
  AquaSimAddress GetOwner();
  double GetDepth();

  //inherited methods
  virtual uint32_t GetSerializedSize(void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  virtual TypeId GetInstanceTypeId(void) const;

private:
  Vector m_position;
  uint32_t m_packetID;  // unique id for packet
  //uint16_t m_
  //int valid_;		// is this header in the packet? //not used...
  uint8_t m_mode; // routing mode: greedy|recovery|beacon
  uint16_t m_nhops;		// max # of hops to broadcast
        // in recovery mode
  AquaSimAddress m_prevHop;	// the sender
  AquaSimAddress m_target;
  uint16_t m_numForward;
  AquaSimAddress m_owner;	// from whom the sender got it
  double m_depth;		// the depth of last hop
};  // class DBRHeader


 /**
  * \brief DDoS routing header
  * Protocol paper: Data Centric Approach to Analyzing Security Threats in Underwater Sensor Networks
  */
class DDOSHeader : public Header
{
public:
  enum DdosPacketType { Interest, Data, NACK, Alert };

  DDOSHeader();
  virtual ~DDOSHeader();
  static TypeId GetTypeId();

  void SetPacketType(uint8_t pt);
  void SetRowIndex(uint32_t index);
  uint8_t GetPacketType();
  uint32_t GetRowIndex();

  //inherited methods
  virtual uint32_t GetSerializedSize(void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  virtual TypeId GetInstanceTypeId(void) const;

private:
  uint8_t m_pt;
  uint32_t m_index;  //used to mimic naming hierarchy without intensive naming splitting

};  // class DDOSHeader

}  // namespace ns3

#endif /* AQUA_SIM_HEADER_ROUTING_H */
