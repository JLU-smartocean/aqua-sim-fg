/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 University of Connecticut
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
 * Author: Robert Martin <robert.martin@engr.uconn.edu>
 */

#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/double.h"
#include "ns3/config.h"

#include "ns3/aqua-sim-net-device.h"
#include "ns3/aqua-sim-propagation.h"
#include "ns3/aqua-sim-noise-generator.h"
#include "ns3/aqua-sim-address.h"
#include "ns3/application.h"
#include "ns3/aqua-sim-signal-cache.h"
#include "ns3/aqua-sim-sinr-checker.h"

#include "aqua-sim-helper.h"

#include <sstream>
#include <string>
#include<iostream>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AquaSimHelper");

static void AsciiPhyRxEvent(std::ostream *os, std::string context,
		Ptr<Packet> pkt, double noise) {
	*os << "r " << Simulator::Now().GetSeconds() << " " << context << " "
			<< *pkt << std::endl;
}

static void AsciiPhyTxEvent(std::ostream *os, std::string context,
		Ptr<Packet> pkt, double noise) {
	*os << "t " << Simulator::Now().GetSeconds() << " " << context << " "
			<< *pkt << std::endl;
}

AquaSimChannelHelper::AquaSimChannelHelper() {
}

AquaSimChannelHelper AquaSimChannelHelper::Default(void) {
	AquaSimChannelHelper channelHelper;
	channelHelper.SetPropagation("ns3::AquaSimRangePropagation");
	channelHelper.SetNoiseGenerator("ns3::AquaSimConstNoiseGen");
	channelHelper.SetChannel("ns3::AquaSimChannel");
	return channelHelper;
}

void AquaSimChannelHelper::SetPropagation(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_propagation = factory;
}

void AquaSimChannelHelper::SetNoiseGenerator(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_noiseGen = factory;
}

void AquaSimChannelHelper::SetChannel(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_channel = factory;
}

Ptr<AquaSimChannel> AquaSimChannelHelper::Create(void) const {
	//Ptr<AquaSimChannel> channel = CreateObject<AquaSimChannel> ();
	Ptr<AquaSimChannel> channel = m_channel.Create<AquaSimChannel>();
	Ptr<AquaSimPropagation> prop = m_propagation.Create<AquaSimPropagation>();
	Ptr<AquaSimNoiseGen> noise = m_noiseGen.Create<AquaSimNoiseGen>();
	channel->SetPropagation(prop);
	channel->SetNoiseGenerator(noise);
	return channel;
}

/*
 int64_t
 AquaSimChannelHelper::AssignStreams (Ptr<AquaSimChannel> c, int64_t stream)
 {
 return c->AssignStreams (stream);	//this needs to be implemented or removed...
 }
 */

AquaSimHelper::AquaSimHelper() {
	m_channel.clear();
	/*
	 * Protocol prefix setting
	 */
	m_phy.SetTypeId("ns3::AquaSimPhyCmn");
	m_phy.Set("RXThresh", DoubleValue(0));
	m_phy.Set("txPower", DoubleValue(80));
	m_phy.Set("transRange", DoubleValue(1500.0));
	m_signalCache.SetTypeId("ns3::AquaSimSignalCacheRange");
	m_mac.SetTypeId("ns3::AquaSimBroadcastMac");
	m_routing.SetTypeId("ns3::AquaSimStaticRouting");
	// m_transport.SetTypeId("ns3::AquaSimTransport");	// be commented out because don't used transport layer -- spinach

	m_energyM.SetTypeId("ns3::AquaSimEnergyModel");
	m_sync.SetTypeId("ns3::AquaSimSync");
	m_localization.SetTypeId("ns3::AquaSimRBLocalization");
	m_sinrChecker.SetTypeId("ns3::AquaSimThresholdSinrChecker");
	m_attacker = false;
}

AquaSimHelper AquaSimHelper::Default() {
	AquaSimHelper asHelper;

	//populate the default case with child classes for mac/routing

	return asHelper;
}

void AquaSimHelper::SetChannel(Ptr<AquaSimChannel> channel) {
	NS_ASSERT_MSG(channel, "provided channel pointer is null");
	m_channel.push_back(channel);
}

Ptr<AquaSimChannel> AquaSimHelper::GetChannel(int channelId) {
	return m_channel.at(channelId);
}

void AquaSimHelper::SetAttacker(bool attacker) {
	m_attacker = attacker;
}

void AquaSimHelper::SetPhy(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_phy = factory;
}

void AquaSimHelper::SetMac(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_mac = factory;
}

void AquaSimHelper::SetRouting(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;

	factory.SetTypeId(type);
	// std::cout<<factory.GetTypeId();
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_routing = factory;
}

void AquaSimHelper::SetTransport(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;

	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_transport = factory;
}

void AquaSimHelper::SetEnergyModel(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_energyM = factory;
}

void AquaSimHelper::SetSync(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_sync = factory;
}

void AquaSimHelper::SetLocalization(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_localization = factory;
}

void AquaSimHelper::SetAttackModel(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_attackM = factory;
}


void AquaSimHelper::SetSignalCache(std::string type, std::string n0,
		const AttributeValue &v0, std::string n1, const AttributeValue &v1,
		std::string n2, const AttributeValue &v2, std::string n3,
		const AttributeValue &v3, std::string n4, const AttributeValue &v4,
		std::string n5, const AttributeValue &v5, std::string n6,
		const AttributeValue &v6, std::string n7, const AttributeValue &v7) {
	ObjectFactory factory;
	factory.SetTypeId(type);
	factory.Set(n0, v0);
	factory.Set(n1, v1);
	factory.Set(n2, v2);
	factory.Set(n3, v3);
	factory.Set(n4, v4);
	factory.Set(n5, v5);
	factory.Set(n6, v6);
	factory.Set(n7, v7);
	m_signalCache = factory;
}

