// #include "ns3/log.h"
// #include "ns3/attribute.h"
// #include "ns3/simulator.h"
// #include "ns3/ptr.h"
// #include "ns3/pointer.h"
// #include "ns3/trace-source-accessor.h"

// #include "aqua-sim-header.h"
// #include "aqua-sim-routing.h"
// #include "aqua-sim-socket.h"
// #include "aqua-sim-mac.h"
// #include <iostream>
// #include "aqua-sim-transport.h"
// #include "aqua-sim-header-transport.h"
// #include "aqua-sim-header-mac.h"
// #include "aqua-sim-pt-tag.h"
// namespace ns3 {

// NS_LOG_COMPONENT_DEFINE("AquaSimTransport");
// NS_OBJECT_ENSURE_REGISTERED(AquaSimTransport);

// TypeId AquaSimTransport::GetTypeId(void) {
// 	static TypeId tid =
// 			TypeId("ns3::AquaSimTransport").SetParent<Object>().AddConstructor<
// 					AquaSimTransport>();
// 	return tid;
// }

// AquaSimTransport::AquaSimTransport() :
// 		Trans_Status(PASSIVE), sendupcount(0), retrans_count(0) {

// 	NS_LOG_INFO("TRANS");

// }

// AquaSimTransport::~AquaSimTransport() {
// }

// void AquaSimTransport::SetNetDevice(Ptr<AquaSimNetDevice> device) {
// 	m_device = device;
// }

// void AquaSimTransport::SetMac(Ptr<AquaSimMac> mac) {
// 	m_mac = mac;
// }

// void AquaSimTransport::SetRouting(Ptr<AquaSimRouting> routing) {
// 	m_routing = routing;
// }

// Ptr<AquaSimNetDevice> AquaSimTransport::GetNetDevice() {
// 	return m_device;
// }
// Ptr<AquaSimMac> AquaSimTransport::GetMac() {
// 	return m_mac;
// }
// Ptr<AquaSimRouting> AquaSimTransport::GetRouting() {
// 	return m_routing;
// }

// uint16_t AquaSimTransport::CheckSum(uint8_t *a, int len) {
// 	uint16_t sum = 0;

// 	while (len > 1) {

// 		sum += *a++;
// 		len -= 1;
// 	}

// 	while (sum >> 8) {
// 		sum = (sum >> 8) + (sum & 0xff);
// 	}

// 	return (uint16_t) (~sum);
// }

// /*make Ack*/
// Ptr<Packet> AquaSimTransport::MakeACK(AquaSimAddress SourceAddress) {
// 	Ptr<Packet> temppkt = PktRQueue.back()->Copy();
// 	AquaSimHeader ash;
// 	TransportHeader tsh;
// 	temppkt->RemoveHeader(ash);
// 	temppkt->PeekHeader(tsh);
// 	temppkt->AddHeader(ash);

// 	Ptr<Packet> pkt = Create<Packet>();
// 	AquaSimHeader asHeader;
// 	TransportHeader tsHeader;
// 	//AquaSimPtTag ptag;
// 	asHeader.SetSAddr(AquaSimAddress::ConvertFrom(m_device->GetAddress()));
// 	// std::cout<<"####SourceAddress####"<<asHeader.GetSAddr()<<"\n";
// 	asHeader.SetDAddr(SourceAddress);
// 	// std::cout<<"####DestAddress####"<<asHeader.GetDAddr()<<"\n";
// 	asHeader.SetSize(asHeader.GetSize() + tsHeader.GetSerializedSize());
// 	//asHeader.SetTxTime(m_device->GetMac()->GetTxTime(asHeader.GetSerializedSize() + tsHeader.GetSerializedSize()));
// 	asHeader.SetErrorFlag(false);
// 	asHeader.SetDirection(AquaSimHeader::DOWN);

// 	tsHeader.SetPType(TransportHeader::ACK);
// 	tsHeader.SetSourceAddress(
// 			AquaSimAddress::ConvertFrom(m_device->GetAddress()));
// 	tsHeader.SetDestAddress(SourceAddress);
// 	tsHeader.SetAckNumber(tsh.GetSequenceNumber() + 1);

// 	pkt->AddHeader(tsHeader);
// 	pkt->AddHeader(asHeader);

// 	//std::cout<<"make ack type id"<<+tsHeader.GetPType()<<"\n";

// 	return pkt;
// }

// /*send packet*/
// void AquaSimTransport::SendPacket() {
// 	if (Trans_Status == PASSIVE) {
// 		if (!PktSQueue.empty()) {
// 			Ptr<Packet> temp = PktSQueue.front()->Copy();
// 			AquaSimHeader ash;
// 			TransportHeader tsh;
// 			temp->RemoveHeader(ash);
// 			temp->PeekHeader(tsh);
// 			temp->AddHeader(ash);
// //			std::cout << "Node "<<AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt()
// //					<< ":Send SequenceNumber:" << +tsh.GetSequenceNumber()<<" Packet"<<std::endl;

// 			m_routing->Recv(temp, ash.GetDAddr(), 0);
// 			Trans_Status = WAIT_ACK;

