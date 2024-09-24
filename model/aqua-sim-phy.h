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
#ifndef AQUA_SIM_PHY_H
#define AQUA_SIM_PHY_H

#include <string>

#include "ns3/object.h"
#include "ns3/traced-callback.h"
#include "aqua-sim-net-device.h"
#include "aqua-sim-channel.h"
#include "aqua-sim-sinr-checker.h"
#include "aqua-sim-signal-cache.h"

namespace ns3
{

	// Using net device as device status singleton
	/*enum PhyStatus {
	PHY_RECV,
	PHY_SEND,
	PHY_IDLE,
	PHY_SLEEP,
	PHY_TRIG,
	PHY_NONE,
	PHY_DISABLE
	};*/

	class AquaSimNetDevice;
	class AquaSimChannel;
	class AquaSimMac;
	class AquaSimEnergyModel;
	class AquaSimSinrChecker;
	class AquaSimSignalCache;
	class AquaSimModulation;
	class Packet;
	class Time;

	/**
	 * \ingroup aqua-sim-ng
	 *
	 * \brief Base class for Phy layer.
	 *
	 * Overall modem status can be found under AquaSimNetDevice
	 */
	class AquaSimPhy : public Object
	{
	public:
		AquaSimPhy();
		static TypeId GetTypeId();

		void SetTxPower(double txPower);
		void SetRxPower(double rxPower);
		void SetIdlePower(double idlePower);

		void SetNetDevice(Ptr<AquaSimNetDevice> device);
		void SetChannel(std::vector<Ptr<AquaSimChannel>> channel);
		void SetSinrChecker(Ptr<AquaSimSinrChecker> sinrChecker);
		void SetSignalCache(Ptr<AquaSimSignalCache> sC);


		Ptr<AquaSimNetDevice> GetNetDevice();
		Ptr<AquaSimMac> GetMac();
		Ptr<AquaSimEnergyModel> EM();

		virtual bool IsPoweredOn();
		virtual void addModulation(Ptr<AquaSimModulation> modulation, int modulationNum) = 0;

		virtual void SendPktUp(Ptr<Packet> p);
		virtual bool PktTransmit(Ptr<Packet> p, int channelId = 0);

		void PowerOn();
		void PowerOff();
		virtual void StatusShift(double x);

		virtual Time calcTxTime(Ptr<Packet> pkt) = 0;
		virtual Time calcTxTime(uint32_t pktSize) = 0;
		virtual Time calcTxTime(uint32_t pktSize, int modulationNum) = 0;
		virtual double CalcPktSize(double txtime, int modulationNum) = 0;

		virtual void SignalCacheCallback(Ptr<Packet> p);
		virtual bool txProcess(Ptr<Packet> p);
		virtual bool recvProcess(Ptr<Packet> p);
		virtual void setStatus(TransStatus status, uint8_t bandid);

		virtual Ptr<AquaSimSignalCache> GetSignalCache() { return m_sC; };
		virtual void SetTransRange(double range) {transRange = range; };
		virtual double GetTransRange() {return transRange; };
		virtual Ptr<Packet> StampTxInfo(Ptr<Packet> p) = 0;
		virtual std::vector<double> getBandWidth() = 0;
		virtual std::vector<double> getFreq() = 0;


		virtual int64_t AssignStreams(int64_t stream) = 0;
		typedef void (*TracedCallback)(Ptr<Packet> pkt, double noise);
		void NotifyTx(Ptr<Packet> packet);
		void NotifyRx(Ptr<Packet> packet);

	protected:
		virtual bool MatchFreq(double freq1, double freq2);
		virtual bool MatchBand(double band1, double band2);
		virtual bool matchPkt(Ptr<Packet> p) = 0;

		virtual Ptr<Packet> PrevalidateIncomingPkt(Ptr<Packet> p);
		virtual void checkPhyStatus(Ptr<Packet> p);


		virtual void UpdateTxEnergy(Time txTime, double pT);
		virtual void UpdateRxEnergy(Time txTime);
		virtual void UpdateIdleEnergy(void);
		virtual void EnergyDeplete();

		AquaSimAddress aquasimAddress();
		virtual void DoDispose();

		double RXThresh; // receive power threshold (W)
		double transRange; //max tramission range for modem. **NOTE: Only functional with range-propagation model**.


		bool powerOn;
		double txPower;		  // power consumption for transmission (W)
		double rxPower;		  // power consumption for reception (W)
		double idlePower;	  // idle power consumption (W)
		double turnOnEnergy;  // energy consumption for turning on the modem (J)
		double turnOffEnergy; // energy consumption for turning off the modem (J)
		double lastUpdateEnergyTime; // the last time we update energy.


		std::vector<Ptr<AquaSimChannel>> m_channel; // for multi-channel support
		Ptr<AquaSimNetDevice> m_device;
		Ptr<AquaSimSignalCache> m_sC;
		Ptr<AquaSimSinrChecker> m_sinrChecker;


		EventId statusEventId;
		Time statusEventTime;

		static std::vector<std::string> statusName;

		friend class AquaSimEnergyModel;
		friend class AquaSimNetDevice; 

	private:
		ns3::TracedCallback<Ptr<Packet>> m_phyTxTrace;
		ns3::TracedCallback<Ptr<Packet>> m_phyRxTrace;
		ns3::TracedCallback<Ptr<Packet>, double> m_rxLogger;
		ns3::TracedCallback<Ptr<Packet>, double> m_txLogger;
		Ptr<UniformRandomVariable> dropPktRand;
	}; // AquaSimPhy class

} // ns3 namespace

#endif /* AQUA_SIM_PHY_H */
