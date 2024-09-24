/*
 * aqua-sim-mac-TDMA.h
 *
 *  Created on: Oct 28, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */


#ifndef AQUA_SIM_MAC_TDMA_H
#define AQUA_SIM_MAC_TDMA_H

#include "aqua-sim-mac.h"
#include "ns3/event-id.h"
#include "ns3/random-variable-stream.h"
#include "ns3/timer.h"
#include "ns3/packet.h"

#include <queue>
#include <map>
#include <math.h>


namespace ns3
{

	class AquaSimAddress;
	/**
 * \ingroup aqua-sim-ng
 *
 * \brief Implementation of ALOHA (backoff assisted) protocol in underwater
 */

	class AquaSimTDMA : public AquaSimMac
	{
	public:
		AquaSimTDMA();
		~AquaSimTDMA();
		static TypeId GetTypeId(void);
		int64_t AssignStreams(int64_t stream);

		virtual bool TxProcess(Ptr<Packet> pkt);
		virtual bool RecvProcess(Ptr<Packet> pkt);

	protected:
		double guardTime;		// 保护时间
		double m_maxPropDelay;


		void SendPkt();
		void init();

		virtual void DoDispose();


	private:
		std::queue<Ptr<Packet>> pktQue;
		Ptr<UniformRandomVariable> m_rand;
		int token;
		int slotNum;
		Time slotLen;
		Time calTime2Send();
		void scheduleNextPkt();

	}; // class AquaSimTDMA

} // namespace ns3

#endif /* AQUA_SIM_MAC_TDMA_H */
