/*
 * aqua-sim-modulation.h
 *
 *  Created on: Aug 25, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#ifndef AQUA_SIM_MODULATION_H
#define AQUA_SIM_MODULATION_H

#include "ns3/object.h"

namespace ns3
{

    /**
 * \ingroup aqua-sim
 *
 * \brief Modulation class to assist channel/physical simulation.
 */
    class AquaSimModulation : public Object
    {

    public:
        static TypeId GetTypeId(void);

        AquaSimModulation();
        virtual ~AquaSimModulation() {}

        /*
        *  Get transmission time by packet size (expected in bits).
        */
        virtual double TxTime(int pktSize) = 0;





    protected:

        int bitPerSymbol;
        int k;
        int n;
        double ber;       //bit error rate

        double guardTime;
        double preTime;

    }; // AquaSimModulation

} // namespace ns3

#endif /* AQUA_SIM_MODULATION_H */