void AquaSimHelper::SetRouteAttribute(std::string name,
		const AttributeValue &value) {
	m_routing.Set(name, value);
}

void AquaSimHelper::SetMacAttribute(std::string name,
		const AttributeValue &value) {
	m_mac.Set(name, value);
}

void AquaSimHelper::SetPhyAttribute(std::string name,
		const AttributeValue &value) {
	m_phy.Set(name, value);
}

Ptr<AquaSimNetDevice> AquaSimHelper::Create(Ptr<Node> node,
		Ptr<AquaSimNetDevice> device) {
	Ptr<AquaSimPhy> phy = m_phy.Create<AquaSimPhy>();
	Ptr<AquaSimMac> mac = m_mac.Create<AquaSimMac>();
	Ptr<AquaSimSignalCache> signalCache = m_signalCache.Create<AquaSimSignalCache>();
	phy->SetSignalCache(signalCache);
	
	Ptr<AquaSimRouting> routing;
	
	//Ptr<AquaSimTransport> transport ;
	Ptr<AquaSimEnergyModel> energyM = m_energyM.Create<AquaSimEnergyModel>();
	//Ptr<AquaSimSync> sync = m_sync.Create<AquaSimSync>();
	//Ptr<AquaSimLocalization> loc = m_localization.Create<AquaSimLocalization>();
	Ptr<AquaSimThresholdSinrChecker> sinr = m_sinrChecker.Create<
			AquaSimThresholdSinrChecker>();

	device->SetPhy(phy);
	device->SetMac(mac);

	device->ConnectLayers();

	NS_ASSERT(!m_channel.empty());
	device->SetChannel(m_channel);
	device->SetEnergyModel(energyM);
	device->SetAddress(AquaSimAddress::Allocate());
	device->GetPhy()->SetSinrChecker(sinr);

	if (m_attacker) {
		Ptr<AquaSimAttackModel> attackM =
				m_attackM.Create<AquaSimAttackModel>();
		device->SetAttackModel(attackM);
	}
	routing = m_routing.Create<AquaSimRouting>();
	
	// std::cout<<routing->GetTypeId();
	device->SetRouting(routing);
	
	//transport = m_transport.Create<AquaSimTransport>();
	//device->SetTransport(transport);
	node->AddDevice(device);

	return device;
}

Ptr<AquaSimNetDevice> AquaSimHelper::CreateWithoutRouting(Ptr<Node> node,
		Ptr<AquaSimNetDevice> device) {
	Ptr<AquaSimPhy> phy = m_phy.Create<AquaSimPhy>();
	Ptr<AquaSimMac> mac = m_mac.Create<AquaSimMac>();
	Ptr<AquaSimEnergyModel> energyM = m_energyM.Create<AquaSimEnergyModel>();
	//Ptr<AquaSimSync> sync = m_sync.Create<AquaSimSync>();
	//Ptr<AquaSimLocalization> loc = m_localization.Create<AquaSimLocalization>();
	Ptr<AquaSimThresholdSinrChecker> sinr = m_sinrChecker.Create<
			AquaSimThresholdSinrChecker>();

	device->SetPhy(phy);
	device->SetMac(mac);
	//device->SetMac(mac,sync,loc);
	device->ConnectLayers();

	NS_ASSERT(!m_channel.empty());
	device->SetChannel(m_channel);

	device->SetEnergyModel(energyM);
	device->SetAddress(AquaSimAddress::Allocate());
	device->GetPhy()->SetSinrChecker(sinr);

	if (m_attacker) {
		Ptr<AquaSimAttackModel> attackM =
				m_attackM.Create<AquaSimAttackModel>();
		device->SetAttackModel(attackM);
	}

	node->AddDevice(device);

	return device;
}

void AquaSimHelper::EnableAscii(std::ostream &os, uint32_t nodeid,
		uint32_t deviceid) {
	Packet::EnablePrinting();
	std::ostringstream oss;

	oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid
			<< "/$ns3::AquaSimNetDevice/Phy/Rx";
	Config::Connect(oss.str(), MakeBoundCallback(&AsciiPhyRxEvent, &os));

	oss.str("");

	oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid
			<< "/$ns3::AquaSimNetDevice/Phy/Tx";
	Config::Connect(oss.str(), MakeBoundCallback(&AsciiPhyTxEvent, &os));
}

void AquaSimHelper::EnableAscii(std::ostream &os, NetDeviceContainer c) {
	for (NetDeviceContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
		EnableAscii(os, (*i)->GetNode()->GetId(), (*i)->GetIfIndex());
	}
}

void AquaSimHelper::EnableAscii(std::ostream &os, NodeContainer n) {
	NetDeviceContainer devs;
	for (NodeContainer::Iterator i = n.Begin(); i != n.End(); ++i) {
		Ptr<Node> node = *i;
		for (uint32_t j = 0; j < node->GetNDevices(); ++j) {
			devs.Add(node->GetDevice(j));
		}
	}
	EnableAscii(os, devs);
}

void AquaSimHelper::EnableAsciiAll(std::ostream &os) {
	EnableAscii(os, NodeContainer::GetGlobal());
}

uint64_t AssignStreams(NetDeviceContainer c, int64_t stream) {
	int64_t currentStream = stream;
	Ptr<NetDevice> device;
	for (NetDeviceContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
		device = (*i);
		Ptr<AquaSimNetDevice> asDevice = DynamicCast<AquaSimNetDevice>(device);
		if (asDevice) {
			currentStream += asDevice->GetPhy()->AssignStreams(currentStream);
			currentStream += asDevice->GetMac()->AssignStreams(currentStream);
		}
	}
	return (currentStream - stream);
}

}  //namespace ns3
