/*
 * aqua-sim-mac-broadcast.cc
 *
 *  Created on: Sep 9, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#include "aqua-sim-mac-broadcast.h"
#include "aqua-sim-trailer.h"
#include "aqua-sim-header-mac.h"
#include "aqua-sim-address.h"

#include "ns3/log.h"
#include "ns3/integer.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AquaSimBroadcastMac");
NS_OBJECT_ENSURE_REGISTERED(AquaSimBroadcastMac);

/* ======================================================================
Broadcast MAC for  underwater sensor
====================================================================== */

AquaSimBroadcastMac::AquaSimBroadcastMac()
{
	backoffCounter = 0;
	rand = CreateObject<UniformRandomVariable>();
}

TypeId
AquaSimBroadcastMac::GetTypeId()
{
	static TypeId tid = TypeId("ns3::AquaSimBroadcastMac")
							.SetParent<AquaSimMac>()
							.AddConstructor<AquaSimBroadcastMac>();
	return tid;
}

int64_t
AquaSimBroadcastMac::AssignStreams(int64_t stream)
{
	NS_LOG_FUNCTION(this << stream);
	rand->SetStream(stream);
	return 1;
}



bool AquaSimBroadcastMac::RecvProcess(Ptr<Packet> pkt)
{
	NS_LOG_FUNCTION(this);

	MacHeader mach;
	pkt->RemoveHeader(mach);
	AquaSimAddress dst = mach.GetDA();


	if (dst == AquaSimAddress::GetBroadcast() || dst == AquaSimAddress::ConvertFrom(m_device->GetAddress()))
	{
		return SendUp(pkt);
	}

	pkt = 0;
	return true;
}


bool AquaSimBroadcastMac::TxProcess(Ptr<Packet> pkt)
{
	NS_LOG_FUNCTION(this << pkt);

	MacHeader mach;

	mach.SetDA(AquaSimAddress::GetBroadcast());
	mach.SetSA(AquaSimAddress::ConvertFrom(m_device->GetAddress()));
	pkt->AddHeader(mach);

	sendPkt(pkt);
	return true; 
}

void AquaSimBroadcastMac::sendPkt(Ptr<Packet> pkt){
	switch (m_device->GetTransmissionStatus())
	{
	case SLEEP:
		PowerOn();
	case NIDLE:
		SendDown(pkt);
		break;
	default:
		break;
	}
}


uint32_t AquaSimBroadcastMac::getHeaderSize(){
	MacHeader mach;
    return mach.GetSerializedSize();
}

void AquaSimBroadcastMac::DoDispose()
{
	NS_LOG_FUNCTION(this);
	AquaSimMac::DoDispose();
}

