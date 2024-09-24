/*
 * aqua-sim-modulation-ofdm.h
 *
 *  Created on: Mar 14, 2022
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#ifndef AQUA_SIM_MODULATION_OFDM_H
#define AQUA_SIM_MODULATION_OFDM_H



#include "ns3/object.h"
#include "aqua-sim-modulation.h"

namespace ns3
{

    /**
     * \ingroup aqua-sim
     *
     * \brief OFDM Modulation class to assist channel/physical simulation.
     */
    class AquaSimModulationOFDM : public AquaSimModulation  
    {

    public:
        static TypeId GetTypeId(void);

        AquaSimModulationOFDM();
        virtual ~AquaSimModulationOFDM() {}

        /*
         *  Get transmission time by packet size (expected in bits).
         */
        virtual double TxTime(int pktSize);
        void init();

    protected:

        double df;
        int pilotNum;
        int checkNum;
        double bandWidth;

        int maxBlockNum;
        int blockSize;  
        int maxPktSize;

        double blockTime;


    }; // AquaSimModulationOFDM

} // namespace ns3

#endif /* AQUA_SIM_MODULATION_OFDM_H */
