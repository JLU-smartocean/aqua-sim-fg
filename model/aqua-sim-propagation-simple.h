/*
 * aqua-sim-propagation-simple.h
 *
 *  Created on: Sep 4, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */


#ifndef AQUA_SIM_SIMPLE_PROPAGATION_H
#define AQUA_SIM_SIMPLE_PROPAGATION_H

#include <vector>
#include "aqua-sim-propagation.h"

namespace ns3
{

	class Packet;

	/**
  * \ingroup aqua-sim-ng
  *
  * \brief Simple propagation model. This propagation model calculates attenuation using rayleigh model and allows all nodes in the network to receive a copy.
  *   Depenedent on transmission factors (range/power/noise/etc.) other parts of simulator will determine if copy is decodable.
  */
	class AquaSimSimplePropagation : public AquaSimPropagation
	{
	public:
		static TypeId GetTypeId(void);
		AquaSimSimplePropagation(void);
		~AquaSimSimplePropagation(void);

		virtual std::vector<PktRecvUnit> *ReceivedCopies(Ptr<AquaSimNetDevice> s,
														 Ptr<Packet> p,
														 std::vector<Ptr<AquaSimNetDevice>> dList);


	protected:
		double getRecvPower(double dist, double freq, double pT);

	}; //class AquaSimSimplePropagation

} // namespace ns3

#endif /* AQUA_SIM_SIMPLE_PROPAGATION_H */
