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

#ifndef AQUA_SIM_PHY_CMN_H
#define AQUA_SIM_PHY_CMN_H

#include <string>
#include <list>
#include <map>
#include <vector>

#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/traced-callback.h"
#include "aqua-sim-phy.h"
#include "aqua-sim-energy-model.h"
#include "aqua-sim-modulation.h"


//Aqua Sim Phy Cmn
namespace ns3
{

	class Packet;

	/**
 * \ingroup aqua-sim
 *
 * \brief Common phy layer class. Currently only supports a single phy component per node.
 */
	class AquaSimPhyCmn : public AquaSimPhy
	{
	public:
		AquaSimPhyCmn(void);
		virtual ~AquaSimPhyCmn(void);
		static TypeId GetTypeId(void);


		virtual void addModulation(Ptr<AquaSimModulation> modulation, int modulationNum);
		virtual Time calcTxTime(Ptr<Packet> pkt);
		virtual Time calcTxTime(uint32_t pktSize);
		virtual Time calcTxTime(uint32_t pktSize, int modulationNum);
		virtual double CalcPktSize(double txtime, int modulationNum);

		virtual std::vector<double> getBandWidth();
		virtual std::vector<double> getFreq();

		int64_t AssignStreams(int64_t stream);

	protected:
		virtual Ptr<Packet> StampTxInfo(Ptr<Packet> p);
		virtual bool matchPkt(Ptr<Packet> p);
		virtual void initmodulations();
		std::map<int, Ptr<AquaSimModulation>> modulations;
		int modulationNum; //default modulation
		double freq;	 // frequency

		friend class AquaSimEnergyModel;

		virtual void DoDispose();


	}; //AquaSimPhyCmn

} //namespace ns3

#endif /* AQUA_SIM_PHY_CMN_H */
