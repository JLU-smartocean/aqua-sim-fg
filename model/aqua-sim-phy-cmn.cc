/*
 * aqua-sim-phy-cmn.h
 *
 *  Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#include <string>
#include <vector>

#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "aqua-sim-header.h"
#include "aqua-sim-trailer.h"
#include "aqua-sim-energy-model.h"
#include "aqua-sim-phy-cmn.h"

// Aqua Sim Phy Cmn

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AquaSimPhyCmn");
NS_OBJECT_ENSURE_REGISTERED(AquaSimPhyCmn);

AquaSimPhyCmn::AquaSimPhyCmn(void)
{
	NS_LOG_FUNCTION(this);
	m_channel.clear();
	freq = 25;
	initmodulations();
}

void AquaSimPhyCmn::initmodulations()
{

	ObjectFactory factory;
	factory.SetTypeId("ns3::AquaSimModulationOFDM");
	Ptr<AquaSimModulation> modulation = factory.Create<AquaSimModulation>();
	addModulation(modulation, 1);

	factory.Set("BPS",  IntegerValue(2));
	modulation = factory.Create<AquaSimModulation>();
	addModulation(modulation, 2);

	factory.Set("K",  IntegerValue(3));
	factory.Set("N",  IntegerValue(4));
	modulation = factory.Create<AquaSimModulation>();
	addModulation(modulation, 3);

	factory.Set("BPS",  IntegerValue(4));
	modulation = factory.Create<AquaSimModulation>();
	addModulation(modulation, 5);

	factory.Set("K",  IntegerValue(1));
	factory.Set("N",  IntegerValue(2));
	modulation = factory.Create<AquaSimModulation>();
	addModulation(modulation, 4);

}

AquaSimPhyCmn::~AquaSimPhyCmn(void)
{
	Dispose();
}

TypeId AquaSimPhyCmn::GetTypeId(void)
{
	static TypeId tid =
		TypeId("ns3::AquaSimPhyCmn")
		.SetParent<AquaSimPhy>()
		.AddConstructor<AquaSimPhyCmn>()
		.AddAttribute("Frequency", "The frequency, default is 25(khz).", 
			DoubleValue(25), MakeDoubleAccessor(&AquaSimPhyCmn::freq), 
			MakeDoubleChecker<double>());
	return tid;
}



void AquaSimPhyCmn::addModulation(Ptr<AquaSimModulation> modulation,
								  int modulationNum)
{

	if (modulation == NULL)
		NS_LOG_WARN("PHY-AddModulation NULL value for modulation "
					<< modulation << " or name " << modulationNum);
	else if (modulations.count(modulationNum) > 0)
		NS_LOG_WARN("PHY-Duplicate modulations");
	else
	{
		if (modulations.size() == 0)
		{
			this->modulationNum = modulationNum;
		}
		modulations[modulationNum] = modulation;
	}
}



/**
 * calculate transmission time of a packet of size pktsize
 * we consider the preamble
 */
Time AquaSimPhyCmn::calcTxTime(uint32_t pktSize, int modulationNum)
{
	NS_LOG_INFO("PHY-using modulation" << modulationNum);
	// double time = double(pktSize)/667 * 8 + 0.87;
	double time = modulations[modulationNum]->TxTime(pktSize);
	return Seconds(time);
}

Time AquaSimPhyCmn::calcTxTime(Ptr<Packet> pkt)
{
	int pktSize = pkt->GetSize();
	AquaSimTrailer ast;
	pkt->PeekTrailer(ast);
	return calcTxTime(pktSize - ast.GetSerializedSize(), ast.GetModeId());
}

Time AquaSimPhyCmn::calcTxTime(uint32_t pktSize)
{
	return calcTxTime(pktSize, modulationNum);
}

double AquaSimPhyCmn::CalcPktSize(double txTime, int modulationNum)
{
	// return Modulation(modName)->PktSize(txTime - Preamble());
	return 100;
}

bool AquaSimPhyCmn::matchPkt(Ptr<Packet> p){
	AquaSimPacketStamp pstamp;
	p->PeekHeader(pstamp);
	return AquaSimPhy::MatchFreq(pstamp.GetFreq(), this->freq);
}


std::vector<double> AquaSimPhyCmn::getBandWidth(){
	return std::vector<double>(1, 6);
}

std::vector<double> AquaSimPhyCmn::getFreq(){
	return std::vector<double>(1, freq);
}


Ptr<Packet> AquaSimPhyCmn::StampTxInfo(Ptr<Packet> p)
{
	AquaSimPacketStamp pstamp;
	pstamp.SetPt(10*log10(txPower) + 170.77);
	pstamp.SetTxRange(transRange);
	pstamp.SetFreq(freq);
	pstamp.SetBandWidth(6);
	p->AddHeader(pstamp);
	return p;
}




void AquaSimPhyCmn::DoDispose()
{
	NS_LOG_FUNCTION(this);
	for (std::map<int, Ptr<AquaSimModulation>>::iterator it =
			 modulations.begin();
		 it != modulations.end(); ++it)
		it->second = 0;
	AquaSimPhy::DoDispose();
}

int64_t AquaSimPhyCmn::AssignStreams(int64_t stream)
{
	NS_LOG_FUNCTION(this << stream);
	return 0;
}
