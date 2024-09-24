/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 Emmanuelle Laprise, INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Emmanuelle Laprise <emmanuelle.laprise@bluekazoo.ca>
 *          Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#include "ns3/aqua-sim-socket.h"
#include "ns3/aqua-sim-pt-tag.h"
#include "ns3/aqua-sim-socket-address.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/aqua-sim-header.h"
#include "ns3/aqua-sim-trailer.h"
#include <algorithm>
#include "ns3/aqua-sim-header-transport.h"
#include <ctime>
#include <utility>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AquaSimSocket");

NS_OBJECT_ENSURE_REGISTERED(AquaSimSocket);

TypeId AquaSimSocket::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::AquaSimSocket").SetParent<Socket>().SetGroupName(
					"Network").AddConstructor<AquaSimSocket>().AddTraceSource(
					"Drop", "Drop packet due to receive buffer overflow",
					MakeTraceSourceAccessor(&AquaSimSocket::m_dropTrace),
					"ns3::Packet::TracedCallback").AddAttribute("RcvBufSize",
					"AquaSimSocket maximum receive buffer size (bytes)",
					UintegerValue(131072),
					MakeUintegerAccessor(&AquaSimSocket::m_rcvBufSize),
					MakeUintegerChecker<uint32_t>())
					.AddAttribute("Mode", "node transport mode", 
					UintegerValue(1), 
					MakeUintegerAccessor(&AquaSimSocket::m_modeid),
					MakeUintegerChecker<uint32_t>());
	return tid;
}

AquaSimSocket::AquaSimSocket() :
		m_rxAvailable(0) {
	NS_LOG_FUNCTION(this);
	m_state = STATE_OPEN;
	m_shutdownSend = false;
	m_shutdownRecv = false;
	m_errno = ERROR_NOTERROR;
	m_isSingleDevice = false;
	m_device = 0;
}

void AquaSimSocket::SetNode(Ptr<Node> node) {
	NS_LOG_FUNCTION(this << node);
	m_node = node;
}

AquaSimSocket::~AquaSimSocket() {
	NS_LOG_FUNCTION(this);
}

void AquaSimSocket::DoDispose(void) {
	NS_LOG_FUNCTION(this);
	m_device = 0;
}

enum Socket::SocketErrno AquaSimSocket::GetErrno(void) const {
	NS_LOG_FUNCTION(this);
	return m_errno;
}

enum Socket::SocketType AquaSimSocket::GetSocketType(void) const {
	NS_LOG_FUNCTION(this);
	return NS3_SOCK_RAW;
}

Ptr<Node> AquaSimSocket::GetNode(void) const {
	NS_LOG_FUNCTION(this);
	return m_node;
}

int AquaSimSocket::Bind(void) {
	NS_LOG_FUNCTION(this);
	AquaSimSocketAddress address;
	address.SetProtocol(0);
	return DoBind(address);
}

int AquaSimSocket::Bind6(void) {
	NS_LOG_FUNCTION(this);
	return (Bind());
}

int AquaSimSocket::Bind(const Address &address) {
	NS_LOG_UNCOND("AquaSimSocket::Bind :"<<GetNode()->GetId()<<":"<<address);
	NS_LOG_FUNCTION(this << address);
	if (!AquaSimSocketAddress::IsMatchingType(address)) { //NS_LOG_UNCOND ("!AquaSimSocketAddress::IsMatchingType (address)");
		m_errno = ERROR_INVAL;
		return -1;
	}
	AquaSimSocketAddress ad = AquaSimSocketAddress::ConvertFrom(address);
	return DoBind(ad);
}

int AquaSimSocket::DoBind(const AquaSimSocketAddress &address) { //NS_LOG_UNCOND ("$$$$$$$$$$$$$$$$$$$$$AquaSimSocket::DoBind");
	NS_LOG_FUNCTION(this << address);
	if (m_state == STATE_BOUND || m_state == STATE_CONNECTED) {
		NS_LOG_UNCOND("$$$$$$$$$$$$$$$$$$$$$Bind before");
		m_errno = ERROR_INVAL;
		return -1;
	}
	if (m_state == STATE_CLOSED) {
		NS_LOG_UNCOND("$$$$$$$$$$$$$$$$$$$$$state closed");
		m_errno = ERROR_BADF;
		return -1;
	}
	Ptr<NetDevice> dev;
	if (address.IsSingleDevice()) {
		dev = m_node->GetDevice(0);
	} else {
		dev = 0;
	}
	m_node->RegisterProtocolHandler(
			MakeCallback(&AquaSimSocket::ForwardUp, this),
			address.GetProtocol(), dev, true);
	m_state = STATE_BOUND;
	m_protocol = address.GetProtocol();
	m_isSingleDevice = address.IsSingleDevice();
	m_device = address.GetSingleDevice();
	m_boundnetdevice = dev;
	return 0;
}

