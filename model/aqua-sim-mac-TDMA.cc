/*
 * aqua-sim-mac-TDMA.cc
 *
 *  Created on: Oct 28, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#include "aqua-sim-mac-TDMA.h"
#include "aqua-sim-pt-tag.h"
#include "aqua-sim-header.h"
#include "aqua-sim-trailer.h"
#include "aqua-sim-header-mac.h"

#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/random-variable-stream.h"
#include "ns3/simulator.h"
#include "ns3/integer.h"
#include "ns3/double.h"
#include <iostream>

namespace ns3
{

	NS_LOG_COMPONENT_DEFINE("AquaSimTDMA");
	NS_OBJECT_ENSURE_REGISTERED(AquaSimTDMA);


	//construct function
	AquaSimTDMA::AquaSimTDMA() 
	:AquaSimMac()
	{
		guardTime = 0.01;
		slotNum = 6;
		m_rand = CreateObject<UniformRandomVariable>();
		Simulator::Schedule(Seconds(0.1), &AquaSimTDMA::init, this);
	}

	void AquaSimTDMA::init(){
		m_maxPropDelay = Device()->GetPhy()->GetTransRange() / 1500.0;
		token = AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt() % slotNum; // node token
		slotLen = Seconds(m_maxPropDelay + guardTime) + GetTxTime(80, 1);
	} 

	AquaSimTDMA::~AquaSimTDMA()
	{
	}

	TypeId
	AquaSimTDMA::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::AquaSimTDMA")
		.SetParent<AquaSimMac>()
		.AddConstructor<AquaSimTDMA>();
		return tid;
	}

	int64_t
	AquaSimTDMA::AssignStreams(int64_t stream)
	{
		NS_LOG_FUNCTION(this << stream);
		m_rand->SetStream(stream);
		return 1;
	}

	
	Time AquaSimTDMA::calTime2Send(){
		int num = (int)(Simulator::Now().GetDouble() / slotLen.GetDouble());
        int nowT = num % slotNum;
        int interval = (token - nowT + slotNum) % slotNum;
        if(interval == 0)
            interval = slotNum;
        Time t = interval * slotLen + num * slotLen - Simulator::Now();
        return t;
	}


	/*===========================Send and Receive===========================*/

	bool AquaSimTDMA::TxProcess(Ptr<Packet> pkt)
	{
		NS_LOG_FUNCTION(m_device->GetAddress() << pkt << Simulator::Now().GetSeconds());
		AquaSimTrailer ast;
		MacHeader mach;
		pkt->PeekTrailer(ast);

		mach.SetSA(AquaSimAddress::ConvertFrom(m_device->GetAddress()));
		mach.SetDA(ast.GetNextHop());

		pkt->AddHeader(mach);

		if(!pktQue.empty()){
			pktQue.push(pkt);
			return true;
		}
		
		pktQue.push(pkt);
		Time t = calTime2Send();
		Simulator::Schedule(t, &AquaSimTDMA::SendPkt, this);
		return true;
	}

	void AquaSimTDMA::scheduleNextPkt(){
		pktQue.pop();
		if(pktQue.empty())
			return;
		Time t = calTime2Send();
		Simulator::Schedule(t, &AquaSimTDMA::SendPkt, this);
	}

	void AquaSimTDMA::SendPkt()
	{	
		Ptr<Packet> pkt = pktQue.front()->Copy();
		MacHeader mach;

		pkt->PeekHeader(mach);

		switch (m_device->GetTransmissionStatus())
		{
		case SLEEP:
			PowerOn();

		case NIDLE:
		{
			NS_LOG_DEBUG("TDMA-" <<AquaSimAddress::ConvertFrom(m_device->GetAddress()) 
			<< " SendDown " 
			<< " dest: " << mach.GetDA() << " "<< Simulator::Now().GetSeconds());
			SendDown(pkt);
			Simulator::Schedule(slotLen, &AquaSimTDMA::scheduleNextPkt, this);
			break;
		}
		default:
			NS_LOG_WARN("TDMA-send false");
		}
	}	

	bool AquaSimTDMA::RecvProcess(Ptr<Packet> pkt)
	{
		NS_LOG_FUNCTION(m_device->GetAddress());
		MacHeader mach;
		pkt->RemoveHeader(mach);

		NS_LOG_DEBUG("TDMA-" << AquaSimAddress::ConvertFrom(m_device->GetAddress()) 
			<< " recv " 
			<< " dest: " << mach.GetDA() << " "<< Simulator::Now().GetSeconds());


		AquaSimAddress recver = mach.GetDA();
		AquaSimAddress myAddr = AquaSimAddress::ConvertFrom(m_device->GetAddress());

		if(recver != myAddr && recver != AquaSimAddress::GetBroadcast()){
			pkt = 0;
			return true;
		}
		
		SendUp(pkt->Copy());
		pkt = 0;
		return true;
	}



	void AquaSimTDMA::DoDispose()
	{
		NS_LOG_FUNCTION(this);
		while (!pktQue.empty())
		{
			pktQue.front() = 0;
			pktQue.pop();
		}
		m_rand = 0;
		AquaSimMac::DoDispose();
	}

} // namespace ns3
