// #ifndef AQUA_SIM_TRANSPORT_H
// #define AQUA_SIM_TRANSPORT_H

// #include "ns3/object.h"
// #include "ns3/address.h"
// #include "ns3/nstime.h"
// #include "ns3/traced-value.h"
// #include "ns3/packet.h"

// #include "aqua-sim-address.h"

// #include "aqua-sim-net-device.h"
// #include <queue>
// #include "ns3/timer.h"
// #include "ns3/event-id.h"

// #define MTU 500

// namespace ns3 {

// class AquaSimMac;
// class AquaSimRouting;
// class AquaSimTransport : public Object
// {
// public:
//   enum {
// 	  PASSIVE,
// 	  WAIT_ACK,
//   }Trans_Status;

//   static TypeId GetTypeId(void);
//   AquaSimTransport(void);
//   virtual ~AquaSimTransport(void);

//   virtual void SetNetDevice(Ptr<AquaSimNetDevice> device);
//   virtual void SetMac(Ptr<AquaSimMac> mac);
//   virtual void SetRouting(Ptr<AquaSimRouting> routing);
//   virtual Ptr<AquaSimNetDevice> GetNetDevice();
//   virtual Ptr<AquaSimMac> GetMac();
//   virtual Ptr<AquaSimRouting> GetRouting();

//   virtual bool RecvTran(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber);
//   virtual bool RecvTran(Ptr<Packet> packet);

//   Ptr<Packet> MakeACK(AquaSimAddress RTS_Sender);

//   void	ReplyACK(Ptr<Packet> pkt);
//   void	SendPacket();
//   //void	StatusProcess(bool isAck);
//   bool ReTrans();
//   void SubPacket(Ptr<Packet> pkt);

//   Ptr<Packet> MergePacket();

//   static uint16_t CheckSum(uint8_t *a, int len);


// private:
//   Ptr<AquaSimNetDevice> m_device;
//   Ptr<AquaSimMac> m_mac;
//   Ptr<AquaSimRouting> m_routing;
//   std::queue<Ptr<Packet> >	PktSQueue; //send
//   std::queue<Ptr<Packet> >	PktRQueue; //recv
//   uint8_t sendupcount;
//   EventId m_waitACKTimer;
//   uint8_t retrans_count;
//   uint32_t packetcount;
// };
// }

// #endif /* AQUA_SIM_TRANSPORT_H_ */