int AquaSimSocket::ShutdownSend(void) {
	NS_LOG_FUNCTION(this);
	if (m_state == STATE_CLOSED) {
		m_errno = ERROR_BADF;
		return -1;
	}
	m_shutdownSend = true;
	return 0;
}

int AquaSimSocket::ShutdownRecv(void) {
	NS_LOG_FUNCTION(this);
	if (m_state == STATE_CLOSED) {
		m_errno = ERROR_BADF;
		return -1;
	}
	m_shutdownRecv = true;
	return 0;
}

int AquaSimSocket::Close(void) {
	NS_LOG_FUNCTION(this);
	if (m_state == STATE_CLOSED) {
		m_errno = ERROR_BADF;
		return -1;
	} else if (m_state == STATE_BOUND || m_state == STATE_CONNECTED) {
		m_node->UnregisterProtocolHandler(
				MakeCallback(&AquaSimSocket::ForwardUp, this));
	}
	m_state = STATE_CLOSED;
	m_shutdownSend = true;
	m_shutdownRecv = true;
	return 0;
}

int AquaSimSocket::Connect(const Address &ad) {
	AquaSimSocketAddress address;
	if (m_state == STATE_CLOSED) {
		m_errno = ERROR_BADF;
		goto error;
	}
	if (m_state == STATE_OPEN) {
		// connect should happen _after_ bind.
		m_errno = ERROR_INVAL; // generic error condition.
		goto error;
	}
	if (m_state == STATE_CONNECTED) {
		m_errno = ERROR_ISCONN;
		goto error;
	}
	if (!AquaSimSocketAddress::IsMatchingType(ad)) {
		m_errno = ERROR_AFNOSUPPORT;
		goto error;
	}
	m_destAddr = ad;
	m_state = STATE_CONNECTED;
	NotifyConnectionSucceeded();
	return 0;
	error: NotifyConnectionFailed();
	return -1;
}
int AquaSimSocket::Listen(void) {
	NS_LOG_FUNCTION(this);
	m_errno = Socket::ERROR_OPNOTSUPP;
	return -1;
}

int AquaSimSocket::Send(Ptr<Packet> p, uint32_t flags) {
	if (m_state == STATE_OPEN || m_state == STATE_BOUND) {
		std::cout << "--------AquaSimSocket::Send---wrong------------\n";
		m_errno = ERROR_NOTCONN;
		return -1;
	}

	AquaSimSocketAddress ad;
	ad = AquaSimSocketAddress::ConvertFrom(m_destAddr);
	Address dest = ad.GetDestinationAddress();
	
	
	AquaSimTrailer ast;
	uint8_t packetmode = (uint8_t)m_modeid;
	ast.SetSAddr(AquaSimAddress::ConvertFrom(GetNode()->GetDevice(0)->GetAddress()));
	ast.SetDAddr(AquaSimAddress::ConvertFrom(dest));
	ast.SetModeId(packetmode);
	p->AddTrailer(ast);

	SubPacket(p, packetmode);

	/** if all segment is transmitted, the transmission is considered successful(pkt's size), else false(-1)  **/
	int sendPktSize = 0;
	while (!PktSendQue.empty()) {
		Ptr<Packet> tempPkt = PktSendQue.front()->Copy();
        // std::cout << "son packet size::" << tempPkt->GetSize()<<"\n";
		int sendDownRes = SendTo(tempPkt, flags, m_destAddr);
		if (sendDownRes == -1) {
			std::cout << "\npacket send failed in transport, "
                      << "error pointer info:" << tempPkt
                      << std::endl;
            return -1;
		}
		if (sendDownRes != (int)tempPkt->GetSize()) {
			std::cout << "\nsent packet does not match the original packet, "
                      << "error pointer info:" << tempPkt
                      << std::endl;
            return -1;
		}
		sendPktSize += sendDownRes;
        PktSendQue.front() = 0;
        PktSendQue.pop();
	}

	return sendPktSize;
	/** changed by spinach -- end **/
}

