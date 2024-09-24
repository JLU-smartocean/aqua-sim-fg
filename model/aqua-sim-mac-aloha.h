/*
 * aqua-sim-mac-aloha.h
 *
 *  Created on: Aug 25, 2021
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */


#ifndef AQUA_SIM_MAC_ALOHA_H
#define AQUA_SIM_MAC_ALOHA_H

#include "aqua-sim-mac.h"

#include "ns3/event-id.h"
#include "ns3/random-variable-stream.h"
#include "ns3/timer.h"
#include "ns3/packet.h"

#include <queue>
#include <map>
#include <math.h>

#define CALLBACK_DELAY 0.001 //the interval between two consecutive sendings
#define MAXIMUMCOUNTER 3
#define Broadcast -1

namespace ns3
{

	class AquaSimAddress;
	/**
 * \ingroup aqua-sim-ng
 *
 * \brief Implementation of ALOHA (backoff assisted) protocol in underwater
 */

	class AquaSimAloha : public AquaSimMac
	{
	public:
		AquaSimAloha();
		~AquaSimAloha();
		static TypeId GetTypeId(void);
		int64_t AssignStreams(int64_t stream);

		virtual bool TxProcess(Ptr<Packet> pkt);
		virtual bool RecvProcess(Ptr<Packet> pkt);
		virtual uint32_t getHeaderSize();


		static std::vector<std::string> alohaStatusName;

	protected:
		enum
		{
			PASSIVE,
			BACKOFF,
			SEND,
			WAIT_ACK,
		} ALOHA_Status;


		uint8_t sendCnt;
		uint8_t m_AckOn;

		double guardTime;
		double m_persistent;
		double m_minBackoff;
		double m_maxBackoff;
		double m_maxACKRetryInterval;
		double m_maxPropDelay;

		EventId waitAckTimer;

		std::map<AquaSimAddress, uint8_t> nextSendPktSeq;
		std::map<AquaSimAddress, uint8_t> nextRecvPktSeq;	


		Ptr<Packet> MakeACK(AquaSimAddress dataSender, int ackSeq);

		void ReplyACK(AquaSimAddress dataSender, int ackSeq);


		void SendPkt(Ptr<Packet> pkt);
		void SendDataPkt();
		void DoBackoff();
		void ProcessPassive();
		void init();
		void addPktSeq(Ptr<Packet> pkt);
		bool validDataPktSeq(uint8_t pktSeq, AquaSimAddress srcAddress);
		bool validAckPktSeq(uint8_t pktSeq, AquaSimAddress srcAddress);

		void RetryACK(Ptr<Packet> ack);
		virtual void DoDispose();


	private:
		std::queue<Ptr<Packet>> pktQue;
		Ptr<UniformRandomVariable> m_rand;

	}; // class AquaSimAloha

} // namespace ns3

#endif /* AQUA_SIM_MAC_ALOHA_H */
