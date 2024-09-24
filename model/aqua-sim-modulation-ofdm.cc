/*
 * aqua-sim-modulation-ofdm.cc
 *
 *  Created on: Mar 14, 2022
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#include <cmath>


#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

#include "aqua-sim-modulation-ofdm.h"

namespace ns3
{

    NS_OBJECT_ENSURE_REGISTERED(AquaSimModulationOFDM);
    NS_LOG_COMPONENT_DEFINE("AquaSimModulationOFDM");

    AquaSimModulationOFDM::AquaSimModulationOFDM()
    {
        bandWidth = 6;
        df = 5.8555;
        pilotNum = 352;
        checkNum = 4;
        maxBlockNum = 16;

        Simulator::Schedule(Seconds(0.01), &AquaSimModulationOFDM::init, this);
    }

    void AquaSimModulationOFDM::init(){
        blockTime = 1 / df;

        int carrierNum = int(bandWidth * 1000 / df) - pilotNum;
        int bitNum = carrierNum * bitPerSymbol * k / n;
        int ByteNum = bitNum / 8;
        blockSize = ByteNum - checkNum;
        maxPktSize = blockSize * maxBlockNum;
    }


    TypeId
    AquaSimModulationOFDM::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::AquaSimModulationOFDM")
                                .SetParent<AquaSimModulation>()
                                .AddConstructor<AquaSimModulationOFDM>()
                                .AddAttribute("bandWidth", "the bandwitoh of the phy",
                                              DoubleValue(6.0),
                                              MakeDoubleAccessor(&AquaSimModulationOFDM::bandWidth),
                                              MakeDoubleChecker<double>())
                                .AddAttribute("deltaf", "the deltaf of the phy",
                                              DoubleValue(5.8555),
                                              MakeDoubleAccessor(&AquaSimModulationOFDM::df),
                                              MakeDoubleChecker<double>())
                                .AddAttribute("pilotNum", "pilotNum",
                                            IntegerValue(352),
                                            MakeIntegerAccessor(&AquaSimModulationOFDM::pilotNum),
                                            MakeIntegerChecker<int>())
                                .AddAttribute("checkNum", "checkNum",
                                            IntegerValue(4),
                                            MakeIntegerAccessor(&AquaSimModulationOFDM::checkNum),
                                            MakeIntegerChecker<int>())
                                .AddAttribute("maxBlockNum", "maxBlockNum",
                                            IntegerValue(16),
                                            MakeIntegerAccessor(&AquaSimModulationOFDM::maxBlockNum),
                                            MakeIntegerChecker<int>());
        return tid;
    }

    double
    AquaSimModulationOFDM::TxTime(int pktSize)
    {
        if(pktSize < blockSize)
            return preTime;
        if(pktSize > maxPktSize){
            pktSize = maxPktSize;
            NS_LOG_WARN("OFDM-pkt size too big");
        }
        int blockNum = ceil((double)pktSize / blockSize);
        double txtime = blockNum * (blockTime + guardTime) + preTime;
        NS_LOG_DEBUG("OFDM-pktsize:" << pktSize << " blocksize:" << blockSize
                        << " blocknum:" << blockNum << " blocktime:" << blockTime
                        << " txtime:" << txtime);
        return txtime;
    }



} // namespace ns3
