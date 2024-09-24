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

#include "ns3/nstime.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "aqua-sim-phy.h"
#include "aqua-sim-net-device.h"
#include "aqua-sim-signal-cache.h"
#include "aqua-sim-mac.h"
#include "aqua-sim-header-mac.h"
#include "aqua-sim-trailer.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AquaSimPhy");
NS_OBJECT_ENSURE_REGISTERED(AquaSimPhy);

TypeId
AquaSimPhy::GetTypeId()
{
	static TypeId tid = 
		TypeId("ns3::AquaSimPhy")
		.SetParent<Object>()
		.AddAttribute("txPower", "Power consumption for transmission (W). Default is 0.660 (1.6W).",
			DoubleValue(1.3), MakeDoubleAccessor(&AquaSimPhy::txPower),
			MakeDoubleChecker<double>())
		.AddAttribute("rxPower", "Power consumption for reception (W). Default is 0.395 (1.2W).",
			DoubleValue(0.395), MakeDoubleAccessor(&AquaSimPhy::rxPower),
			MakeDoubleChecker<double>())
		.AddAttribute("idlePower", "Idle power consumption (W). Default is 0.0 (0W).",
			DoubleValue(0.0), MakeDoubleAccessor(&AquaSimPhy::idlePower),
			MakeDoubleChecker<double>())
		.AddAttribute("turnOnEnergy", "Energy consumption for turning on the modem (J).",
			DoubleValue(0), MakeDoubleAccessor(&AquaSimPhy::turnOnEnergy),
			MakeDoubleChecker<double>())
		.AddAttribute("turnOffEnergy", "Energy consumption for turning off the modem (J).",
			DoubleValue(0), MakeDoubleAccessor(&AquaSimPhy::turnOffEnergy),
			MakeDoubleChecker<double>())
		.AddAttribute("transRange", "transRange(m), default is 1000.", 
			DoubleValue(1000), MakeDoubleAccessor(&AquaSimPhy::transRange), 
			MakeDoubleChecker<double>())
		.AddAttribute("RXThresh", "Receive power threshold (W), default is 3.652e-10 set as 0.", 
			DoubleValue(0), MakeDoubleAccessor(&AquaSimPhy::RXThresh), 
			MakeDoubleChecker<double>())
		.AddAttribute("SignalCache", "Signal cache attached to this node.", 
			PointerValue(), MakePointerAccessor(&AquaSimPhy::m_sC), 
			MakePointerChecker<AquaSimSignalCache>())
		.AddTraceSource("Rx", "A packet was receieved.", 
			MakeTraceSourceAccessor(&AquaSimPhy::m_rxLogger), "ns3::AquaSimPhy::TracedCallback")
		.AddTraceSource("Tx", "A packet was transmitted.", 
			MakeTraceSourceAccessor(&AquaSimPhy::m_txLogger), "ns3::AquaSimPhy::TracedCallback");
	return tid;
}

AquaSimPhy::AquaSimPhy()
{
	powerOn = true;
	turnOnEnergy = 0;
	turnOffEnergy = 0;
	lastUpdateEnergyTime = Simulator::Now().GetSeconds();
	txPower = 1.1;
	rxPower = 0.395;
	idlePower = 0.0;

	RXThresh = 0;
	transRange = 1000;

	dropPktRand = CreateObject<UniformRandomVariable>();
	Simulator::Schedule(Seconds(1), &AquaSimPhy::UpdateIdleEnergy, this); 
}

std::vector<std::string> AquaSimPhy::statusName = {
	"SLEEP", "NIDLE", "SEND", "RECV", "NSTATUS", "DISABLE"
};



void AquaSimPhy::SetTxPower(double txPower)
{
	this->txPower = txPower;
	EM()->SetTxPower(txPower);
}

void AquaSimPhy::SetRxPower(double rxPower)
{
	this->rxPower = rxPower;
	EM()->SetRxPower(rxPower);
}

void AquaSimPhy::SetIdlePower(double idlePower)
{
	this->idlePower = idlePower;
	EM()->SetIdlePower(idlePower);
}

bool AquaSimPhy::IsPoweredOn()
{
	return powerOn;
}


void AquaSimPhy::PowerOn()
{
	NS_LOG_FUNCTION(this);

	if (GetNetDevice()->GetTransmissionStatus() == DISABLE)
		NS_LOG_FUNCTION(
			this << " Node " << GetNetDevice()->GetNode() << " is disabled.");
	else
	{
		powerOn = true;
		GetNetDevice()->SetTransmissionStatus(NIDLE);
		// SetPhyStatus(PHY_IDLE);
		if (EM() != NULL)
		{
			// minus the energy consumed by power on
			EM()->SetEnergy(std::max(0.0, EM()->GetEnergy() - turnOnEnergy));
			lastUpdateEnergyTime = std::max(Simulator::Now().GetSeconds(),
										  lastUpdateEnergyTime);
		}
	}
}

