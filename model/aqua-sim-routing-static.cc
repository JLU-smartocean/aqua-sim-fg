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

#include "aqua-sim-routing-static.h"
#include "aqua-sim-header.h"
#include "aqua-sim-trailer.h"
#include "aqua-sim-address.h"

#include "ns3/string.h"
#include "ns3/log.h"

#include <cstdio>
#include <iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

#include "aqua-sim-header-mac.h"
#include "aqua-sim-pt-tag.h"
#include "aqua-sim-header-transport.h"
#include "aqua-sim-header-routing.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AquaSimStaticRouting");
NS_OBJECT_ENSURE_REGISTERED(AquaSimStaticRouting);

AquaSimStaticRouting::AquaSimStaticRouting() :
		m_hasSetRouteFile(false), m_hasSetNode(false) {
	NS_LOG_FUNCTION(this);

}

AquaSimStaticRouting::AquaSimStaticRouting(char *routeFile) :
		m_hasSetNode(false) {
}

AquaSimStaticRouting::~AquaSimStaticRouting() {
}

TypeId AquaSimStaticRouting::GetTypeId() {
	static TypeId tid = TypeId("ns3::AquaSimStaticRouting").SetParent<
			AquaSimRouting>().AddConstructor<AquaSimStaticRouting>();
	return tid;
}

int64_t AquaSimStaticRouting::AssignStreams(int64_t stream) {
	NS_LOG_FUNCTION(this << stream);
	return 0;
}

void AquaSimStaticRouting::SetRouteTable(const char *routeFile) {
	m_hasSetRouteFile = false;
	strcpy(m_routeFile, routeFile);
	ReadRouteTable(m_routeFile);
}

/*
 * Load the static routing table in filename
 *
 * @param filename   the file containing routing table
 * */
void AquaSimStaticRouting::ReadRouteTable(const char *filename) {
	NS_LOG_FUNCTION(this);

	std::string str;
	std::ifstream ifs(filename);
	int current_node, dst_node, nxt_hop;
	if (!ifs) {
		std::cout << "open file fail!" << std::endl;
	}
	while (getline(ifs, str)) {
		std::istringstream istr(str);
		istr >> current_node;
		istr >> dst_node;
		istr >> nxt_hop;
		if (AquaSimAddress::ConvertFrom(m_device->GetAddress()).GetAsInt()
				== current_node) {
			m_rTable[AquaSimAddress(dst_node)] = AquaSimAddress(nxt_hop);
		}

	}
	ifs.close();
}

bool AquaSimStaticRouting::TxProcess(Ptr<Packet> p, const Address &dest, uint16_t protocolNumber) {
	/**
	 * 路由接受函数的判断逻辑如下:
	 * 1.加载路由表
	 * 2.判断是否为本节点数据	是:接收并向上传递
	 * 3.找到下一跳，判断是否死循环	是:不处理该包
	 * 4.更新头部信息
	 * 5.发包
	 */
	// dest如果为???,说明包从下方来，不做添加头部处理
	RoutingHeader routingHeader;
	routingHeader.SetNumForwards((uint16_t)0);
	routingHeader.SetSA(AquaSimAddress::ConvertFrom(m_device->GetAddress()));
	routingHeader.SetDA(AquaSimAddress::ConvertFrom(dest));
	p->AddHeader(routingHeader);

	return Recv(p, dest, protocolNumber);
}

bool AquaSimStaticRouting::Recv(Ptr<Packet> p, const Address &dest,
	uint16_t protocolNumber) {
	/**
	 * 路由接受函数的判断逻辑如下:
	 * 1.加载路由表
	 * 2.判断是否为本节点数据	是:接收并向上传递
	 * 3.找到下一跳，判断是否死循环	是:不处理该包
	 * 4.更新头部信息
	 * 5.发包
	 */
	// dest如果为???,说明包从下方来，不做添加头部处理
	AquaSimTrailer ast;
	RoutingHeader rth;
	p->RemoveTrailer(ast);

	if ((m_rTable.empty()) == true) {
		SetRouteTable("1.txt");
	}
	if (AmIDst(p)) {
		std::cout<<"Routing receive a packet"<<std::endl;
		p->RemoveHeader(rth);
		p->AddTrailer(ast);
		return SendUp(p);
	}
	if (IsDeadLoop(p)) {
		p = 0;	// 死循环，抛弃该packet
		std::cout<<"Routing is dead loop\n"<<std::endl;

		return false;
	}

	//find the next hop and forward
	AquaSimAddress next_hop = FindNextHop(p);
	std::cout << "Route Algorithm start to find next_hop,next hop is Node " << next_hop.GetAsInt() << std::endl;
	//increase the number of forwards
	p->RemoveHeader(rth);
	rth.SetNumForwards(uint16_t(rth.GetNumForwards() + 1));
	p->AddHeader(rth);
	ast.SetNextHop(next_hop);
	p->AddTrailer(ast);
	
	return SendDown(p, next_hop, Seconds(0.0));
}

/*
 * @param p   a packet
 * @return    the next hop to route packet p
 * */
AquaSimAddress AquaSimStaticRouting::FindNextHop(const Ptr<Packet> p) {
	// AquaSimHeader ash;
	// p->PeekHeader(ash);
	RoutingHeader rth;
	p->PeekHeader(rth);
	std::map<AquaSimAddress, AquaSimAddress>::iterator it = m_rTable.find(
			rth.GetDA());
	return it == m_rTable.end() ? AquaSimAddress::GetBroadcast() : it->second;
}

}  // namespace ns3
