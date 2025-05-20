/*
 * aqua-sim-propagation-bellhop.h
 *
 *  Created on: May 8, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#ifndef SRC_AQUA_SIM_NG_MODEL_AQUA_SIM_PROPAGATION_BELLHOP_H_
#define SRC_AQUA_SIM_NG_MODEL_AQUA_SIM_PROPAGATION_BELLHOP_H_


#include "aqua-sim-propagation.h"
#include "ns3/event-id.h"
#include <string>
#include <vector>


namespace ns3 {

    class Packet;

    /**
     * \ingroup aqua-sim-ng
     *
     * \brief Simple propagation model. This propagation model calculates attenuation using rayleigh model and allows all nodes in the network to receive a copy.
     *   Depenedent on transmission factors (range/power/noise/etc.) other parts of simulator will determine if copy is decodable.
     */
    class AquaSimBellhopPropagation : public AquaSimPropagation {
    public:
        static TypeId GetTypeId(void);

        AquaSimBellhopPropagation(void);

        ~AquaSimBellhopPropagation(void);

        virtual std::vector <PktRecvUnit> *ReceivedCopies(Ptr <AquaSimNetDevice> s,
                                                          Ptr <Packet> p,
                                                          std::vector <Ptr<AquaSimNetDevice>> dList);



    protected:
        void bellhop(int senderID);
        double getRange(Vector sendPos, Vector recvPos);
        void updateEnvFile(int senderId, Vector sendPos, Vector recvPos, double freq);
        double readShd(std::string sender);
        Time readArr(std::string sender);

    private:
        std::string basePath;

    };  //class AquaSimSimplePropagation

}  // namespace ns3


#endif /* SRC_AQUA_SIM_NG_MODEL_AQUA_SIM_PROPAGATION_BELLHOP_H_ */
