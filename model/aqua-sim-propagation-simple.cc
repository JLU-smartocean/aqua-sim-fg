/*
 * aqua-sim-propagation-simple.cc
 *
 *  Created on: Sep 4, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */



#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"
#include "ns3/random-variable-stream.h"
#include "ns3/packet.h"

#include "aqua-sim-propagation-simple.h"
#include "aqua-sim-header.h"

#include <stdio.h>
#include <vector>
#include <math.h>

#define _USE_MATH_DEFINES

namespace ns3
{

	NS_LOG_COMPONENT_DEFINE("AquaSimSimplePropagation");
	NS_OBJECT_ENSURE_REGISTERED(AquaSimSimplePropagation);

	TypeId
	AquaSimSimplePropagation::GetTypeId()
	{
		static TypeId tid = TypeId("ns3::AquaSimSimplePropagation")
								.SetParent<AquaSimPropagation>()
								.AddConstructor<AquaSimSimplePropagation>();
		return tid;
	}

	AquaSimSimplePropagation::AquaSimSimplePropagation() : AquaSimPropagation()
	{
	}

	AquaSimSimplePropagation::~AquaSimSimplePropagation()
	{
	}

	std::vector<PktRecvUnit> *
	AquaSimSimplePropagation::ReceivedCopies(Ptr<AquaSimNetDevice> s,
											 Ptr<Packet> p,
											 std::vector<Ptr<AquaSimNetDevice>> dList)
	{
		NS_LOG_FUNCTION(this);
		NS_ASSERT(dList.size());

		std::vector<PktRecvUnit> *res = new std::vector<PktRecvUnit>;
		//find all nodes which will receive a copy
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

			dist = senderModel->GetDistanceFrom(recvModel);
			pru.recver = dList[i];
			pru.pDelay = Time::FromDouble(dist / ns3::SOUND_SPEED_IN_WATER, Time::S);
			pru.pR = getRecvPower(dist, pstamp.GetFreq(), pstamp.GetPt());
			res->push_back(pru);
			NS_LOG_DEBUG("PROSIM-dist:" << dist
								<< " recver:" << pru.recver->GetAddress()
								<< " pDelay:" << pru.pDelay.GetMilliSeconds()
								<< " pR:" << pru.pR
								<< " Pt:" << pstamp.GetPt());
		}
		return res;
	}


	double
	AquaSimSimplePropagation::getRecvPower(double dist, double freq, double Pt)
	{
		double SL = Pt - ThorpDB(dist, freq);
		return SL;
	}

} // namespace ns3