uint32_t AquaSimSocket::GetMinMtu(AquaSimSocketAddress ad) const {
	NS_LOG_FUNCTION(this << ad);
	if (ad.IsSingleDevice()) {
		Ptr<NetDevice> device = m_node->GetDevice(ad.GetSingleDevice());
		return device->GetMtu();
	} else {
		uint32_t minMtu = 0xffff;
		for (uint32_t i = 0; i < m_node->GetNDevices(); i++) {
			Ptr<NetDevice> device = m_node->GetDevice(i);
			minMtu = std::min(minMtu, (uint32_t) device->GetMtu());
		}
		return minMtu;
	}
}

uint32_t AquaSimSocket::GetTxAvailable(void) const {
	NS_LOG_FUNCTION(this);
	if (m_state == STATE_CONNECTED) {
		AquaSimSocketAddress ad = AquaSimSocketAddress::ConvertFrom(m_destAddr);
		return GetMinMtu(ad);
	}
	// If we are not connected, we return a 'safe' value by default.
	return 0xffff;
}

int AquaSimSocket::SendTo(Ptr<Packet> p, uint32_t flags,
		const Address &address) {
	/*Test*/
	Ptr<Packet> packet = p->Copy();
	AquaSimPtTag ast;
	packet->RemovePacketTag(ast);
//	std::cout<<"Aqua-Sim-Socket:SendTo ptag:"<<ast.GetPacketType()<<std::endl;
//	std::cout<<"Aqua-Sim-Socket:SendTo AquaSimHeader:SA->"<<ash.GetSAddr()<<" DA->"<<ash.GetDAddr()<<std::endl;
	AquaSimSocketAddress ad;
	if (m_state == STATE_CLOSED) {
		std::cout << "----------ERROR_BADF-----\n";
		NS_LOG_LOGIC("ERROR_BADF");
		m_errno = ERROR_BADF;
		return -1;
	}
	if (m_shutdownSend) {
		std::cout << "----------ERROR_SHUTDOWN------\n";
		NS_LOG_LOGIC("ERROR_SHUTDOWN");
		m_errno = ERROR_SHUTDOWN;
		return -1;
	}
	if (!AquaSimSocketAddress::IsMatchingType(address)) {
		std::cout << "----------ERROR_AFNOSUPPORT------\n";
		NS_LOG_LOGIC("ERROR_AFNOSUPPORT");
		m_errno = ERROR_AFNOSUPPORT;
		return -1;
	}
	ad = AquaSimSocketAddress::ConvertFrom(address);

	uint8_t priority = GetPriority();
	if (priority) {
		SocketPriorityTag priorityTag;
		priorityTag.SetPriority(priority);
		p->ReplacePacketTag(priorityTag);
	}

	bool error = false;
	Address dest = ad.GetDestinationAddress();
	if (ad.IsSingleDevice()) {
		Ptr<NetDevice> device = m_node->GetDevice(ad.GetSingleDevice());
		if (!device->Send(p, dest, ad.GetProtocol())) {
			NS_LOG_LOGIC("error: NetDevice::Send error");
			error = true;
		}
	} else {
		for (uint32_t i = 0; i < m_node->GetNDevices(); i++) {
			Ptr<NetDevice> device = m_node->GetDevice(i);
			if (!device->Send(p, dest, ad.GetProtocol())) {
				NS_LOG_LOGIC("error: NetDevice::Send error");
				error = true;
			}
		}
	}
	if (!error) {
		NotifyDataSent(p->GetSize());
		NotifySend(GetTxAvailable());
	}

	if (error) {
		NS_LOG_LOGIC("ERROR_INVAL 2");
		std::cout << "ERROR_INVAL 2";
		m_errno = ERROR_INVAL;
		return -1;
	} else {
		return p->GetSize();
	}
}

