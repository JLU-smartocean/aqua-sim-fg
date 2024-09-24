/*
 * aqua-sim-signal-cache.cc
 *
 *  Created on: Sep 5, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

// Mar 15 2022, add multi band support

//#include ...
#include "aqua-sim-signal-cache.h"
#include "aqua-sim-header.h"
#include "aqua-sim-trailer.h"
#include "aqua-sim-header-mac.h"

#include <queue>

#include "aqua-sim-phy.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

// Aqua Sim Signal Cache

namespace ns3
{

	NS_LOG_COMPONENT_DEFINE("AquaSimSignalCache");
	NS_OBJECT_ENSURE_REGISTERED(AquaSimSignalCache);

	AquaSimSignalCache::AquaSimSignalCache()
	{
		NS_LOG_FUNCTION(this);
		heads.clear();
		pktNums.clear();
		interferences.clear();

		interferences[255] = 0;
		pktNums[255] = 0;
		heads[255] = CreateObject<IncomingPacket>();
	}

	AquaSimSignalCache::~AquaSimSignalCache()
	{
	}

	TypeId AquaSimSignalCache::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::AquaSimSignalCache")
								.SetParent<Object>();
		return tid;
	}

	void AquaSimSignalCache::AddNewPacket(Ptr<Packet> p)
	{

		AquaSimPacketStamp pstamp;
		p->RemoveHeader(pstamp);
		uint8_t bandId = pstamp.GetBandId();
		uint8_t bandNum = pstamp.GetBandNum();
		double bandWidth = pstamp.GetBandWidth();
		double pr = pstamp.GetPr();
		uint8_t status = pstamp.GetPacketStatus();

		if (bandId == bandNum)
		{
			bandId = 255;
		}
		Time transmissionDelay = m_phy->calcTxTime(p);

		Ptr<IncomingPacket> inPkt = CreateObject<IncomingPacket>(p,
																 (AquaSimPacketStamp::PacketStatus)status, bandId, bandWidth);
		inPkt->singalPower = pr;
		inPkt->interference = pow(10, pr/10);

		Simulator::Schedule(transmissionDelay, &AquaSimSignalCache::SubmitPkt,
							this, inPkt);

		inPkt->next = heads[bandId]->next;
		heads[bandId]->next = inPkt;
		pktNums[bandId]++;
		interferences[bandId] += pow(10, pr / 10);

		UpdatePacketStatus(inPkt);
	}

	bool AquaSimSignalCache::DeleteIncomingPacket(Ptr<IncomingPacket> inPkt)
	{
		NS_LOG_FUNCTION(this);
		uint8_t bandId = inPkt->bandId;

		Ptr<IncomingPacket> pre = heads[bandId];
		Ptr<IncomingPacket> ptr = pre->next;

		while (ptr != NULL && ptr != inPkt)
		{
			ptr = ptr->next;
			pre = pre->next;
		}

		if (ptr != NULL)
		{
			pktNums[bandId]--;
			interferences[bandId] -= pow(10, inPkt->singalPower / 10);
			pre->next = ptr->next;
			ptr = 0;
			return true;
		}
		return false;
	}

	void AquaSimSignalCache::SubmitPkt(Ptr<IncomingPacket> inPkt)
	{
		NS_LOG_FUNCTION(this << inPkt << inPkt->status);

		AquaSimPacketStamp::PacketStatus status = inPkt->status;
		Ptr<Packet> p = inPkt->packet;
		double pr = inPkt->singalPower;
		double inter = inPkt->interference - pow(10,  pr/10);
		double noise = pow(10, m_noise->Noise()/10) * inPkt->bandWidth * 1000;
		double sinr = pr - 10 * log10(inter + noise);
		NS_LOG_INFO("pr(DB):" << pr << " noise+inter(DB):" << 10 * log10(inter + noise));
		DeleteIncomingPacket(inPkt);
		if (status != AquaSimPacketStamp::RECEPTION)
		{
			NS_LOG_DEBUG("Packet(" << p << ") dropped");
			p = 0;
			return;
		}

		AquaSimTrailer ast;
		p->RemoveTrailer(ast);
		ast.SetSinr(sinr);
		p->AddTrailer(ast);
		m_phy->SignalCacheCallback(p);
	}

	void AquaSimSignalCache::AttachPhy(Ptr<AquaSimPhy> phy)
	{
		m_phy = phy;
		std::vector<double> bandWidth = m_phy->getBandWidth();
		if (bandWidth.size() == 1)
		{
			bandWidthRatio.push_back(1);
			pktNums[0] = 0;
			heads[0] = CreateObject<IncomingPacket>();
			return;
		}
		for (size_t i = 0; i < bandWidth.size() - 1; i++)
		{
			bandWidthRatio.push_back(bandWidth[i] / bandWidth[bandWidth.size() - 1]);
			pktNums[i] = 0;
			heads[i] = CreateObject<IncomingPacket>();
			interferences[i] = 0;
		}
	}

	void AquaSimSignalCache::SetNoiseGen(Ptr<AquaSimNoiseGen> noise)
	{
		NS_LOG_FUNCTION(this);
		m_noise = noise;
	}

	double AquaSimSignalCache::GetNoise()
	{
		return m_noise->Noise();
	}

	void AquaSimSignalCache::DoDispose()
	{
		NS_LOG_FUNCTION(this);

		std::map<uint8_t, Ptr<IncomingPacket>>::iterator it = heads.begin();

		while (it != heads.end())
		{
			Ptr<IncomingPacket> head = it->second;
			Ptr<IncomingPacket> p;
			while (head != NULL)
			{
				p = head;
				head = head->next;

				p->packet = 0;
				p = 0;
			}
			++it;
		}
		m_phy = 0;
		m_noise = 0;
		Object::DoDispose();
	}

	NS_OBJECT_ENSURE_REGISTERED(AquaSimSignalCacheSINR);

	AquaSimSignalCacheSINR::AquaSimSignalCacheSINR()
		: AquaSimSignalCache()
	{
	}

	AquaSimSignalCacheSINR::~AquaSimSignalCacheSINR()
	{
	}

	TypeId AquaSimSignalCacheSINR::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::AquaSimSignalCacheSINR")
								.SetParent<AquaSimSignalCache>()
								.AddConstructor<AquaSimSignalCacheSINR>();
		return tid;
	}

	void AquaSimSignalCacheSINR::UpdatePacketStatus(Ptr<IncomingPacket> inPkt)
	{
		NS_LOG_FUNCTION(this);
		uint8_t bandId = inPkt->bandId;

		std::map<uint8_t, double>::iterator it1 = interferences.begin();
		double inter255 = 0;
		for (; it1 != interferences.end(); it1++)
		{
			inter255 += it1->second;
		}

		if (bandId == 255)
		{

			std::map<uint8_t, Ptr<IncomingPacket>>::iterator it = heads.begin();
			for (; it != heads.end(); it++)
			{
				double inter = 0;
				if (it->first == 255)
				{
					inter = inter255;
				}
				else
				{
					inter = interferences[it->first] + interferences[255] * bandWidthRatio[it->first];
				}
				for (Ptr<IncomingPacket> ptr = it->second->next; ptr != NULL; ptr = ptr->next)
				{
					if (inter > ptr->interference)
						ptr->interference = inter;
				}
			}
		}
		else
		{

			double inter = interferences[bandId] + interferences[255] * bandWidthRatio[bandId];
			for (Ptr<IncomingPacket> ptr = heads[bandId]->next; ptr != NULL; ptr = ptr->next)
			{
				if (inter > ptr->interference)
					ptr->interference = inter;
			}
			inter = interferences[bandId] + interferences[255];
			for (Ptr<IncomingPacket> ptr = heads[255]->next; ptr != NULL; ptr = ptr->next)
			{
				if (inter255 > ptr->interference)
					ptr->interference = inter;
			}
		}
	}

	void AquaSimSignalCacheSINR::DoDispose()
	{
		NS_LOG_FUNCTION(this);

		AquaSimSignalCache::DoDispose();
	}

	NS_OBJECT_ENSURE_REGISTERED(AquaSimSignalCacheRange);

	AquaSimSignalCacheRange::AquaSimSignalCacheRange()
		: AquaSimSignalCache()
	{
	}

	AquaSimSignalCacheRange::~AquaSimSignalCacheRange()
	{
	}

	TypeId AquaSimSignalCacheRange::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::AquaSimSignalCacheRange")
								.SetParent<AquaSimSignalCache>()
								.AddConstructor<AquaSimSignalCacheRange>();
		return tid;
	}

	void AquaSimSignalCacheRange::UpdatePacketStatus(Ptr<IncomingPacket> inPkt)
	{

		NS_LOG_FUNCTION(this);
		uint8_t bandId = inPkt->bandId;

		if (bandId == 255)
		{
			std::map<uint8_t, int>::iterator it = pktNums.begin();
			int sum = 0;
			for (; it != pktNums.end(); ++it)
			{
				sum += it->second;
			}
			if (sum == 1)
				return;

			std::map<uint8_t, Ptr<IncomingPacket>>::iterator it1 = heads.begin();
			for (; it1 != heads.end(); ++it1)
			{
				Ptr<IncomingPacket> ptr = it1->second->next;
				for (; ptr != NULL; ptr = ptr->next)
				{
					ptr->status = AquaSimPacketStamp::COLLISION;
					NS_LOG_DEBUG("SCRANGE-COLLSION");
				}
			}
		}
		else
		{
			if (pktNums[bandId] + pktNums[255] == 1)
			{
				return;
			}

			Ptr<IncomingPacket> ptr = heads[bandId]->next;

			for (; ptr != NULL; ptr = ptr->next)
			{
				ptr->status = AquaSimPacketStamp::COLLISION;
				NS_LOG_DEBUG("SCRANGE-COLLSION");
			}

			ptr = heads[255]->next;

			for (; ptr != NULL; ptr = ptr->next)
			{
				ptr->status = AquaSimPacketStamp::COLLISION;
				NS_LOG_DEBUG("SCRANGE-COLLSION");
			}
		}
	}

	void AquaSimSignalCacheRange::DoDispose()
	{
		NS_LOG_FUNCTION(this);

		AquaSimSignalCache::DoDispose();
	}

};
// namespace ns3
