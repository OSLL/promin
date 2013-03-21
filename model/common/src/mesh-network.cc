/*
 * Copyright 2011-2013  Open Source and Linux Lab
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

#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include <ns3/wifi-module.h>
#include <ns3/mobility-module.h>
#include <ns3/mesh-module.h>
#include <ns3/network-module.h>

#include "mesh-network.h"
#include "mxml-writer.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MeshNetwork");

static const char* PROTOCOL_STACK = "ns3::Dot11sStack";
static const std::string TRACE_DIR = "traces/";

MeshNetwork::MeshNetwork(const std::string& networkName, size_t nodesNumber,
                         double totalTime, double packetInterval,
                         uint16_t packetSize) :
  m_networkName(networkName), m_nodesNumber(nodesNumber),
  m_totalTime(totalTime), m_packetInterval(packetInterval),
  m_packetSize(packetSize), m_pcap(false), m_xml(true)
{
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_FUNCTION);
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_FUNCTION);
  LogComponentEnable("MeshNetwork", LOG_LEVEL_DEBUG);

  mkdir(TRACE_DIR.c_str(), 0755);
}

void
MeshNetwork::Configure(int argc, char * argv[])
{
  CommandLine cmd;

  cmd.AddValue("time", "Simulation time, seconds [100 s]", m_totalTime);
  cmd.AddValue("packet-interval", "Interval between packets in UDP ping, seconds [0.001 s]",
      m_packetInterval);
  cmd.AddValue("packet-size", "Size of packets in UDP ping", m_packetSize);
  cmd.AddValue("pcap", "Enable PCAP traces on interfaces. [0]", m_pcap);
  cmd.AddValue("xml", "Enable XML traces on interfaces. [0]", m_xml);

  cmd.Parse(argc, argv);

  NS_LOG_DEBUG("Simulation time: " << m_totalTime << " s");
}

void
MeshNetwork::CreateNodes()
{
  m_nodes.Create(m_nodesNumber);
}

void
MeshNetwork::InstallDevices()
{
  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
  YansWifiChannelHelper channelHelper = YansWifiChannelHelper::Default();

  wifiPhyHelper.SetChannel(channelHelper.Create());

  m_meshHelper = MeshHelper::Default();
  m_meshHelper.SetStackInstaller(PROTOCOL_STACK);
  m_devices = m_meshHelper.Install(wifiPhyHelper, m_nodes);
  m_meshHelper.SetMacType("RandomStart", TimeValue(Seconds(0.1)));

  if (m_pcap)
    wifiPhyHelper.EnablePcapAll(TRACE_DIR + m_networkName);
}

void
MeshNetwork::InstallInternetStack()
{
  InternetStackHelper internetStack;
  internetStack.Install(m_nodes);
  Ipv4AddressHelper address;

  address.SetBase("10.1.1.0", "255.255.255.0");
  m_interfaces = address.Assign(m_devices);
}

void
MeshNetwork::ConnectMxmlWriter()
{
  std::map<MxmlWriter::Option, std::string> config;

  config[MxmlWriter::FILE_NAME] = TRACE_DIR + "trace-" + m_networkName + ".mxml";
  config[MxmlWriter::PROGRAM_NAME] = m_networkName;
  config[MxmlWriter::PROCESS_NAME] = "MeshNetwork test";
  config[MxmlWriter::PROCESS_INSTANCE] = "1";

  MxmlWriter::GetInstance().Connect(config);
}

void
MeshNetwork::ActivateAnimator()
{
  m_animator.reset(new ns3::AnimationInterface(TRACE_DIR + m_networkName + "-netanim.xml"));
  //m_animator.EnablePacketMetadata(true); - fails with notimplemented
}

void
MeshNetwork::Report()
{
  if (m_xml)
    {
      size_t n(0);

      for (NetDeviceContainer::Iterator devIter = m_devices.Begin();
           devIter != m_devices.End(); ++devIter, ++n)
        {
          std::ostringstream os;

          os << TRACE_DIR << m_networkName << "-report-" << n << ".xml";
          std::cout << "Printing mesh point client device #" << n
                    << " diagnostics to " << os.str() << "\n";

          std::ofstream of;

          of.open(os.str().c_str());

          if (!of.is_open())
            {
              std::cerr << "Error: Can't open file " << os.str() << "\n";
              return;
            }

          m_meshHelper.Report(*devIter, of);
          of.close();
        }
    }
}

void
MeshNetwork::PrintNodePositions() const
{
  for (NodeContainer::Iterator nodeIter = m_nodes.Begin();
      nodeIter != m_nodes.End(); ++nodeIter)
    {
      Ptr<MobilityModel> model = (*nodeIter)->GetObject<MobilityModel>();

      NS_LOG_DEBUG("Position of node " << ": " << model->GetPosition ());
      NS_LOG_DEBUG("Velocity of node " << ": " << model->GetVelocity ());
    }
}
