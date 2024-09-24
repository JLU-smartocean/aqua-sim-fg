
/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 *
 * Date:2021.4.7
 * Author: chenhao <chenhao2118@mails.jlu.edu.cn>
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/aqua-sim-fg-module.h"
#include "ns3/applications-module.h"
#include "ns3/log.h"
#include "ns3/callback.h"
#include <fstream>
#include <unistd.h>
#include <iostream>

/*
 * GOALMAC
 *
 * String topology:
 * S---->  N  -----> N -----> N -----> D
 *                   D
 *                  / \
 *                 /   \
 *                F     F
 *               /|\   /|\
 *              / | \ / | \
 *             /  |  F  |  \
 *            /   | / \ |  S
 *           S    S    \| /
 *                      S
 * 
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Goalmac");

int main(int argc, char *argv[])
{
	double appStop = 5000; //seconds
	int nodel1 = 50;
	int nodel2 = 0;
	int nodel3 = 1;
	int Nsend = 50;
	int nodes = nodel1 + nodel2 + nodel3;
	double traffic = 0.02;
	uint32_t m_packetSize = 600;
	double transrange = 2000;
	uint32_t m_dataRate = 80;

	LogComponentEnable("AquaSimHHVBF", LOG_LEVEL_DEBUG);
	LogComponentEnable("AquaSimGoalN", LOG_LEVEL_DEBUG);
	LogComponentEnable("OnOffNDApplication", LOG_LEVEL_DEBUG);
    //LogComponentEnable("AquaSimNetDevice", LOG_LEVEL_DEBUG);
	//LogComponentEnable("AquaSimPhyCmn", LOG_LEVEL_DEBUG);
	//LogComponentEnable("AquaSimSimplePropagation", LOG_LEVEL_DEBUG);
	//LogComponentEnable("AquaSimSignalCache", LOG_LEVEL_DEBUG);

	//to change on the fly
	CommandLine cmd;
	cmd.AddValue("simStop", "Length of simulation", appStop);
	cmd.AddValue("nodes", "Amount of regular underwater nodes", nodes);
	cmd.Parse(argc, argv);

	// GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));

	std::cout << "-----------Initializing simulation-----------\n";

	NodeContainer nodesCon;
	nodesCon.Create(nodes);

	AquaSimSocketHelper socketHelper;
	socketHelper.Install(nodesCon);

	AquaSimChannelHelper channel = AquaSimChannelHelper::Default();
	//channel.SetPropagation("ns3::AquaSimSimplePropagation");
	channel.SetPropagation("ns3::AquaSimBellhopPropagation");

	AquaSimHelper asHelper = AquaSimHelper::Default();
	asHelper.SetChannel(channel.Create());
	Vector sinkPos(4500,4500,5);
	//asHelper.SetRouting("ns3::AquaSimStaticRouting");
	//asHelper.SetRouting("ns3::AquaSimHHVBF");
	asHelper.SetRouting("ns3::AquaSimVBF", "TargetPos", Vector3DValue(sinkPos));
	//asHelper.SetMac("ns3::AquaSimGoalN");
	asHelper.SetMac("ns3::AquaSimBroadcastMac");
	asHelper.SetPhy("ns3::AquaSimPhyCmn", "transRange", DoubleValue(transrange));
	asHelper.SetPhyAttribute("txPower", DoubleValue(8));
	//asHelper.SetSignalCache("ns3::AquaSimSignalCacheSINR");
    
	MobilityHelper mobility;
	NetDeviceContainer devices;
	Ptr<ListPositionAllocator> position = CreateObject<ListPositionAllocator>();
	//Vector boundry = Vector((rand() % 4001),(rand() % 4001),0);
	//Vector boundry = Vector((1000), (1010), 100); //2800/100

	
	std::ofstream outfile;
	outfile.open("nodepos.cfg",std::ios::out);

	int num = 1;
	Vector boundry;
	std::cout << "Creating Nodes\n";
	for (NodeContainer::Iterator i = nodesCon.Begin(); i != nodesCon.End(); i++)
	{
		if(num <= nodel1){
			boundry.x=rand()%5000;
			boundry.y=rand()%5000;
			boundry.z=rand()%4800;
		}
		else if(num <= nodel1 + nodel2){
			boundry.x=rand()%5000;
			boundry.y=rand()%5000;
			boundry.z=rand()%4800;
		}
		else{
			boundry.x=4500;
			boundry.y=4500;
			boundry.z=5;
		}
		outfile << num << " " << boundry.x << " " << boundry.y << " " << boundry.z << "\n";
		num++;

		Ptr<AquaSimNetDevice> newDevice = CreateObject<AquaSimNetDevice>();
		position->Add(boundry);
		devices.Add(asHelper.Create(*i, newDevice));
		NS_LOG_DEBUG(
			"Node:" << newDevice->GetAddress() << " position(x):" << boundry.x << "  position(y):" << boundry.y << "  position(z):" << boundry.z);
		
	}
	outfile.close();
	std::cout << "Creating Nodes End\n";

	mobility.SetPositionAllocator(position);
	mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Mode",
							  StringValue("Time"), "Time", StringValue("2s"), "Speed",
							  //StringValue("ns3::UniformRandomVariable[Min=0|Max=0]"), "Bounds",
							  StringValue("ns3::UniformRandomVariable[Min=0|Max=0]"), "Bounds",
							  StringValue("0|100000|0|100000"));
	
	mobility.Install(nodesCon);

	//PacketSocketAddress socket;
	AquaSimSocketAddress socket;
	socket.SetAllDevices();
	socket.SetDestinationAddress(devices.Get(nodes - 1)->GetAddress()); //Set dest to first sink (nodes+1 device)
	socket.SetProtocol(0);

	//OnOffHelper app ("ns3::PacketSocketFactory", Address (socket));
	OnOffNdHelper app("ns3::AquaSimSocketFactory", Address(socket));
	//app.SetAttribute("OnTime",
					 //StringValue("ns3::ExponentialRandomVariable[Mean=100|Bound=0.0]"));
	app.SetAttribute("OffTime",
					 StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	// app.SetAttribute ("Interval", StringValue ("ns3::ExponentialRandomVariable[Mean=0.01|Bound=0.0]"));
	app.SetAttribute("DataRate", DataRateValue(m_dataRate));
	app.SetAttribute("PacketSize", UintegerValue(m_packetSize));
	app.SetAttribute("Nsend", UintegerValue(Nsend));
	app.SetAttribute("traffic", DoubleValue(traffic));
	app.SetAttribute("stopSend", DoubleValue(appStop));



	ApplicationContainer apps = app.Install(nodesCon);
	apps.Start(Seconds(5));
	apps.Stop(Seconds(appStop * 10));


	Packet::EnablePrinting(); //for debugging purposes
	std::cout << "-----------Running Simulation-----------\n";
	Simulator::Stop(Seconds(appStop * 10));
	Simulator::Run();
	Simulator::Destroy();

	std::cout << "fin.\n";
	return 0;
}