void AquaSimSocket::ForwardUp(Ptr<NetDevice> device, Ptr<const Packet> packet,
		uint16_t protocol, const Address &from, const Address &to,
		NetDevice::PacketType packetType) {
	if (m_shutdownRecv) {
		return;
	}
	AquaSimSocketAddress address;
	address.SetDestinationAddress(from);
	address.SetSingleDevice(device->GetIfIndex());
	address.SetProtocol(protocol);

	/**
	 * 放入接收缓冲队列前先放在临时缓冲区队列等待后续数据到达
	 * 如果某一节点发送的数据全部收到，对数据进行组包操作，并放入接收缓冲区中
	 * -- by spinach
	 */
	Ptr<Packet> mergedPkt = RecvPktProcess(packet, address);

	if (mergedPkt == 0) {
		return;
	}
	//smergedPkt->Print(std::cout);

	if ((m_rxAvailable + mergedPkt->GetSize()) <= m_rcvBufSize) {
		Ptr<Packet> copy = mergedPkt->Copy();
		AquaDeviceNameTag dnt;
		dnt.SetDeviceName(device->GetTypeId().GetName());
		AquaSimSocketTag pst;
		pst.SetPacketType(packetType);
		pst.SetDestAddress(to);
		copy->AddPacketTag(pst); // Attach Packet Type and Dest Address
		copy->AddPacketTag(dnt); // Attach device source name
		// in case the packet still has a priority tag, remove it
		SocketPriorityTag priorityTag;
		copy->RemovePacketTag(priorityTag);
		m_deliveryQueue.push(std::make_pair(copy, address));

		m_rxAvailable += mergedPkt->GetSize();
		NS_LOG_LOGIC(
				"UID is " << mergedPkt->GetUid () << " AquaSimSocket " << this);
		NotifyDataRecv();
	} else {
		// In general, this case should not occur unless the
		// receiving application reads data from this socket slowly
		// in comparison to the arrival rate
		//
		// drop and trace packet
		NS_LOG_WARN("No receive buffer space available.  Drop.");
		m_dropTrace(mergedPkt);
	}
}

uint32_t AquaSimSocket::GetRxAvailable(void) const {
	NS_LOG_FUNCTION(this);
	// We separately maintain this state to avoid walking the queue
	// every time this might be called
	return m_rxAvailable;
}

Ptr<Packet> AquaSimSocket::Recv(uint32_t maxSize, uint32_t flags) {
	NS_LOG_FUNCTION(this << maxSize << flags);

	Address fromAddress;
	Ptr<Packet> packet = RecvFrom(maxSize, flags, fromAddress);
	return packet;
}

Ptr<Packet> AquaSimSocket::RecvFrom(uint32_t maxSize, uint32_t flags,
		Address &fromAddress) {
	NS_LOG_FUNCTION(this << maxSize << flags);

	if (m_deliveryQueue.empty()) {
		return 0;
	}
	Ptr<Packet> p = m_deliveryQueue.front().first;
	fromAddress = m_deliveryQueue.front().second;
	if (p->GetSize() <= maxSize) {
		m_deliveryQueue.pop();
		m_rxAvailable -= p->GetSize();
	} else {
		p = 0;
	}
	return p;
}

int AquaSimSocket::GetSockName(Address &address) const {
	NS_LOG_FUNCTION(this << address);
	AquaSimSocketAddress ad;

	ad.SetProtocol(m_protocol);
	if (m_isSingleDevice) {
		Ptr<NetDevice> device = m_node->GetDevice(m_device);
		ad.SetDestinationAddress(device->GetAddress());
		ad.SetSingleDevice(m_device);
	} else {
		ad.SetDestinationAddress(Address());
		ad.SetAllDevices();
	}
	address = ad;

	return 0;
}

int AquaSimSocket::GetPeerName(Address &address) const {
	NS_LOG_FUNCTION(this << address);

	if (m_state != STATE_CONNECTED) {
		m_errno = ERROR_NOTCONN;
		return -1;
	}

	address = m_destAddr;

	return 0;
}

bool AquaSimSocket::SetAllowBroadcast(bool allowBroadcast) {
	NS_LOG_FUNCTION(this << allowBroadcast);
	if (allowBroadcast) {
		return false;
	}
	return true;
}

bool AquaSimSocket::GetAllowBroadcast() const {
	NS_LOG_FUNCTION(this);
	return false;
}

