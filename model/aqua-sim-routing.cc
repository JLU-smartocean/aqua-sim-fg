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

#include "ns3/log.h"
#include "ns3/attribute.h"
#include "ns3/simulator.h"
#include "ns3/ptr.h"
#include "ns3/pointer.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/csma-helper.h"
#include "ns3/csma-net-device.h"

#include "aqua-sim-header.h"
#include "aqua-sim-trailer.h"
#include "aqua-sim-routing.h"
#include "aqua-sim-socket.h"
#include "aqua-sim-mac.h"
#include <iostream>

#include "ns3/aqua-sim-header-mac.h"
#include "aqua-sim-pt-tag.h"

#include "aqua-sim-header-routing.h"
#include "aqua-sim-header-mac.h"
//Aqua Sim Routing

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AquaSimRouting");
NS_OBJECT_ENSURE_REGISTERED(AquaSimRouting);

TypeId AquaSimRouting::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::AquaSimRouting").SetParent<Object>().AddAttribute(
					"SetNetDevice",
					"The net device where this routing layer resides on",
					PointerValue(),
					MakePointerAccessor(&AquaSimRouting::m_device),
					MakePointerChecker<AquaSimRouting>()).AddTraceSource(
					"RoutingTx",
					"Trace source indicating a packet has started transmitting.",
					MakeTraceSourceAccessor(&AquaSimRouting::m_routingTxTrace),
					"ns3::AquaSimRouting::TxCallback").AddTraceSource(
					"RoutingRx",
					"Trace source indicating a packet has been received.",
					MakeTraceSourceAccessor(&AquaSimRouting::m_routingRxTrace),
					"ns3::AquaSimRouting::RxCallback").AddTraceSource(
					"TrafficPkts", "Amount of network traffic in packets.",
					MakeTraceSourceAccessor(&AquaSimRouting::trafficPktsTrace),
					"ns3::TracedValueCallback::Uint32").AddTraceSource(
					"TrafficBytes", "Amount of network traffic in bytes",
					MakeTraceSourceAccessor(&AquaSimRouting::trafficBytesTrace),
					"ns3::TracedValueCallback::Uint32");
	return tid;
}

AquaSimRouting::AquaSimRouting() :
		trafficPktsTrace(0), trafficBytesTrace(0), m_sendUpPktCount(0),
		averageDelay(0)
{
	m_data.clear(); //just in case.
	NS_LOG_FUNCTION(this);
	//m_tracetarget=NULL;		//to be implemented
	//ll(NULL), port_dmux(NULL)
}

AquaSimRouting::~AquaSimRouting() {
	NS_LOG_FUNCTION(this);
}

void AquaSimRouting::SetNetDevice(Ptr<AquaSimNetDevice> device) {
	NS_LOG_FUNCTION(this);
	m_device = device;
}

void AquaSimRouting::SetMac(Ptr<AquaSimMac> mac) {
	NS_LOG_FUNCTION(this << mac);
	m_mac = mac;
}

Ptr<AquaSimNetDevice> AquaSimRouting::GetNetDevice() {
	return m_device;
}
Ptr<AquaSimMac> AquaSimRouting::GetMac() {
	return m_mac;
}

/**
 * send packet p to the upper layer, i.e., port dmux
 *
 * @param p   a packet
 * */
bool AquaSimRouting::SendUp(Ptr<Packet> p) {
	AquaSimTrailer ast;
	p->PeekTrailer(ast);
	double delay = Simulator::Now().GetSeconds() - ast.GetRouteTxTime();
	averageDelay = (averageDelay*m_sendUpPktCount+delay)/(++m_sendUpPktCount);

	std::cout<<"ROUTE-" << AquaSimAddress::ConvertFrom(m_device->GetAddress())
		<< " pktNum:"<< m_sendUpPktCount
		<<" averageDelay:" << averageDelay << "\n";

	m_device->SendUp(p);
	return true;
}

/**
 * send packet p to the lower layer
 *
 * @param p			a packet
 * @param next_hop	the next hop to route packet p
 * @param delay		packet p will be sent in time of delay
 * */
bool AquaSimRouting::SendDown(Ptr<Packet> p, AquaSimAddress nextHop,
		Time delay) {
	NS_LOG_FUNCTION(this << p << nextHop << delay);
	NS_ASSERT(p != NULL);
	//NS_LOG_DEBUG("Pktsize=" << header.GetSize());
	//std::cout<<"send down\n";
	//send down after given delay

	/*Note this schedule will not work, should instead call internal function once
	 * event is executed which will internal call Mac's function directly.
	 * This should most likely be a callback.
	 */
	Simulator::Schedule(delay, &AquaSimRouting::SendPacket, this, p);
	return true;
}

void AquaSimRouting::SendPacket(Ptr<Packet> p) {
	NS_LOG_FUNCTION(this << m_mac);
	m_mac = m_device->GetMac();
	if (!m_mac->TxProcess(p)) {
		NS_LOG_DEBUG(this << "Mac recv error");
	}

}

