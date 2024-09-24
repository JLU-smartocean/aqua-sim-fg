#ifndef AQUA_SIM_HEADER_TRANSPORT_H
#define AQUA_SIM_HEADER_TRANSPORT_H

#include <stdint.h>
#include "ns3/header.h"
#include "ns3/buffer.h"
// #include "ns3/tcp-socket-factory.h"
// #include "ns3/ipv4-address.h"
// #include "ns3/ipv6-address.h"
#include "ns3/sequence-number.h"
// #include "aqua-sim-address.h"

namespace ns3
{

class TransportHeader : public Header
{
public:
    TransportHeader();
    virtual ~TransportHeader();

    /**
     * \brief Print a Transport header into an output stream
     * \param os output stream
     * \param tc TCP header to print
     * \return The ostream passed as first argument
     */
    friend std::ostream &operator<<(std::ostream &os, TransportHeader const &tc);

    /**
     * Comparison operator
     * \param lhs left operand
     * \param rhs right operand
     * \return true if the operands are equal
     */
    friend bool operator== (const TransportHeader &lhs, const TransportHeader &rhs);

    /**
     * Setters
     */
    void SetSequenceNumber(uint8_t seqNumber);
    void SetSubSequenceNumber(uint8_t subSeqNumber);
    void SetCount(uint8_t count);

    /**
     * Getters
     */
    uint8_t GetSequenceNumber();
    uint8_t GetSubSequenceNumber();
    uint8_t GetCount();

    static  TypeId    GetTypeId(void);
    virtual TypeId    GetInstanceTypeId(void) const;
    virtual void      Print(std::ostream &os) const;
    virtual uint32_t  GetSerializedSize(void) const;
    virtual void      Serialize(Buffer::Iterator start) const;
    virtual uint32_t  Deserialize(Buffer::Iterator start);

private:
    uint8_t m_seqNumber;    //<! the large packet's number
    uint8_t m_subSeqNumber; //<! sub-packet's sequence number
    uint8_t m_count;        //<! count of sub-pkts separated from large data pkt
  };

} // namespace ns3

#endif /* AQUA_SIM_HEADER_TRANSPORT */

    // enum PacketType {
    // DATA,
    // ACK
    // } packet_type;

//   /**
//    * \brief Enable checksum calculation for TCP
//    *
//    * \todo currently has no effect
//    */
//   void EnableChecksums (void);

// //Setters

//   /**
//    * \brief Set the sequence Number
//    * \param sequenceNumber the sequence number for this TransportHeader
//    */
//   void SetSequenceNumber (uint8_t sequenceNumber);

//   /**
//    * \brief Set the ACK number
//    * \param ackNumber the ACK number for this TransportHeader
//    */
//   void SetAckNumber (uint8_t ackNumber);

//   void SetPType(uint8_t pType);

//   uint8_t GetPType();

// //Getters

//   /**
//    * \brief Get the sequence number
//    * \return the sequence number for this TransportHeader
//    */
//   uint8_t GetSequenceNumber ();

//   /**
//    * \brief Get the ACK number
//    * \return the ACK number for this TransportHeader
//    */
//   uint8_t GetAckNumber ();

//   void SetSourceAddress(AquaSimAddress address);
//   void SetDestAddress(AquaSimAddress address);
//   AquaSimAddress GetSourceAddress();
//   AquaSimAddress GetDestAddress();
//   void SetCount(uint8_t count);
//   uint8_t GetCount();
//   void SetCheckSum(uint16_t checksum);
//   uint16_t GetCheckSum();

//   uint16_t protocalNum;


// private:

//   AquaSimAddress m_source;       //!< Source address
//   AquaSimAddress m_destination;  //!< Destination address
//   uint8_t m_sequenceNumber;  //!< Sequence number
//   uint8_t m_ackNumber;       //!< ACK number
//   uint8_t m_pType;
//   uint8_t m_count;
//   uint16_t m_checksum;
