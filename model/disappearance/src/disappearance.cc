/*
 * Copyright 2011-2013 Open Source and Linux Lab
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * The advertising clause requiring mention in adverts must never be included.
 */

/* PROJ: OSLL/promin  */

#include <ns3/mesh-module.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>

#include "disappearance.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Disappearance");

Disappearance::Disappearance() :
    MeshNetwork("dispersion"),
    m_initialRadius(0.0), m_movingSpeed(1200)
{
  LogComponentEnable("Disappearance", LOG_LEVEL_DEBUG);
}

void
Disappearance::Configure(int argc, char * argv[])
{
  MeshNetwork::Configure(argc, argv);

  CommandLine cmd;

  cmd.AddValue("initial-radius", "Initial radius. [100 m]", m_initialRadius);
  cmd.AddValue("speed", "Moving speed. [10 mps]", m_movingSpeed);

  cmd.Parse(argc, argv);
}

void
Disappearance::SetupMobility()
{
  int i = 1;
  NodeContainer::Iterator nodeIter = m_nodes.Begin();

  nodeIter++; // The very first will be static

  for (; nodeIter != m_nodes.End(); ++nodeIter, ++i)
    {
      int xDirection = i % 2 == 0 ? 1 : -1;
      int yDirection = i < 2 ? 1 : -1;

      Vector position(xDirection * m_initialRadius, yDirection * m_initialRadius, 0);
      Vector velocity(xDirection * m_movingSpeed, yDirection * m_movingSpeed, 0);

      MobilityHelper mobility;
      mobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX",
                                    DoubleValue(xDirection * m_initialRadius), "MinY",
                                    DoubleValue(yDirection * m_initialRadius));

      mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
      mobility.Install(*nodeIter);
      Ptr<ConstantVelocityMobilityModel> model = (*nodeIter)->GetObject<
          ConstantVelocityMobilityModel>();
      model->SetVelocity(velocity);
    } // for nodeIter in movingNodes

  MobilityHelper centralMobility;

  centralMobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX",
                                       DoubleValue(0), "MinY", DoubleValue(0));
  centralMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  centralMobility.Install(m_nodes.Get(0));
  PrintNodePositions();
}

void
Disappearance::InstallApplications()
{
  UdpEchoServerHelper serverHelper(9);
  ApplicationContainer serverApps = serverHelper.Install(m_nodes.Get(0));

  serverApps.Start(Seconds(0.0));
  serverApps.Stop(Seconds(m_totalTime));

  UdpEchoClientHelper clientHelper(m_interfaces.GetAddress(0), 9);

  NS_LOG_DEBUG("Server address: " << m_interfaces.GetAddress (0));

  clientHelper.SetAttribute("MaxPackets", UintegerValue((uint32_t) (m_totalTime * (1 / m_packetInterval))));
  clientHelper.SetAttribute("Interval", TimeValue(Seconds(m_packetInterval)));
  clientHelper.SetAttribute("PacketSize", UintegerValue(m_packetSize));

  NodeContainer movingNodes;

  for(size_t i = 1; i < m_nodesNumber; ++i)
    movingNodes.Add(m_nodes.Get(i));

  ApplicationContainer clientApps = clientHelper.Install(movingNodes);

  clientApps.Start(Seconds(0.0));
  clientApps.Stop(Seconds(m_totalTime));
}

int
Disappearance::Run()
{
  ns3::Packet::EnablePrinting(); //TODO where to put this thing?

  CreateNodes();
  InstallDevices();
  SetupMobility();
  InstallInternetStack();
  InstallApplications();
  Simulator::Schedule(Seconds(m_totalTime), &MeshNetwork::Report, this);
  Simulator::Stop(Seconds(m_totalTime));

  ConnectMxmlWriter();
  ActivateAnimator();

  Simulator::Run();
  PrintNodePositions();
  Simulator::Destroy();

  return 0;
}
