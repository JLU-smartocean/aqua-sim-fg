/*
 * aqua-sim-mac-aloha.cc
 *
 *  Created on: Aug 25, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#include "aqua-sim-mac-aloha.h"
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

	NS_LOG_COMPONENT_DEFINE("AquaSimAloha");
	NS_OBJECT_ENSURE_REGISTERED(AquaSimAloha);




	std::vector<std::string> AquaSimAloha::alohaStatusName = {
		"PASSIVE", "BACKOFF", "SEND", "WAIT_ACK"
	};

	//construct function
	AquaSimAloha::AquaSimAloha() 
	:AquaSimMac(), sendCnt(0), ALOHA_Status(PASSIVE), m_persistent(1.0),
	m_AckOn(1), m_minBackoff(0.0), m_maxBackoff(1.5), m_maxACKRetryInterval(0.05)
	{
		guardTime = 0.01;
		m_rand = CreateObject<UniformRandomVariable>();

		Simulator::Schedule(Seconds(0.1), &AquaSimAloha::init, this);
	}

	void AquaSimAloha::init(){
		m_maxPropDelay = Device()->GetPhy()->GetTransRange() / 1500.0;
	} 

	AquaSimAloha::~AquaSimAloha()
	{
	}

	TypeId
	AquaSimAloha::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::AquaSimAloha")
		.SetParent<AquaSimMac>()
		.AddConstructor<AquaSimAloha>()
		.AddAttribute("Persistent", "Persistence of sending data packets",
			DoubleValue(1.0),
			MakeDoubleAccessor(&AquaSimAloha::m_persistent),
			MakeDoubleChecker<double>())
		.AddAttribute("AckOn", "If acknowledgement is on",
			IntegerValue(1),
			MakeIntegerAccessor(&AquaSimAloha::m_AckOn),
			MakeIntegerChecker<int>())
		.AddAttribute("MinBackoff", "Minimum back off time",
			DoubleValue(0.0),
			MakeDoubleAccessor(&AquaSimAloha::m_minBackoff),
			MakeDoubleChecker<double>())
			.AddAttribute("MaxBackoff", "Maximum back off time",
			DoubleValue(1.5),
		    MakeDoubleAccessor(&AquaSimAloha::m_maxBackoff),
			MakeDoubleChecker<double>());
		return tid;
	}

	int64_t
	AquaSimAloha::AssignStreams(int64_t stream)
	{
		NS_LOG_FUNCTION(this << stream);
		m_rand->SetStream(stream);
		return 1;
	}

	void AquaSimAloha::DoBackoff()
	{
		NS_LOG_DEBUG("Aloha-" << AquaSimAddress::ConvertFrom(GetAddress())
				<< " DoBackoff");

		ALOHA_Status = BACKOFF;
		Time BackoffTime = Seconds(m_rand->GetValue(m_minBackoff, m_maxBackoff));
		sendCnt++;
		if (sendCnt >= MAXIMUMCOUNTER)
		{
			sendCnt = 0;
			if (!pktQue.empty())
			{
				NS_LOG_DEBUG("Aloha-" << AquaSimAddress::ConvertFrom(GetAddress())
						<< " backoff too much to drop a pkt");
				pktQue.front() = 0;
				pktQue.pop();
			}
		}

		Simulator::Schedule(BackoffTime, &AquaSimAloha::ProcessPassive, this);
	}

	void AquaSimAloha::ProcessPassive()
	{
		ALOHA_Status = PASSIVE;
		SendDataPkt();
	}


	/*===========================Send and Receive===========================*/

	bool AquaSimAloha::TxProcess(Ptr<Packet> pkt)
	{
		NS_LOG_FUNCTION(m_device->GetAddress() << pkt << Simulator::Now().GetSeconds());
		AquaSimTrailer ast;
		AlohaHeader alohaH;
		pkt->PeekTrailer(ast);

		alohaH.setPktType(AlohaHeader::DATA);
		alohaH.SetSA(AquaSimAddress::ConvertFrom(m_device->GetAddress()));
		alohaH.SetDA(ast.GetNextHop());

		pkt->AddHeader(alohaH);
		
		pktQue.push(pkt);
		SendDataPkt();

		return true;
	}

	void AquaSimAloha::SendDataPkt()
	{
		NS_LOG_FUNCTION(this);
		if(ALOHA_Status != PASSIVE || pktQue.empty())
			return;

		double P = m_rand->GetValue(0, 1);
		if (P <= m_persistent){
			ALOHA_Status = SEND;
			SendPkt(pktQue.front()->Copy());
		}
		else{
			sendCnt--;
			DoBackoff();
		}

		return;
	}	

	void AquaSimAloha::addPktSeq(Ptr<Packet> pkt){
		AlohaHeader alohaH;
		pkt->RemoveHeader(alohaH);

		AquaSimAddress destAddress = alohaH.GetDA();
		std::map<AquaSimAddress, uint8_t>::iterator it;
		it =  nextSendPktSeq.find(destAddress);
		if(it == nextSendPktSeq.end()){
			alohaH.setPktSeq(0);
			nextSendPktSeq[destAddress] = 0;
		}
		else{
			alohaH.setPktSeq(nextSendPktSeq[destAddress]);
			//nextSendPktSeq[destAddress] = 1 - nextSendPktSeq[destAddress];
		}


		pkt->AddHeader(alohaH);
	}

	void AquaSimAloha::SendPkt(Ptr<Packet> pkt)
	{
		NS_LOG_FUNCTION(this);
		AlohaHeader alohaH;
		pkt->PeekHeader(alohaH);

		Time txTime = GetTxTime(pkt);
		Time rtt = txTime + GetTxTime(getHeaderSize(), 1) 
						+ 2 * Seconds(m_maxPropDelay) + Seconds(guardTime);


		switch (m_device->GetTransmissionStatus())
		{
		case SLEEP:
			PowerOn();

		case NIDLE:
		{

			if (alohaH.getPktType() == AlohaHeader::DATA)
			{
				if ((alohaH.GetDA() != AquaSimAddress::GetBroadcast()) && m_AckOn)
				{
					addPktSeq(pkt);
					ALOHA_Status = WAIT_ACK;
					waitAckTimer = Simulator::Schedule(rtt, &AquaSimAloha::DoBackoff, this);
				}
				else
				{
					if (!pktQue.empty()){
						pktQue.front() = 0;
						pktQue.pop();
					}
					sendCnt = 0;
					Simulator::Schedule(txTime, &AquaSimAloha::ProcessPassive, this);
				}

			}
			
			pkt->PeekHeader(alohaH);
			NS_LOG_DEBUG("Aloha-" <<AquaSimAddress::ConvertFrom(m_device->GetAddress()) 
			<< " SendDown " << AlohaHeader::pktTypeName[alohaH.getPktType()]
			<< " pkt id is " << (int)alohaH.getPktSeq()
			<< " dest: " << alohaH.GetDA() << " "<< Simulator::Now().GetSeconds());

			SendDown(pkt);

			break;
		}
		default:
			NS_LOG_DEBUG("Aloha-" << AquaSimAddress::ConvertFrom(m_device->GetAddress())
					<< " TX/(TX|RX) collison");
			pkt = 0;
			if (alohaH.getPktType() == AlohaHeader::DATA){
				DoBackoff();
			}
		}
	}

	bool AquaSimAloha::validDataPktSeq(uint8_t pktSeq, AquaSimAddress srcAddress){
		std::map<AquaSimAddress, uint8_t>::iterator it;
		it =  nextRecvPktSeq.find(srcAddress);

		if(it == nextRecvPktSeq.end()){
			nextRecvPktSeq[srcAddress] = 1 - pktSeq;
			return true;
		}
		else{
			uint8_t nextPktSeq = nextRecvPktSeq[srcAddress];
			nextRecvPktSeq[srcAddress] = (pktSeq == nextPktSeq)? 1 - nextPktSeq : nextPktSeq;
			return (pktSeq == nextPktSeq);
		}
	}

	bool AquaSimAloha::validAckPktSeq(uint8_t pktSeq, AquaSimAddress srcAddress){
		std::map<AquaSimAddress, uint8_t>::iterator it;
		it =  nextSendPktSeq.find(srcAddress);

		if(it == nextSendPktSeq.end()){
			NS_LOG_WARN("Aloha-recv a ack but no sending data!");
			return false;
		}
		else if(pktSeq == nextSendPktSeq[srcAddress]){
			nextSendPktSeq[srcAddress] = 1 - nextSendPktSeq[srcAddress];
			return true;
		}
		else{
			return false;
		}
	}

	bool AquaSimAloha::RecvProcess(Ptr<Packet> pkt)
	{
		NS_LOG_FUNCTION(m_device->GetAddress());
		AlohaHeader alohaH;
		pkt->RemoveHeader(alohaH);

		NS_LOG_DEBUG("Aloha-" << AquaSimAddress::ConvertFrom(m_device->GetAddress()) 
			<< " recv " << AlohaHeader::pktTypeName[alohaH.getPktType()]
			<< " pkt id is " << (int)alohaH.getPktSeq()<< " dest: " << alohaH.GetDA()
			<< " at "<< Simulator::Now().GetSeconds());


		AquaSimAddress srcAddr = alohaH.GetSA();
		AquaSimAddress recver = alohaH.GetDA();
		AquaSimAddress myAddr = AquaSimAddress::ConvertFrom(m_device->GetAddress());

		if(recver != myAddr && recver != AquaSimAddress::GetBroadcast()){
			pkt = 0;
			return true;
		}
		if(recver == AquaSimAddress::GetBroadcast()){
			if(alohaH.getPktType() == AlohaHeader::DATA){
				SendUp(pkt->Copy());
				pkt = 0;
			}
			return true;
		}
		if (alohaH.getPktType() == AlohaHeader::ACK)
		{
			bool flag = validAckPktSeq(alohaH.getPktSeq(), srcAddr);
			if(flag){
				waitAckTimer.Cancel();
				sendCnt = 0;
				if (!pktQue.empty())
				{
					pktQue.front() = 0;
					pktQue.pop();
				}
				ProcessPassive();
			}
		}
		else if (alohaH.getPktType() == AlohaHeader::DATA)
		{
			bool sendUpFlag = true;
			if (m_AckOn){
				sendUpFlag = validDataPktSeq(alohaH.getPktSeq(), srcAddr);
				ReplyACK(srcAddr, alohaH.getPktSeq());
			}
			if(sendUpFlag)
				Simulator::Schedule(GetTxTime(getHeaderSize() , 1) + Seconds(guardTime), 
						&AquaSimAloha::SendUp, this, pkt->Copy());
		}

		pkt = 0;
		return true;
	}

	void AquaSimAloha::ReplyACK(AquaSimAddress dataSender, int ackSeq) //sendACK
	{
		NS_LOG_FUNCTION(this);
		SendPkt(MakeACK(dataSender, ackSeq));
	}

	Ptr<Packet> AquaSimAloha::MakeACK(AquaSimAddress dataSender, int ackSeq)
	{
		NS_LOG_FUNCTION(this);
		Ptr<Packet> pkt = Create<Packet>();
		AquaSimTrailer ast;
		AlohaHeader alohaH;

		ast.SetModeId(1);

		alohaH.setPktType(AlohaHeader::ACK);
		alohaH.SetSA(AquaSimAddress::ConvertFrom(m_device->GetAddress()));
		alohaH.SetDA(dataSender);
		alohaH.setPktSeq(ackSeq);

		pkt->AddHeader(alohaH);
		pkt->AddTrailer(ast);
		return pkt;
		
	}



	uint32_t AquaSimAloha::getHeaderSize(){
		AlohaHeader alohaHeader;
		return alohaHeader.GetSerializedSize();
	}

	void AquaSimAloha::DoDispose()
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
