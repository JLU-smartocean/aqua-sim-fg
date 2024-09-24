/*
 * aqua-sim-propagation.cc
 *
 *  Created on: Sep 4, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#include "ns3/nstime.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"

#include "aqua-sim-propagation.h"

namespace ns3
{

	const double SOUND_SPEED_IN_WATER = 1500;

	NS_LOG_COMPONENT_DEFINE("AquaSimPropagation");
	NS_OBJECT_ENSURE_REGISTERED(AquaSimPropagation);

	TypeId
	AquaSimPropagation::GetTypeId()
	{
		static TypeId tid = TypeId("ns3::AquaSimPropagation")
								.SetParent<Object>();
		return tid;
	}

	Time
	AquaSimPropagation::PDelay(Ptr<MobilityModel> s, Ptr<MobilityModel> r)
	{
		NS_LOG_FUNCTION(this);
		return Time::FromDouble((s->GetDistanceFrom(r) / ns3::SOUND_SPEED_IN_WATER), Time::S);
	}




	double
	AquaSimPropagation::Thorp(double dist, double freq)
	{
		double k;
		k = 2;
		double distKm = dist / 1000.0;


		double t1 = pow(dist, k);

		double alpha = alphaDB(freq);
		alpha = pow(10.0, (alpha / 10.0));
		double t3 = pow(alpha, distKm);

		return t1 * t3;
	}



	/**
	 * @param SL sound level in dB
	 * @return receiving power in J
	 */
	double
	AquaSimPropagation::Rayleigh(double SL)
	{
		double mPr = std::pow(10, SL / 20 - 6); //signal strength (pressure in Pa)
		double segma = pow(mPr, 2) * 2 / M_PI;

		Ptr<UniformRandomVariable> m_rand = CreateObject<UniformRandomVariable>();

		return -2 * segma * std::log(m_rand->GetValue());
	}





	/**
	 * @param freq  central frequency
	 * @return 10*lg a(f)
	 */
	double
	AquaSimPropagation::alphaDB(double freq)
	{
		return (0.11 * pow(freq, 2) / (1 + pow(freq, 2)) + 
				44 * pow(freq, 2) / (4100 + pow(freq, 2)) + 
				0.000275 * pow(freq, 2) + 0.003);
	}


	/**
	 *
	 * @param dist	distance that singal travels
	 * @param freq  central frequency
	 * @return TL(DB)
	 */
	double
	AquaSimPropagation::ThorpDB(double dist, double freq)
	{

		double k, spre, abso;

		if (dist <= 500)
		{
			k = 3;
		}
		else if (dist <= 2000)
		{
			k = 2;
		}
		else
		{
			k = 1.5;
		}

		spre = k * 10 * log10(dist);
		abso = dist / 1000 * alphaDB(freq);
		return spre + abso;
	}


	void AquaSimPropagation::SetTraceValues(double a,double b,double c){

	}

} // namespace n3