void AquaSimPhy::PowerOff()
{
	NS_LOG_FUNCTION(this);

	if (GetNetDevice()->GetTransmissionStatus() == DISABLE)
		NS_LOG_FUNCTION(
			this << " Node " << GetNetDevice()->GetNode() << " is disabled.");
	else
	{
		powerOn = false;
		GetNetDevice()->SetTransmissionStatus(SLEEP);
		// SetPhyStatus(PHY_SLEEP);
		if (EM() == NULL)
			return;

		// minus the energy consumed by power off
		EM()->SetEnergy(std::max(0.0, EM()->GetEnergy() - turnOffEnergy));

		if (Simulator::Now().GetSeconds() > lastUpdateEnergyTime)
		{
			EM()->DecrIdleEnergy(
				Simulator::Now().GetSeconds() - lastUpdateEnergyTime,
				idlePower);
			lastUpdateEnergyTime = Simulator::Now().GetSeconds();
		}
	}
}



/**
 * update energy for transmitting for duration of P_t
 */
void AquaSimPhy::UpdateTxEnergy(Time txTime, double pT)
{
	NS_LOG_FUNCTION(this << "Currently not implemented");
	double startTime = Simulator::Now().GetSeconds();
	double endTime = Simulator::Now().GetSeconds() + txTime.GetSeconds();

	if (EM() == NULL)
	{
		NS_LOG_FUNCTION(this << " No EnergyModel set.");
		return;
	}

	if (startTime >= lastUpdateEnergyTime)
		EM()->DecrIdleEnergy(startTime - lastUpdateEnergyTime, idlePower);
	EM()->DecrTxEnergy(txTime.GetSeconds(), pT);
	if(endTime > lastUpdateEnergyTime)
		lastUpdateEnergyTime = endTime;
}

void AquaSimPhy::UpdateRxEnergy(Time txTime)
{
	NS_LOG_FUNCTION(txTime);

	double startTime = Simulator::Now().GetSeconds();
	double endTime = startTime + txTime.GetSeconds();

	if (EM() == NULL)
	{
		NS_LOG_FUNCTION(this << " No EnergyModel set.");
		return;
	}

	if (startTime > lastUpdateEnergyTime)
		EM()->DecrIdleEnergy(startTime - lastUpdateEnergyTime, idlePower);
	EM()->DecrRcvEnergy(txTime.GetSeconds(), rxPower);
	if(endTime > lastUpdateEnergyTime)
		lastUpdateEnergyTime = endTime;
}

void AquaSimPhy::UpdateIdleEnergy()
{
	if (!powerOn || EM() == NULL)
		return;

	if (Simulator::Now().GetSeconds() > lastUpdateEnergyTime && powerOn)
	{
		EM()->DecrIdleEnergy(Simulator::Now().GetSeconds() - lastUpdateEnergyTime,
							 idlePower);
		lastUpdateEnergyTime = Simulator::Now().GetSeconds();
	}

	// log device energy
	if (EM()->GetEnergy() > 0)
	{
		// GetNetDevice()->LogEnergy(1);
		// NS_LOG_INFO("AquaSimPhyCmn::UpdateRxEnergy: -t " << Simulator::Now().GetSeconds() <<
		//" -n " << GetNetDevice()->GetAddress() << " -e " << EM()->GetEnergy());
	}
	else
	{
		// GetNetDevice()->LogEnergy(0);
		// NS_LOG_INFO("AquaSimPhyCmn::UpdateRxEnergy: -t " << Simulator::Now().GetSeconds() <<
		//" -n " << GetNetDevice()->GetAddress() << " -e 0");
	}

	Simulator::Schedule(Seconds(1), &AquaSimPhy::UpdateIdleEnergy, this);
}

void AquaSimPhy::EnergyDeplete()
{
	// TODO fix energy model and then allow this   SetPhyStatus(PHY_DISABLE);
}


bool AquaSimPhy::MatchFreq(double freq1, double freq2)
{
	double epsilon = 1e-6; // accuracy for float comparison
	return std::fabs(freq1 - freq2) < epsilon;
}

bool AquaSimPhy::MatchBand(double band1, double band2)
{
	double epsilon = 1e-6; // accuracy for float comparison
	return std::fabs(band1 - band2) < epsilon;
}