/**
 * check if packet p is received because of a dead loop
 *
 * @param p		a packet
 * @return		true for p experienced a dead loop, false for not
 * */

bool AquaSimRouting::IsDeadLoop(Ptr<Packet> p) {
	// TODO:跳数应从路由头取出
	NS_LOG_FUNCTION(this);
	// AquaSimHeader asHeader;
	RoutingHeader rtHeader;
	p->PeekHeader(rtHeader);
	return (rtHeader.GetSA()
			== AquaSimAddress::ConvertFrom(m_device->GetAddress()))
			&& (rtHeader.GetNumForwards() > 0);
}

/**
 * check if this node is the source of packet p, i.e. p is generated by this node
 *
 * @param p		a packet
 * @return		true for yes, false for no
 * */
bool AquaSimRouting::AmISrc(const Ptr<Packet> p) {
	NS_LOG_FUNCTION(this);
	AquaSimHeader asHeader;
	p->PeekHeader(asHeader);

	// std::cout<<asHeader.GetSAddr();
	//std::cout<<AquaSimAddress::ConvertFrom(m_device->GetAddress());

	return (asHeader.GetSAddr()
			== AquaSimAddress::ConvertFrom(m_device->GetAddress())
			&& asHeader.GetNumForwards() == 0);
}

/**
 * check if this node is the destination of packet p, i.e. p is destined to this node
 *
 * @param p		a packet
 * @return		true for yes, false for no
 * */
bool AquaSimRouting::AmIDst(const Ptr<Packet> p) {
	NS_LOG_FUNCTION(this);
	RoutingHeader rtHeader;
	p->PeekHeader(rtHeader);

	return (rtHeader.GetDA()
			== AquaSimAddress::ConvertFrom(m_device->GetAddress()));

	// return (asHeader.GetDAddr()
	// 		== AquaSimAddress::ConvertFrom(m_device->GetAddress()));
}

/**
 * check if this node is the next hop of packetr p,
 * i.e., this node needs to forward p later on.
 *
 * @param p		a packet
 * @return		true for yes, false for no
 * */
bool AquaSimRouting::AmINextHop(const Ptr<Packet> p) {
	NS_LOG_FUNCTION(this);
	AquaSimHeader asHeader;
	p->PeekHeader(asHeader);
	/*std::cout<<"asheader:"<<asHeader.GetNextHop();
	 std::cout<<"m_device:"<<AquaSimAddress::ConvertFrom(m_device->GetAddress());
	 std::cout<<"broadcast:"<<AquaSimAddress::GetBroadcast();*/

	return ((asHeader.GetNextHop()
			== AquaSimAddress::ConvertFrom(m_device->GetAddress()))
			|| (asHeader.GetNextHop() == AquaSimAddress::GetBroadcast()));
}

void AquaSimRouting::NotifyRx(std::string path, Ptr<Packet> p) {
	m_routingRxTrace = p;
	SendUp(p);
	NS_LOG_UNCOND(path << " RX " << p->ToString());
}

void AquaSimRouting::NotifyTx(std::string path, Ptr<Packet> p,
		AquaSimAddress nextHop, Time delay) {
	m_routingTxTrace = p;
	SendDown(p, nextHop, delay);
	NS_LOG_UNCOND(path << " TX " << p->ToString());
}

void AquaSimRouting::DoDispose() {
	NS_LOG_FUNCTION(this);
	m_device = 0;
	m_mac = 0;
	Object::DoDispose();
}

void AquaSimRouting::AssignInternalData(std::vector<std::string> collection) {
	NS_ASSERT(!collection.empty());

	m_data = collection;
}

void AquaSimRouting::AssignInternalDataPath(
		std::vector<std::string> collection) {
	NS_ASSERT(!collection.empty());

	m_knownDataPath = collection;
}

bool AquaSimRouting::TxProcess(Ptr<Packet> p, const Address &dest, uint16_t protocolNumber) {
	/**
	 * 路由接受函数的判断逻辑如下:
	 * 1.加载路由表
	 * 2.判断是否为本节点数据	是:接收并向上传递
	 * 3.找到下一跳，判断是否死循环	是:不处理该包
	 * 4.更新头部信息
	 * 5.发包
	 */
	// dest如果为???,说明包从下方来，不做添加头部处理
	RoutingHeader rth;

	RoutingHeader routingHeader;
	routingHeader.SetNumForwards((uint16_t)0);
	routingHeader.SetSA(AquaSimAddress::ConvertFrom(m_device->GetAddress()));
	routingHeader.SetDA(AquaSimAddress::ConvertFrom(dest));
	p->AddHeader(routingHeader);

	return Recv(p, dest, protocolNumber);
}

/*
 int AquaSimRouting::TrafficInBytes(bool diff)
 {
 if (diff) //diff from last trace.
 {
 int byteDiff = trafficBytes - lastTrafficTrace;
 lastTrafficTrace = trafficBytes;
 return byteDiff;
 }
 return trafficBytes;
 }
 */

}  //namespace ns3
