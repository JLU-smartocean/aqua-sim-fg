#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/aqua-sim-fg-module.h"
#include "ns3/applications-module.h"
#include "ns3/tap-bridge-module.h"
#include "ns3/log.h"
#include "ns3/callback.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TestTap");



int

main(int argc, char* argv[])

{

	double appStop = 30; 
	double simStop = 30000; //seconds
	int nodes = 11;
	int Nsend = 1;
	double traffic = 0.01;
	uint32_t m_dataRate = 128;
	uint32_t m_packetSize = 40;

	LogComponentEnable("AquaSimAloha", LOG_LEVEL_DEBUG);
	LogComponentEnable("TestTap", LOG_LEVEL_DEBUG);


	std::string mode = "ConfigureLocal";//add-zy
	std::string tapName = "thetap";//add-zy
	CommandLine cmd;



	cmd.AddValue("simStop", "Length of simulation", appStop);
	cmd.AddValue("nodes", "Amount of regular underwater nodes", nodes);
	cmd.AddValue("mode", "Mode setting of TapBridge", mode);//add-zy
	cmd.AddValue("tapName", "Name of the OS tap device", tapName);//add-zy
	cmd.Parse(argc, argv);



	//GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));//add by guo
	GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));//add by guo
	std::cout << "-----------Initializing simulation-----------\n";







	NodeContainer nodesCon;
	nodesCon.Create(nodes);

	AquaSimSocketHelper socketHelper;
	socketHelper.Install(nodesCon);


	AquaSimChannelHelper channel = AquaSimChannelHelper::Default();
	AquaSimHelper asHelper = AquaSimHelper::Default();
	asHelper.SetChannel(channel.Create());
	asHelper.SetMac("ns3::AquaSimPmacn");
	asHelper.SetRouting("ns3::AquaSimStaticRouting");
	asHelper.SetPhy("ns3::AquaSimPhyFDM", "transRange", DoubleValue(150));



	MobilityHelper mobility;
	NetDeviceContainer devices;
	Ptr<ListPositionAllocator> position = CreateObject<ListPositionAllocator>();
	Vector boundry = Vector(0, 0, 0);



	std::cout << "Creating Nodes\n";
	int addr = 100;
	int k = 1;
	for (NodeContainer::Iterator i = nodesCon.Begin(); i != nodesCon.End(); i++)
	{
		Ptr<AquaSimNetDevice> newDevice = CreateObject<AquaSimNetDevice>();
		position->Add(boundry);
		devices.Add(asHelper.Create(*i, newDevice));
		//newDevice->SetAddress(AquaSimAddress(addr + (k++)));
		NS_LOG_DEBUG("Node:" << AquaSimAddress::ConvertFrom(newDevice->GetAddress()).GetAsInt());
		boundry.x += 100;
	}




	AquaSimTapBridgeHelper tapBridge;
	tapBridge.SetAttribute("Mode", StringValue(mode));
	tapBridge.SetAttribute("DeviceName", StringValue(tapName));
	tapBridge.Install(nodesCon.Get(nodes - 1));



	mobility.SetPositionAllocator(position);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(nodesCon);



	AquaSimSocketAddress socket;
	socket.SetAllDevices();
	socket.SetDestinationAddress(devices.Get(nodes - 1)->GetAddress()); //Set dest to first sink (nodes+1 device)
	socket.SetProtocol(0);


	OnOffNdHelper app("ns3::AquaSimSocketFactory", Address(socket));
	app.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	app.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	app.SetAttribute("DataRate", DataRateValue(m_dataRate));
	app.SetAttribute("PacketSize", UintegerValue(m_packetSize));
	app.SetAttribute("Nsend", UintegerValue(Nsend));
	app.SetAttribute("traffic", DoubleValue(traffic));
	app.SetAttribute("stopSend", DoubleValue(appStop));



	ApplicationContainer apps = app.Install(nodesCon);
	apps.Start(Seconds(0.5));//  apps.Start (Seconds (0.5));
	apps.Stop(Seconds(simStop));



	Packet::EnablePrinting(); //for debugging purposes
	std::cout << "-----------Running Simulation-----------\n";
	Simulator::Stop(Seconds(simStop));
	Simulator::Run();
	Simulator::Destroy();
	std::cout << "fin.\n";



}

