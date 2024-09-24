/*
 * aqua-sim-propagation.cc
 *
 *  Created on: Sep 4, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#ifndef AQUA_SIM_PROPAGATION_H
#define AQUA_SIM_PROPAGATION_H

#include <vector>

#include "ns3/nstime.h"
#include "ns3/object.h"
#include "aqua-sim-net-device.h"

namespace ns3
{

	class AquaSimNetDevice;
	class Packet;
	class MobilityModel;

	extern const double SOUND_SPEED_IN_WATER;

	struct PktRecvUnit
	{
		double pR;
		Time pDelay;
		Ptr<AquaSimNetDevice> recver;
		PktRecvUnit() : pR(-1), pDelay(-1), recver(NULL) {}
		~PktRecvUnit() { recver = 0; }
	};

	/**
 * \ingroup aqua-sim-ng
 *
 * \brief Base class for underwater propagation model.
 *
 * Calculates the Pr by which the receiver will get a packet sent by
 * the node that applied the tx PacketStamp for a given interface type
 */
	class AquaSimPropagation : public Object
	{
	public:
		static TypeId GetTypeId(void);

		virtual std::vector<PktRecvUnit> *ReceivedCopies(Ptr<AquaSimNetDevice> s,
														 Ptr<Packet> p,
														 std::vector<Ptr<AquaSimNetDevice>> dList) = 0;
		


		virtual Time PDelay(Ptr<MobilityModel> s, Ptr<MobilityModel> r);
		virtual void SetTraceValues(double, double, double);

	protected:
		double alphaDB(double freq);


		double Rayleigh(double SL);
		double Thorp(double dist, double freq);
		double ThorpDB(double dist, double freq);
	}; //class AquaSimPropagation

} // namespace ns3

#endif /* AQUA_SIM_PROPAGATION_H */