// 		}
// 	}
// }
// //relay ACK
// void AquaSimTransport::ReplyACK(Ptr<Packet> pkt) {
// 	AquaSimHeader asHeader;
// 	pkt->PeekHeader(asHeader);
// 	Ptr<Packet> p_ack = MakeACK(asHeader.GetSAddr());

// 	AquaSimHeader ash;
// 	TransportHeader tsh;
// 	p_ack->RemoveHeader(ash);
// 	p_ack->PeekHeader(tsh);
// 	p_ack->AddHeader(ash);

// 	std::cout << "\n"
// 			<< AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt()
// 			<< "node	Send ACK	ACKnumber:" << +tsh.GetAckNumber() << std::endl;
// 	m_device->GetRouting()->Recv(p_ack, asHeader.GetSAddr(), 0);

// }

// void AquaSimTransport::SubPacket(Ptr<Packet> pkt) {
// 	//uint8_t buffer[MTU];
// 	uint8_t buffer[65536];
// 	uint16_t checksum;
// 	AquaSimPtTag ast;
// 	AquaSimHeader ash;
// 	TransportHeader tsh;
// 	pkt->RemoveHeader(ash);
// 	pkt->RemovePacketTag(ast);
// 	uint32_t size = pkt->GetSize();
// 	if (size % MTU == 0) {
// 		packetcount = size / MTU;
// 		for (uint32_t i = 0; i < packetcount; i++) {
// 			Ptr<Packet> p = pkt->CreateFragment(i * MTU, MTU);
// 			p->CopyData(buffer, MTU);
// 			uint8_t* m_Buffer=new uint8_t[65536];
// 			pkt->CopyData(m_Buffer,pkt->GetSize());
// 			std::string str = (char*)m_Buffer;
// 			std::cout<<str.size()<<" "<<p->GetSize()<<std::endl;
// 			checksum = CheckSum(buffer, MTU);
// 			tsh.SetCheckSum(checksum);
// 			tsh.SetCount(packetcount);
// 			tsh.SetSequenceNumber(i);
// 			tsh.SetSourceAddress(ash.GetSAddr());
// 			tsh.SetDestAddress(ash.GetDAddr());
// 			tsh.SetPType(TransportHeader::DATA);
// 			p->AddPacketTag(ast);
// 			p->AddHeader(tsh);
// 			p->AddHeader(ash);
// 			PktSQueue.push(p);
// 		}
// 	} else {
// 		packetcount = size / MTU + 1;
// 		for (uint32_t i = 0; i < packetcount - 1; i++) {
// 			Ptr<Packet> p = pkt->CreateFragment(i * MTU, MTU);
// 			p->CopyData(buffer, MTU);
// 			checksum = CheckSum(buffer, MTU);
// 			tsh.SetCheckSum(checksum);
// 			tsh.SetCount(packetcount);
// 			tsh.SetSequenceNumber(i);
// 			tsh.SetSourceAddress(ash.GetSAddr());
// 			tsh.SetDestAddress(ash.GetDAddr());
// 			tsh.SetPType(TransportHeader::DATA);
// 			p->AddHeader(tsh);
// 			p->AddHeader(ash);
// 			PktSQueue.push(p);
// 		}
// 		Ptr<Packet> p = pkt->CreateFragment((packetcount - 1) * MTU,
// 				size - (packetcount - 1) * MTU);
// 		p->CopyData(buffer, size - (packetcount - 1) * MTU);
// 		checksum = CheckSum(buffer, size - (packetcount - 1) * MTU);
// 		tsh.SetCheckSum(checksum);
// 		tsh.SetCount(packetcount);
// 		tsh.SetSequenceNumber(packetcount - 1);
// 		tsh.SetSourceAddress(ash.GetSAddr());
// 		tsh.SetDestAddress(ash.GetDAddr());
// 		tsh.SetPType(TransportHeader::DATA);
// 		p->AddPacketTag(ast);
// 		p->AddHeader(tsh);
// 		p->AddHeader(ash);
// 		PktSQueue.push(p);
// 	}
// //	std::cout<<"Transport Layer start to divide packet,packet count -> "<<packetcount<<std::endl;

// }

// Ptr<Packet> AquaSimTransport::MergePacket() {
// 	AquaSimHeader ash;
// 	TransportHeader tsh;

// 	Ptr<Packet> packetadd = Create<Packet>();

// 	while (!PktRQueue.empty()) {
// 		Ptr<Packet> pkt = PktRQueue.front();
// 		pkt->RemoveHeader(ash);
// 		pkt->RemoveHeader(tsh);
// 		packetadd->AddAtEnd(pkt);
// 		PktRQueue.front() = 0;
// 		PktRQueue.pop();
// 	}

// 	packetadd->AddHeader(ash);
// 	return packetadd;
// }

