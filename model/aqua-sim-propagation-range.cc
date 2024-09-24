/*
 * aqua-sim-propagation-range.cc
 *
 *  Created on: Sep 4, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#include "aqua-sim-propagation-range.h"
#include "aqua-sim-header.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/double.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AquaSimRangePropagation");
NS_OBJECT_ENSURE_REGISTERED(AquaSimRangePropagation);

AquaSimRangePropagation::AquaSimRangePropagation()
{
}

TypeId
AquaSimRangePropagation::GetTypeId()
{
	static TypeId tid = TypeId("ns3::AquaSimRangePropagation")
							.SetParent<AquaSimPropagation>()
							.AddConstructor<AquaSimRangePropagation>()
							.AddAttribute("Temperature", "Temperature of water (C).",
										  DoubleValue(25),
										  MakeDoubleAccessor(&AquaSimRangePropagation::m_temp),
										  MakeDoubleChecker<double>())
							.AddAttribute("Salinty", "Salinty of water (ppt).",
										  DoubleValue(35),
										  MakeDoubleAccessor(&AquaSimRangePropagation::m_salinity),
										  MakeDoubleChecker<double>());
	return tid;
}

void AquaSimRangePropagation::Initialize()
{
	m_temp = 25;
	m_salinity = 35;
}




std::vector<PktRecvUnit> *
AquaSimRangePropagation::ReceivedCopies(Ptr<AquaSimNetDevice> s,
										Ptr<Packet> p,
										std::vector<Ptr<AquaSimNetDevice>> dList)
{
	NS_LOG_FUNCTION(this << dList.size());
	NS_ASSERT(dList.size());

	std::vector<PktRecvUnit> *res = new std::vector<PktRecvUnit>;
	PktRecvUnit pru;
	double dist = 0;

	AquaSimPacketStamp pstamp;
	p->PeekHeader(pstamp);

	Ptr<Object> sObject = s->GetNode();
	Ptr<MobilityModel> senderModel = sObject->GetObject<MobilityModel>();

	unsigned i = 0;
	std::vector<Ptr<AquaSimNetDevice>>::iterator it = dList.begin();
	for (; it != dList.end(); it++, i++)
	{
		Ptr<Object> rObject = dList[i]->GetNode();
		Ptr<MobilityModel> recvModel = rObject->GetObject<MobilityModel>();
		if ((dist = senderModel->GetDistanceFrom(recvModel)) > pstamp.GetTxRange())
			continue;

		pru.recver = dList[i];
		pru.pR = pstamp.GetPt();
		pru.pDelay = Time::FromDouble(dist / ns3::SOUND_SPEED_IN_WATER, Time::S);
		res->push_back(pru);

		NS_LOG_DEBUG("AquaSimRangePropagation::ReceivedCopies: Sender("
					 << s->GetAddress() << ") Recv(" << (pru.recver)->GetAddress()
					 << ") dist(" << dist << ") pDelay(" << pru.pDelay.GetMilliSeconds()
					 << ") pR(" << pru.pR << ")"
					 << " Pt(" << pstamp.GetPt() << ")" << senderModel->GetPosition() << " & " << recvModel->GetPosition());
	
	}
	return res;
}



/*
 * Gives the acoustic speed based on propagation conditions.
 * Model from Mackenzie, JASA, 1981.
 *
 *  input: node depth
 *  returns: m/s
 */
double
AquaSimRangePropagation::AcousticSpeed(double depth)
{
	double s = m_salinity - 35;
	double d = depth / 2;

	return (1448.96 + 4.591 * m_temp - 0.05304 * pow(m_temp, 2) +
			0.0002374 * pow(m_temp, 3) + 1.34 * s + 0.0163 * d +
			0.0000001675 * pow(d, 2) - 0.01025 * m_temp * s -
			0.0000000000007139 * m_temp * pow(d, 3));
}

void AquaSimRangePropagation::SetTemp(double temp)
{
	m_temp = temp;
}

void AquaSimRangePropagation::SetSalinity(double salinity)
{
	m_salinity = salinity;
}


void AquaSimRangePropagation::SetTraceValues(double temp, double salinity)
{
	m_temp = temp;
	m_salinity = salinity;
}
