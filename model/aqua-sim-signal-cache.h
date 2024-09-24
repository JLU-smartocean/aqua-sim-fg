/*
 * aqua-sim-signal-cache.h
 *
 *  Created on: Sep 5, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#ifndef AQUA_SIM_SIGNAL_CACHE_H
#define AQUA_SIM_SIGNAL_CACHE_H

#include <queue>

#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/timer.h"
#include "ns3/object.h"
#include "aqua-sim-noise-generator.h"
#include "aqua-sim-header.h"

//Aqua Sim Signal Cache

namespace ns3 {

	class AquaSimPhy;
	


struct IncomingPacket: Object {
	Ptr<Packet> packet;
	AquaSimPacketStamp::PacketStatus status;
	Ptr<IncomingPacket> next;
	double interference;
	double singalPower;
	double bandWidth;
	uint8_t bandId;

	IncomingPacket() :
			packet(NULL), status(AquaSimPacketStamp::INVALID), next(NULL), 
			interference(0), bandId(0), bandWidth(0){
	}

	IncomingPacket(Ptr<Packet> p, AquaSimPacketStamp::PacketStatus s, uint8_t b, double bw) :
			packet(p), status(s), next(NULL), interference(0), bandId(b), bandWidth(bw){
	}
};

class AquaSimSignalCache;








/**
 * \brief Signal Cache class which simulates the way that modems handle signals without considering multi-path effect
 */




class AquaSimSignalCache: public Object {
public:
	AquaSimSignalCache(void);
	virtual ~AquaSimSignalCache(void);
	static TypeId GetTypeId(void);

	virtual void SubmitPkt(Ptr<IncomingPacket> inPkt);

	void AddNewPacket(Ptr<Packet>);	
	bool DeleteIncomingPacket(Ptr<IncomingPacket> inPkt);


	void SetNoiseGen(Ptr<AquaSimNoiseGen> noise);
	double GetNoise();


protected:
	virtual void UpdatePacketStatus(Ptr<IncomingPacket> inPkt) = 0;
	void DoDispose();

public:

protected:
	Ptr<AquaSimPhy> m_phy;
	Ptr<AquaSimNoiseGen> m_noise;

	std::map<uint8_t, Ptr<IncomingPacket>> heads;
	std::map<uint8_t, int> pktNums;
	std::map<uint8_t, double> interferences;
	std::vector<double> bandWidthRatio;

private:
	void AttachPhy(Ptr<AquaSimPhy> phy);
	friend class AquaSimPhy;
};








class AquaSimSignalCacheSINR: public AquaSimSignalCache {
public:
	AquaSimSignalCacheSINR(void);
	virtual ~AquaSimSignalCacheSINR(void);
	static TypeId GetTypeId(void);

protected:
	virtual void UpdatePacketStatus(Ptr<IncomingPacket> inPkt);

	void DoDispose();
};
//class AquaSimSignalCacheSINR









class AquaSimSignalCacheRange: public AquaSimSignalCache {
public:

	AquaSimSignalCacheRange();
	virtual ~AquaSimSignalCacheRange();
	static TypeId GetTypeId(void);

protected:
	virtual void UpdatePacketStatus(Ptr<IncomingPacket> inPkt);
	void DoDispose();


};
//class AquaSimSignalCacheRange


}//namespace ns3

#endif /* AQUA_SIM_SIGNAL_CACHE_H */