// bool AquaSimTransport::RecvTran(Ptr<Packet> p, const Address &dest,
// 		uint16_t protocolNumber) {
// 	SubPacket(p);
// 	while (!PktSQueue.empty()) {
// 		if (Trans_Status == PASSIVE) {
// //			std::cout << "Node "<<AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt()<<" Transport Layer Sending Down packet"
// //					<<std::endl;
// 			//add timer
// //			m_waitACKTimer = Simulator::Schedule(Seconds(5),
// //					&AquaSimTransport::ReTrans, this);
// 			SendPacket();
// 			return true;
// 		}
// 	}
// 	return false;
// }

// bool AquaSimTransport::RecvTran(Ptr<Packet> p) {

// 	AquaSimHeader ashh;
// 	TransportHeader tshh;
// 	Ptr<Packet> temppkt;
// 	AquaSimHeader ash;
// 	p->RemoveHeader(ash);


// 	uint8_t flag = ash.GetDirection();
// 	TransportHeader tsh;
// 	p->PeekHeader(tsh);
// 	//uint16_t protocalNum = tsh.protocalNum;
// 	uint8_t tempType = tsh.GetPType();

// 	p->AddHeader(ash);
// 	p->PeekHeader(ash);

// 	if (tempType == TransportHeader::DATA && flag == AquaSimHeader::UP) {
// 	//	std::cout<<"Trans receive data"<<std::endl;
// 	//	p->Print(std::cout);
// 		p->RemoveHeader(ash);
// 		p->RemoveHeader(tsh);
// 		p->AddHeader(tsh);
// 		p->AddHeader(ash);
// 		if (!PktRQueue.empty()) {
// 				temppkt = PktRQueue.back()->Copy();
// 				temppkt->RemoveHeader(ashh);
// 				temppkt->PeekHeader(tshh);
// 				temppkt->AddHeader(ashh);

// 	  if (tsh.GetSequenceNumber() == tshh.GetSequenceNumber() + 1) {
// 					sendupcount++;
// 					PktRQueue.push(p);
// 				}		 		 //
// 			} else {
// 				sendupcount++;
// 				PktRQueue.push(p);
// 			}
// 		if (sendupcount == tsh.GetCount())              // pin jie gong zuo
// 				{
// 			sendupcount = 0;
// 			/*while(!PktRQueue.empty())
// 			 {
// 			 std::cout<<"I am transport layer. SendingUp packet"<<"	"<<AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt() <<"\n";
// 			 m_device->SendUp(PktRQueue.front()->Copy());
// 			 PktRQueue.front()=0;
// 			 PktRQueue.pop();
// 			 }
// 			 return true;*/

// 		//	ReplyACK(p);
// //			std::cout << "\n"
// //					<< AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt()
// //					<< " Received Packet	SequenceNumber:"
// //					<< +tsh.GetSequenceNumber() << std::endl;
// 			m_device->SendUp(MergePacket());
// //			std::cout << "I am transport layer. SendingUp packet" << "	"
// //					<< AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt()
// //					<< "\n";

// 			return true;
// 		}
// 		// std::cout<<"\n"<<AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt() <<"	Received Packet	SequenceNumber:"<<+tsh.GetSequenceNumber()<<std::endl;
// 				 // ReplyACK(p);
// 		return true;
// 	}

// 	if (tempType == TransportHeader::ACK && flag == AquaSimHeader::UP) {

// 		temppkt = PktSQueue.front()->Copy();
// 		temppkt->RemoveHeader(ashh);
// 		temppkt->PeekHeader(tshh);
// 		if (tshh.GetSequenceNumber() == tsh.GetAckNumber() - 1) {
// 			m_waitACKTimer.Cancel();
// 			AquaSimHeader ash;
// 			p->RemoveHeader(ash);
// 			ash.SetDirection(AquaSimHeader::DOWN);
// 			p->AddHeader(ash);
// 			PktSQueue.front() = 0;
// 			PktSQueue.pop();
// 			retrans_count = 0;
// 			Trans_Status = PASSIVE;
// //			std::cout << "\n"
// //					<< AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt()
// //					<< "	receive ACK	ACKnumber:" << +tsh.GetAckNumber()
// //					<< std::endl;
// 			SendPacket();
// 			return true;
// 		}
// 	}
// 	return false;
// }

// bool AquaSimTransport::ReTrans() {
// 	if (retrans_count >= 3) {
// 		sendupcount++;
// 		PktSQueue.front() = 0;
// 		PktSQueue.pop();
// 		retrans_count = 0;
// 		Trans_Status = PASSIVE;
// 		return true;
// 	} else {
// 		retrans_count++;
// 		//	std::cout<<"\n\n**************"<<+retrans_count<<"**************\n\n";
// 		Ptr<Packet> temp = PktSQueue.front();
// 		AquaSimHeader ash;
// 		temp->PeekHeader(ash);
// 		AquaSimAddress destination = ash.GetDAddr();
// 		//	std::cout<<AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt()<<"	I am transport layer. ReTrans Sending down packet\n";

// 		//Trans_Status = WAIT_ACK;
// 		//return m_device->GetRouting()->Recv(PktSQueue.front()->Copy(), destination, 0);
// 		Trans_Status = PASSIVE;
// 		SendPacket();
// 		return true;
// 	}
// }

// }

