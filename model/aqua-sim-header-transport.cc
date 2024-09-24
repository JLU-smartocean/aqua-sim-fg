#include <stdint.h>
#include <iostream>
#include "aqua-sim-header-transport.h"
#include "ns3/buffer.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TransportHeader");

NS_OBJECT_ENSURE_REGISTERED (TransportHeader);

TransportHeader::TransportHeader ()
  : m_subSeqNumber (0),
    m_seqNumber (0),
    m_count (0)
{
	//std::cout<<"chuangjianchenggong\n";
}

TransportHeader::~TransportHeader ()
{
	//std::cout<<"xiaohuichenggong\n";
}

/* Setters */

void
TransportHeader::SetSequenceNumber (uint8_t seqNumber)
{
  m_seqNumber = seqNumber;
}

void
TransportHeader::SetSubSequenceNumber (uint8_t subSeqNumber)
{
  m_subSeqNumber = subSeqNumber;
}

void
TransportHeader::SetCount(uint8_t count)
{
	m_count = count;
}

/* Getters */

uint8_t
TransportHeader::GetSequenceNumber ()
{
  return m_seqNumber;
}

uint8_t
TransportHeader::GetSubSequenceNumber ()
{
  return m_subSeqNumber;
}

uint8_t
TransportHeader::GetCount()
{
	return m_count;
}

/* Others */

TypeId
TransportHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TransportHeader")
    .SetParent<Header> ()
    .AddConstructor<TransportHeader> ()
  ;
  return tid;
}

TypeId
TransportHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
TransportHeader::Print (std::ostream &os)  const
{
  // os << "Transport Header: SendAddress=" << m_source << ", DestAddress=" << m_destination << ", SequenceNumber=" << +m_sequenceNumber << ",AckNumber=" << +m_ackNumber << ",Count=" << m_count << ",CheckSum="<<m_checksum<<",PacketType=";
  // switch(m_pType)
  // {
	// case DATA: os << "DATA"; break;
	// case ACK: os << "ACK"; break;
  // }
  os << "Transport Header: seqNumber=" << m_seqNumber << ", subSeqNumber=" << m_subSeqNumber << ", count=" << m_count;
  os << "\n";
}

uint32_t
TransportHeader::GetSerializedSize (void)  const
{
  return 1+1+1;
  // return 2+2+1+1+1+1+2;
}

void
TransportHeader::Serialize (Buffer::Iterator start)  const
{
  Buffer::Iterator i = start;
  i.WriteU8(m_seqNumber);
  i.WriteU8(m_subSeqNumber);
  i.WriteU8(m_count);

  /*
  i.WriteU16 (m_source.GetAsInt());
  i.WriteU16 (m_destination.GetAsInt());
  i.WriteU8 (m_sequenceNumber);
  i.WriteU8 (m_ackNumber);
  i.WriteU8 (m_pType);
  i.WriteU8 (m_count);
  i.WriteU16(m_checksum);
  */
  // Serialize options if they exist
  // This implementation does not presently try to align options on word
  // boundaries using NOP options
  // padding to word alignment; add ENDs and/or pad values (they are the same)

  // Make checksum
/*  if (m_calcChecksum)
    {
      uint16_t headerChecksum = CalculateHeaderChecksum (start.GetSize ());
      i = start;
      uint16_t checksum = i.CalculateIpChecksum (start.GetSize (), headerChecksum);

      i = start;
      i.Next (16);
      i.WriteU16 (checksum);
    }*/
}

uint32_t
TransportHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_seqNumber    = i.ReadU8();
  m_subSeqNumber = i.ReadU8();
  m_count        = i.ReadU8();
  return GetSerializedSize ();

  /*
  m_source = (AquaSimAddress) i.ReadU16();
  m_destination = (AquaSimAddress) i.ReadU16();
  m_sequenceNumber = i.ReadU8();
  m_ackNumber = i.ReadU8();
  m_pType = i.ReadU8();
  m_count = i.ReadU8();
  m_checksum=i.ReadU16();
  */
  // Do checksum
/*  if (m_calcChecksum)
    {
      uint16_t headerChecksum = CalculateHeaderChecksum (start.GetSize ());
      i = start;
      uint16_t checksum = i.CalculateIpChecksum (start.GetSize (), headerChecksum);
      m_goodChecksum = (checksum == 0);
    }*/
}

bool
operator== (const TransportHeader &lhs, const TransportHeader &rhs)
{
  return (
    lhs.m_seqNumber == rhs.m_seqNumber
    && lhs.m_subSeqNumber == rhs.m_subSeqNumber
    );
}

std::ostream&
operator<< (std::ostream& os, TransportHeader const & tc)
{
  tc.Print (os);
  return os;
}

} // namespace ns3




// void
// TransportHeader::SetPType(uint8_t pType)
// {
//   m_pType = pType;
// }

// void
// TransportHeader::SetSourceAddress(AquaSimAddress address)
// {
// 	m_source = address;
// }

// void
// TransportHeader::SetDestAddress(AquaSimAddress address)
// {
// 	m_destination = address;
// }

// AquaSimAddress
// TransportHeader::GetSourceAddress()
// {
// 	return m_source;
// }

// AquaSimAddress
// TransportHeader::GetDestAddress()
// {
// 	return m_destination;
// }

// uint8_t
// TransportHeader::GetAckNumber ()
// {
//   return m_ackNumber;
// }

// uint8_t
// TransportHeader::GetPType()
// {
//   return m_pType;
// }

// void
// TransportHeader::SetCheckSum(uint16_t checksum)
// {
// 	m_checksum=checksum;
// }
// uint16_t
// TransportHeader::GetCheckSum()
// {
// 	return m_checksum;
// }

/*uint8_t
TransportHeader::CalculateHeaderLength () const
{
  uint32_t len = 20;
  return len >> 2;
}*/