int AquaSimSocket::SubPacket(Ptr<Packet> pkt, uint8_t pktMode) {
	/**
	 * 发送端暂时没有考虑多应用并行对多目的主机并行发送数据的情况
	 * （需要的话就要考虑socket并发，那就得重新写了）
	 */
	/**
	 * 根据不同的传输模式设置不同的MSS值，根据MSS值对传输的数据进行分拆
	 * 分拆后的数据段保存在PktSendQue中
	 * add by spinach
	 */ 
	uint16_t MSS;  // Maximum Segment Size
	// pktMode -= '0';//--useless
    switch ((int)pktMode)
    {
    case 0: MSS = 48;     break;
    case 1: MSS = 608;    break;
    case 2: MSS = 1280;   break;
    case 3: MSS = 1952;   break;
    case 4: MSS = 2624;   break;
    case 5: MSS = 3968;   break;
    default:
        MSS = 608;
    }
	/**
	 * MSS获取data部分的大小需要知道各层的headerSize
	 */
	// TODO:对MSS进行处理

	uint8_t buffer[65536];
	// AquaSimPtTag ast;
	AquaSimTrailer ast;
	TransportHeader tsh;
	pkt->RemoveTrailer(ast);
	// pkt->RemovePacketTag(ast);
	uint32_t size = pkt->GetSize();

    bool lastPktIsFull = size % MSS == 0 ? true : false;
    uint32_t fullPktCount = size / MSS;
    packetcount = fullPktCount + (lastPktIsFull ? 0 : 1);

	std::cout << "sub-packets' count is " << (int)packetcount
			  << ", data part size is " << (int)size
			  << ", trans mode is " << (int)pktMode
			  << ", MSS is " << (int)MSS << std::endl;

	uint8_t seqnumber = (uint8_t)(rand()%256);
    // process packet
    for (uint32_t i = 0; i < packetcount; i++) {
        Ptr<Packet> p = (i == packetcount-1 && !lastPktIsFull)
			? pkt->CreateFragment(fullPktCount * MSS, size - fullPktCount * MSS)
			: pkt->CreateFragment(i * MSS, MSS);
		if (i == packetcount-1 && !lastPktIsFull) {
			p->CopyData(buffer, size - fullPktCount * MSS);
		} else {
	        p->CopyData(buffer, MSS);
		}

        /** part of process transport header **/
		tsh.SetSequenceNumber(seqnumber);
		tsh.SetSubSequenceNumber(i);
		tsh.SetCount(packetcount);

        // p->AddPacketTag(ast);
        p->AddHeader(tsh);
		p->AddTrailer(ast);
        PktSendQue.push(p);
    }

	return 0;
}

Ptr<Packet> AquaSimSocket::RecvPktProcess(Ptr<const Packet> packet, AquaSimSocketAddress & address) {
	Ptr<Packet> mergedPkt = 0;
	Ptr<Packet> pkt = packet->Copy();
	TransportHeader tshh, tsh;
	AquaSimTrailer  ast;
	pkt->PeekHeader(tsh);
	pkt->PeekTrailer(ast);
	std::pair<Address, uint8_t> index(address, tsh.GetSequenceNumber());
	if (!PktRecvQues[index].empty()) {
		Ptr<Packet> tempPkt = PktRecvQues[index].back()->Copy();
		tempPkt->PeekHeader(tshh);

		if (tsh.GetSubSequenceNumber() == tshh.GetSubSequenceNumber() + 1) {
			std::cout << "Socket get packet from " << address.GetDestinationAddress() << ", number is " << (int)tsh.GetSubSequenceNumber() << "\n";
			recvCounts[index]++;
			PktRecvQues[index].push(pkt);
		} else {
			// sequence error and output relevant information
			std::cout << "Transport ForwardUp() : sequence error. "
					  << "aimed number is " << (int)tshh.GetSubSequenceNumber() + 1
					  << ", but get number is " << (int)tsh.GetSubSequenceNumber() << "\n";
			recvCounts[index] = 0;
			PktRecvQues.erase(index);
		}
	} else {
		recvCounts[index]++;
		PktRecvQues[index].push(pkt);
	}
	// this packet is the last packet
	if (recvCounts[index] == tsh.GetCount()) {
		std::cout << "get last sub-packet!\n";
		recvCounts[index] = 0;
		mergedPkt = MergePacket(PktRecvQues[index]);
		
		// clear the useless queue
		recvCounts.erase(index);
		PktRecvQues.erase(index);
	}

	return mergedPkt;
}

