/*
 * aqua-sim-mac-broadcast.h
 *
 *  Created on: Sep 9, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#ifndef AQUA_SIM_MAC_BROADCAST_H
#define AQUA_SIM_MAC_BROADCAST_H

#include "aqua-sim-mac.h"

namespace ns3
{

	/**
	 * \ingroup aqua-sim-ng
	 *
	 * \brief Broadcast MAC using basic backoff mechanism
	 */
	class AquaSimBroadcastMac : public AquaSimMac
	{
	public:
		AquaSimBroadcastMac();
		int64_t AssignStreams(int64_t stream);

		static TypeId GetTypeId(void);

		virtual bool RecvProcess(Ptr<Packet>);

		virtual bool TxProcess(Ptr<Packet>);
		void sendPkt(Ptr<Packet> pkt);

		virtual uint32_t getHeaderSize();

	protected:
		virtual void DoDispose();

	private:
		int backoffCounter;
		Ptr<UniformRandomVariable> rand;

	}; // class AquaSimBroadcastMac

} // namespace ns3

#endif /* AQUA_SIM_MAC_BROADCAST_H */
