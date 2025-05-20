from ns import ns

Nnode = 2
Nsend = 1
packetSize = 30
traffic = 0.03
appStop = 10000

ns.core.LogComponentEnable("UdpEchoClientApplication", ns.core.LOG_LEVEL_DEBUG)

nodes = ns.network.NodeContainer()
nodes.Create(Nnode)

socketHelper = ns.aqua_sim_tg.AquaSimSocketHelper()
socketHelper.Install(nodes)

channel = ns.aqua_sim_tg.AquaSimChannelHelper.Default()
#channel.SetPropagation("ns3::AquaSimSimplePropagation")

asHelper = ns.aqua_sim_tg.AquaSimHelper.Default()
asHelper.SetChannel(channel.Create())
asHelper.SetRouting("ns3::AquaSimStaticRouting")
asHelper.SetMac("ns3::AquaSimAloha")
#asHelper.SetSignalCache("ns3::AquaSimSignalCacheSINR")
#asHelper.SetPhy("ns3::AquaSimPhyCmn")
#asHelper.SetPhy("ns3::AquaSimPhyFDM")
#asHelper.SetPhyAttribute("transRange", DoubleValue(1500.0))


mobility = ns.mobility.MobilityHelper()
devices = ns.network.NetDeviceContainer()
position = ns.core.CreateObject("ListPositionAllocator")

boundry = ns.core.Vector(1000, 1000, 100)
for i in range(Nnode):
    newDevice = ns.core.CreateObject("AquaSimNetDevice")
    position.Add(boundry)
    devices.Add(asHelper.Create(nodes.Get(i), newDevice))
    boundry.y = boundry.y + 1000


mobility.SetPositionAllocator(position)
mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Mode",
							  ns.core.StringValue("Time"), "Time", ns.core.StringValue("2s"), "Speed",
							  ns.core.StringValue("ns3::UniformRandomVariable[Min=0|Max=0]"), "Bounds",
							  ns.core.StringValue("0|100000|0|100000"))
mobility.Install(nodes)


socket = ns.aqua_sim_tg.AquaSimSocketAddress()
socket.SetAllDevices()
socket.SetDestinationAddress(devices.Get(1).GetAddress())
socket.SetProtocol(0)

app = ns.aqua_sim_tg.OnOffNdHelper("ns3::AquaSimSocketFactory", socket.ConvertTo())
app.SetAttribute("OffTime",
					 ns.core.StringValue("ns3::ConstantRandomVariable[Constant=0]"))
app.SetAttribute("PacketSize", ns.core.UintegerValue(packetSize))
app.SetAttribute("Nsend", ns.core.UintegerValue(Nsend))
app.SetAttribute("traffic", ns.core.DoubleValue(traffic))
app.SetAttribute("stopSend", ns.core.DoubleValue(appStop))

apps = app.Install(nodes)
apps.Start(ns.core.Seconds(5))
apps.Stop(ns.core.Seconds(appStop * 2))

ns.network.Packet.EnablePrinting()

ns.core.Simulator.Stop(ns.core.Seconds(appStop * 2))
ns.core.Simulator.Run()
ns.core.Simulator.Destroy()