Ptr<Packet> AquaSimSocket::MergePacket(std::queue<Ptr<Packet> > & PktRecvQue) {
	// AquaSimPtTag ast;
	AquaSimTrailer ast;
    TransportHeader tsh;
    Ptr<Packet> retPacket = Create<Packet>();

    while (!PktRecvQue.empty()) {
        Ptr<Packet> pkt = PktRecvQue.front();
		pkt->RemoveHeader(tsh);
		pkt->RemoveTrailer(ast);
		// pkt->RemoveHeader(ast);
		retPacket->AddAtEnd(pkt);
		PktRecvQue.front() = 0;
		PktRecvQue.pop();
    }

	retPacket->AddTrailer(ast);
	return retPacket;
}


/***************************************************************
 *           PacketSocket Tags
 ***************************************************************/

AquaSimSocketTag::AquaSimSocketTag() {
}

void AquaSimSocketTag::SetPacketType(NetDevice::PacketType t) {
	m_packetType = t;
}

NetDevice::PacketType AquaSimSocketTag::GetPacketType(void) const {
	return m_packetType;
}

void AquaSimSocketTag::SetDestAddress(Address a) {
	m_destAddr = a;
}

Address AquaSimSocketTag::GetDestAddress(void) const {
	return m_destAddr;
}

NS_OBJECT_ENSURE_REGISTERED(AquaSimSocketTag);

TypeId AquaSimSocketTag::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::AquaSimSocketTag").SetParent<Tag>().SetGroupName(
					"Network").AddConstructor<AquaSimSocketTag>();
	return tid;
}
TypeId AquaSimSocketTag::GetInstanceTypeId(void) const {
	return GetTypeId();
}
uint32_t AquaSimSocketTag::GetSerializedSize(void) const {
	return 1 + m_destAddr.GetSerializedSize();
}
void AquaSimSocketTag::Serialize(TagBuffer i) const {
	i.WriteU8(m_packetType);
	m_destAddr.Serialize(i);
}
void AquaSimSocketTag::Deserialize(TagBuffer i) {
	m_packetType = (NetDevice::PacketType) i.ReadU8();
	m_destAddr.Deserialize(i);
}
void AquaSimSocketTag::Print(std::ostream &os) const {
	os << "packetType=" << m_packetType;
}

/***************************************************************
 *           DeviceName Tags
 ***************************************************************/

AquaDeviceNameTag::AquaDeviceNameTag() {
}

void AquaDeviceNameTag::SetDeviceName(std::string n) {
	if (n.substr(0, 5) == "ns3::") {
		n = n.substr(5);
	}
	m_deviceName = n;
}

std::string AquaDeviceNameTag::GetDeviceName(void) const {
	return m_deviceName;
}

NS_OBJECT_ENSURE_REGISTERED(AquaDeviceNameTag);

TypeId AquaDeviceNameTag::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::AquaDeviceNameTag").SetParent<Tag>().SetGroupName(
					"Network").AddConstructor<AquaDeviceNameTag>();
	return tid;
}
TypeId AquaDeviceNameTag::GetInstanceTypeId(void) const {
	return GetTypeId();
}
uint32_t AquaDeviceNameTag::GetSerializedSize(void) const {
	uint32_t s = 1 + m_deviceName.size();  // +1 for name length field
	return s;
}
void AquaDeviceNameTag::Serialize(TagBuffer i) const {
	const char *n = m_deviceName.c_str();
	uint8_t l = (uint8_t) m_deviceName.size();

	i.WriteU8(l);
	i.Write((uint8_t*) n, (uint32_t) l);
}
void AquaDeviceNameTag::Deserialize(TagBuffer i) {
	uint8_t l = i.ReadU8();
	char buf[256];

	i.Read((uint8_t*) buf, (uint32_t) l);
	m_deviceName = std::string(buf, l);
}
void AquaDeviceNameTag::Print(std::ostream &os) const {
	os << "DeviceName=" << m_deviceName;
}

} // namespace ns3
