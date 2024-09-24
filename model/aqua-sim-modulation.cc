/*
 * aqua-sim-modulation.cc
 *
 *  Created on: Aug 25, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */


//Jan 28, 2021 debug TxTime function

#include <cmath>


#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/log.h"

#include "aqua-sim-modulation.h"

namespace ns3
{

    NS_OBJECT_ENSURE_REGISTERED(AquaSimModulation);

    AquaSimModulation::AquaSimModulation()
    {
        ber = 0;

        bitPerSymbol = 1;
        k = 1;
        n = 2;

        guardTime = 0.05;
        preTime = 0.5;
    }


    TypeId
    AquaSimModulation::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::AquaSimModulation")
                                .SetParent<Object>()
                                .AddAttribute("BER", "BER.",
                                            DoubleValue(0),
                                            MakeDoubleAccessor(&AquaSimModulation::ber),
                                            MakeDoubleChecker<double>())
                                .AddAttribute("BPS", "number of bit per symbols.",
                                            IntegerValue(1),
                                            MakeIntegerAccessor(&AquaSimModulation::bitPerSymbol),
                                            MakeIntegerChecker<int>())
                                .AddAttribute("K", "K",
                                            IntegerValue(1),
                                            MakeIntegerAccessor(&AquaSimModulation::k),
                                            MakeIntegerChecker<int>())
                                .AddAttribute("N", "N",
                                            IntegerValue(2),
                                            MakeIntegerAccessor(&AquaSimModulation::n),
                                            MakeIntegerChecker<int>())
                                .AddAttribute("guardTime", "the guardTime of the phy",
                                            DoubleValue(0.05),
                                            MakeDoubleAccessor(&AquaSimModulation::guardTime),
                                            MakeDoubleChecker<double>())
                                .AddAttribute("preTime", "the preTime of the phy",
                                            DoubleValue(0.5),
                                            MakeDoubleAccessor(&AquaSimModulation::preTime),
                                            MakeDoubleChecker<double>());
        return tid;
    }

} // namespace ns3