void AquaSimPhy::StatusShift(double txTime)
{
	double endTime = Simulator::Now().GetSeconds() + txTime;
	if (lastUpdateEnergyTime < endTime)
	{
		double overlapTime = lastUpdateEnergyTime - Simulator::Now().GetSeconds();
		double actualTxTime = endTime - lastUpdateEnergyTime;
		EM()->DecrEnergy(overlapTime, txPower - rxPower);
		EM()->DecrTxEnergy(actualTxTime, txPower);
		lastUpdateEnergyTime = endTime;
	}
	else
	{
		double overlapTime = txTime;
		EM()->DecrEnergy(overlapTime, txPower - rxPower);
	}
}

/**
 * pass packet p to channel
 */
bool AquaSimPhy::PktTransmit(Ptr<Packet> p, int channelId)
{
	NS_LOG_FUNCTION(this << p);

	if (GetNetDevice()->FailureStatus())
	{
		NS_LOG_WARN(
			"AquaSimPhyCmn nodeId=" << GetNetDevice()->GetNode()->GetId() << " fails!\n");
		p = 0;
		return false;
	}

	if (GetNetDevice()->GetTransmissionStatus() == SLEEP || (NULL != EM() && EM()->GetEnergy() <= 0))
	{
		NS_LOG_DEBUG("Unable to reach phy layer (sleep/disable)");
		p = 0;
		return false;
	}

	Time txTime = this->calcTxTime(p);
	UpdateTxEnergy(txTime, txPower);

	StampTxInfo(p);

	AquaSimTrailer ast;
	p->PeekTrailer(ast);

	Simulator::Schedule(txTime, &AquaSimPhy::setStatus, this, NIDLE, ast.GetBandId());

	NotifyTx(p);
	m_txLogger(p, m_sC->GetNoise());

	return m_channel.at(channelId)->Recv(p, this);
}

void AquaSimPhy::setStatus(TransStatus status, uint8_t bandId){
	GetNetDevice()->SetTransmissionStatus(status);
}

/**
 * send packet to upper layer, supposed to be MAC layer,
 * but actually go to any specificed module.
 */
void AquaSimPhy::SendPktUp(Ptr<Packet> p)
{

	NS_LOG_FUNCTION(this);
	MacHeader mach;
	p->PeekHeader(mach);

	NotifyRx(p);
	m_rxLogger(p, m_sC->GetNoise());

	// This can be shifted to within the switch to target specific packet types.
	if (GetNetDevice()->IsAttacker())
	{
		GetNetDevice()->GetAttackModel()->Recv(p);
		return;
	}

	switch (mach.GetDemuxPType())
	{
	case MacHeader::UWPTYPE_OTHER:
		if (!GetMac()->RecvProcess(p))
			NS_LOG_DEBUG(this << "Mac Recv error");
		break;
	case MacHeader::UWPTYPE_LOC:
		std::cout << GetNetDevice()->GetNode() << " Phy-cmn Send up"
				  << std::endl;
		GetNetDevice()->GetMacLoc()->Recv(p);
		break;
	case MacHeader::UWPTYPE_SYNC:
		GetNetDevice()->GetMacSync()->RecvSync(p);
		break;
	case MacHeader::UWPTYPE_SYNC_BEACON:
		GetNetDevice()->GetMacSync()->RecvSyncBeacon(p);
		break;
	case MacHeader::UWPTYPE_NDN:
		GetNetDevice()->GetNamedData()->Recv(p);
		break;
	default:
		NS_LOG_DEBUG("SendPKtUp: Something went wrong.");
	}
}

Ptr<Packet> AquaSimPhy::PrevalidateIncomingPkt(Ptr<Packet> p)
{
	NS_LOG_FUNCTION(this << p);

	if (!matchPkt(p))
	{
		p = 0;
		return NULL;
	}

	AquaSimPacketStamp pstamp;
	MacHeader mach;
	p->RemoveHeader(pstamp);
	p->PeekHeader(mach);
	p->AddHeader(pstamp);

	if (GetNetDevice()->FailureStatus())
	{
		NS_LOG_WARN(
			"AquaSimPhyCmn: nodeId=" << GetNetDevice()->GetNode()->GetId() << " fails!\n");
		p = 0;
		return NULL;
	}


	if ((EM() && EM()->GetEnergy() <= 0) ||
		GetNetDevice()->GetTransmissionStatus() == SLEEP || pstamp.GetPr() < RXThresh)
	{

		NS_LOG_DEBUG("PHY-EM/DEV/PR problem");
		p = 0;
		return NULL;
	}

	checkPhyStatus(p);


	if (mach.GetDemuxPType() == MacHeader::UWPTYPE_LOC)
	{
		GetNetDevice()->GetMacLoc()->SetPr(pstamp.GetPr());
	}

	return p;
}

