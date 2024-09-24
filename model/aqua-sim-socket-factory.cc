/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 Emmanuelle Laprise
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
 * Author: Emmanuelle Laprise <emmanuelle.laprise@bluekazoo.ca>
 */
#include "aqua-sim-socket-factory.h"

#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/aqua-sim-socket.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("AquaSimSocketFactory");

NS_OBJECT_ENSURE_REGISTERED (AquaSimSocketFactory);

TypeId 
AquaSimSocketFactory::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AquaSimSocketFactory")
    .SetParent<SocketFactory> ()
    .SetGroupName("Network");
  return tid;
}

AquaSimSocketFactory::AquaSimSocketFactory ()
{
  NS_LOG_FUNCTION (this);
}

Ptr<Socket> AquaSimSocketFactory::CreateSocket (void)
{
  NS_LOG_FUNCTION (this);
  Ptr<Node> node = GetObject<Node> ();
  Ptr<AquaSimSocket> socket = CreateObject<AquaSimSocket> ();
  socket->SetNode (node);
  return socket;
} 
} // namespace ns3
