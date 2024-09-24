/*
 * aqua-sim-propagation-range.h
 *
 *  Created on: Sep 4, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */


#ifndef AQUA_SIM_RANGE_PROPAGATION_H
#define AQUA_SIM_RANGE_PROPAGATION_H

#include "aqua-sim-propagation-simple.h"

namespace ns3
{

	/**
  * \ingroup aqua-sim-ng
  *
  * \brief Helper class used to detect which nodes are within propagation range of the sender.
  *
  *
  * Each node within the range specified by packet will receive a copy
  * still calculates attenuatin using rayleigh model
  * this would speed up the simulation if we don't expect
  * a very high accuracy in terms of collisions
  *
  * MUST make sure Pt and tx_range are consistent at the physical layer!!
  *
  * Additional acoutic models are provided.
  */
	class AquaSimRangePropagation : public AquaSimPropagation
	{
	public:
		static TypeId GetTypeId(void);
		AquaSimRangePropagation();
		virtual std::vector<PktRecvUnit> *ReceivedCopies(Ptr<AquaSimNetDevice> s,
														 Ptr<Packet> p,
											
											
														 std::vector<Ptr<AquaSimNetDevice>> dList);
		
		
		
		double AcousticSpeed(double depth);
		void SetTemp(double temp);
		void SetSalinity(double salinity);
		void Initialize();
		virtual void SetTraceValues(double temp, double salinity);

	private:
		double m_temp;
		double m_salinity;
	}; // class AquaSimRangePropagation

} // namespace ns3

#endif /* AQUA_SIM_RANGE_PROPAGATION_H */