void AquaSimPhy::checkPhyStatus(Ptr<Packet> p){
	AquaSimPacketStamp pstamp;
	p->RemoveHeader(pstamp);
	pstamp.SetPacketStatus(AquaSimPacketStamp::RECEPTION);
	if (GetNetDevice()->GetTransmissionStatus() == SEND)
	{
		NS_LOG_INFO("PHY-" << aquasimAddress() << " status is SEND,recv false");
		pstamp.SetPacketStatus(AquaSimPacketStamp::COLLISION);
	}
	else if (GetNetDevice()->GetTransmissionStatus() == RECV)
	{
		NS_LOG_INFO("PHY-" << aquasimAddress() << " status is RECV,recv false");
		pstamp.SetPacketStatus(AquaSimPacketStamp::COLLISION);
	}
	else
	{
		Time txTime = calcTxTime(p);
		setStatus(RECV, 0);
		Simulator::Schedule(txTime, &AquaSimPhy::setStatus, this, NIDLE, 0);
		UpdateRxEnergy(txTime);
	}
	p->AddHeader(pstamp);
}


void AquaSimPhy::SignalCacheCallback(Ptr<Packet> p)
{
	NS_LOG_FUNCTION(this << p);
	AquaSimTrailer ast;
	p->PeekTrailer(ast);
	int m = 1 << ast.GetModeId();
	double sinr = ast.GetSinr();
	double s = sqrt(m);
	double ls = log2(s);
	double l = ast.GetModeId();
	double ber;

	if(sinr < 0){
		ber = 1;
	}
	else{
		ber = (s-1)*erfc(sqrt(3*sinr*l/2/(m-1)))/s/ls
				+(s-2)*erfc(3*sqrt(3*sinr*l/2/(m-1)))/s/ls;
	}

	double per = 1 - pow(1-ber, p->GetSize() - ast.GetSerializedSize());
	double r = dropPktRand->GetValue(0, 1);
	
	NS_LOG_INFO("PHY-" << aquasimAddress() << "SINR:" << sinr << "BER:" 
					<< ber << " PER:" << per << " rand num is" << r);

	if(ber > 1e-4){
		NS_LOG_INFO("PHY-drop a pkt");
		p = 0;
		return;
	}

	SendPktUp(p);
}



bool AquaSimPhy::txProcess(Ptr<Packet> p)
{
	if(m_device->GetTransmissionStatus() != NIDLE)
		return false;
	m_device->SetTransmissionStatus(SEND);
	PktTransmit(p);
	return true;
}

bool AquaSimPhy::recvProcess(Ptr<Packet> p)
{	
	p = PrevalidateIncomingPkt(p);
	if (p != NULL)
	{
		m_sC->AddNewPacket(p);
	}
	return true;
}

AquaSimAddress AquaSimPhy::aquasimAddress(){
	return AquaSimAddress::ConvertFrom(m_device->GetAddress());
}

void AquaSimPhy::DoDispose()
{
	NS_LOG_FUNCTION(this);
	m_device = 0;
	m_sC->Dispose();
	m_sC = 0;
	m_sinrChecker = 0;
	for (std::vector<Ptr<AquaSimChannel>>::iterator it = m_channel.begin(); it != m_channel.end(); ++it)
		*it = 0;
}

Ptr<AquaSimNetDevice>
AquaSimPhy::GetNetDevice()
{
	return m_device;
}

Ptr<AquaSimMac>
AquaSimPhy::GetMac()
{
	return m_device->GetMac();
}

Ptr<AquaSimEnergyModel>
AquaSimPhy::EM()
{
	return m_device->EnergyModel();
}

void AquaSimPhy::SetNetDevice(Ptr<AquaSimNetDevice> device)
{
	NS_LOG_FUNCTION(this);
	m_device = device;
}

void AquaSimPhy::SetChannel(std::vector<Ptr<AquaSimChannel>> channel)
{
	NS_LOG_FUNCTION(this);
	m_channel = channel;
}

void AquaSimPhy::SetSinrChecker(Ptr<AquaSimSinrChecker> sinrChecker)
{
	m_sinrChecker = sinrChecker;
}

void AquaSimPhy::SetSignalCache(Ptr<AquaSimSignalCache> sC)
{
	m_sC = sC;
	sC->AttachPhy(this);
}

void AquaSimPhy::NotifyTx(Ptr<Packet> packet)
{
	m_phyTxTrace(packet);
}

void AquaSimPhy::NotifyRx(Ptr<Packet> packet)
{
	m_phyRxTrace(packet);
}
