/*
 * Copyright 2011-2012  Open Source and Linux Lab 
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

#include <iostream>
#include <sstream>
#include <fstream>

#include <ns3/wifi-module.h>
#include <ns3/mesh-module.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>

#include "dispersion.h"
#include "mxml-writer.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Dispersion");

Dispersion::Dispersion () :
	m_initialRadius (0.0),
	m_movingSpeed (1200),
  m_totalTime (1.0),
  m_randomStart (0.1),
  m_packetInterval (0.1),
  m_packetSize (1024),
  m_stack ("ns3::Dot11sStack"),
  m_root ("ff:ff:ff:ff:ff:ff"),
  m_pcap (true),
  m_xml (true) {
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_FUNCTION);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_FUNCTION);
  LogComponentEnable ("Dispersion", LOG_LEVEL_DEBUG);
}

void
Dispersion::Configure (int argc, char * argv[]) {
  CommandLine cmd;

  cmd.AddValue ("initial-radius", "Initial radius. [100 m]", m_initialRadius);
  cmd.AddValue ("speed", "Moving speed. [10 mps]", m_movingSpeed);
  cmd.AddValue ("time",  "Simulation time, seconds [100 s]", m_totalTime);
  cmd.AddValue ("start",  "Maximum random start delay, seconds. [0.1 s]", m_randomStart);
  cmd.AddValue ("packet-interval", 
  		"Interval between packets in UDP ping, seconds [0.001 s]", m_packetInterval);
  cmd.AddValue ("packet-size",  "Size of packets in UDP ping", m_packetSize);
  cmd.AddValue ("stack",  "Type of protocol stack. ns3::Dot11sStack by default", m_stack);
  cmd.AddValue ("root", "Mac address of root mesh point in HWMP", m_root);
  cmd.AddValue ("pcap",   "Enable PCAP traces on interfaces. [0]", m_pcap);
  cmd.AddValue ("xml",   "Enable XML traces on interfaces. [0]", m_xml);

  cmd.Parse (argc, argv);
  NS_LOG_DEBUG ("Simulation time: " << m_totalTime << " s");
}

void
Dispersion::CreateNodes () {
  nodes.Create (MOVING_NODES_NUMBER + 1);
  centralNode.Add (nodes.Get (0));

  for (size_t i = 1; i < nodes.GetN(); ++i)
    movingNodes.Add (nodes.Get (i));
}

void
Dispersion::SetupMobility () {
  int i = 0;
  for (NodeContainer::Iterator nodeIter = movingNodes.Begin ();
  		nodeIter != movingNodes.End (); ++nodeIter, ++i) {
  	int xDirection = i%2==0 ? 1 : -1; // for each i: x = 1 -1  1 -1
		int yDirection = i<2    ? 1 : -1; // for each i: y = 1  1 -1 -1

		Vector position (xDirection*m_initialRadius, yDirection*m_initialRadius, 0);
		Vector velocity (xDirection*m_movingSpeed, yDirection*m_movingSpeed, 0);

		MobilityHelper mobility;
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
		                               "MinX", DoubleValue (xDirection*m_initialRadius),
		                               "MinY", DoubleValue (yDirection*m_initialRadius));

		mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
		mobility.Install (*nodeIter);
		Ptr<ConstantVelocityMobilityModel> model =
				(*nodeIter)->GetObject<ConstantVelocityMobilityModel> ();
		model->SetVelocity (velocity);

		NS_LOG_DEBUG ("Position of moving node " << i << ": " << model->GetPosition ());
		NS_LOG_DEBUG ("Velocity of moving node " << i << ": " << model->GetVelocity ());
	}// for nodeIter in movingNodes

  MobilityHelper centralMobility;
  centralMobility.SetPositionAllocator (
  	"ns3::GridPositionAllocator",
		"MinX", DoubleValue (0),
		"MinY", DoubleValue (0) );
  centralMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  centralMobility.Install (centralNode);

  // Debug
	Ptr<MobilityModel> model = centralNode.Get(0)->GetObject<MobilityModel> ();
	NS_LOG_DEBUG ("Position of central node " << ": " << model->GetPosition ());
	NS_LOG_DEBUG ("Velocity of central node " << ": " << model->GetVelocity ());
}

void
Dispersion::InstallDevices () {
  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper channelHelper = YansWifiChannelHelper::Default ();

  wifiPhyHelper.SetChannel (channelHelper.Create ());

  mesh = MeshHelper::Default ();

  mesh.SetStackInstaller (m_stack);
  mesh.SetMacType ("RandomStart", TimeValue (Seconds (m_randomStart)));

  serverDevices = mesh.Install (wifiPhyHelper, centralNode);
  clientDevices = mesh.Install (wifiPhyHelper, movingNodes);

	if (m_pcap)
		wifiPhyHelper.EnablePcapAll (std::string ("traces/fivenodes"));
}

void
Dispersion::InstallInternetStack () {
  InternetStackHelper internetStack;
  internetStack.Install (nodes);
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  serverInterfaces = address.Assign (serverDevices);
  clientInterfaces = address.Assign (clientDevices);
}

void
Dispersion::InstallApplications () {
  UdpEchoServerHelper serverHelper (9);
  ApplicationContainer serverApps = serverHelper.Install (centralNode);

  serverApps.Start (Seconds (0.0));
  serverApps.Stop (Seconds (m_totalTime));

  UdpEchoClientHelper clientHelper (serverInterfaces.GetAddress (0), 9);

  NS_LOG_DEBUG ("Server address: " << serverInterfaces.GetAddress (0));

  clientHelper.SetAttribute ("MaxPackets", UintegerValue
  		((uint32_t)(m_totalTime*(1/m_packetInterval))));
  clientHelper.SetAttribute ("Interval", TimeValue (Seconds (m_packetInterval)));
  clientHelper.SetAttribute ("PacketSize", UintegerValue (m_packetSize));

  ApplicationContainer clientApps = clientHelper.Install (movingNodes);
  clientApps.Start (Seconds (0.0));
  clientApps.Stop (Seconds (m_totalTime));
}

int
Dispersion::Run () {
  ns3::Packet::EnablePrinting(); //TODO where to put this thing?
  
  CreateNodes ();
  InstallDevices ();
  SetupMobility ();
  InstallInternetStack ();
  InstallApplications ();
  Simulator::Schedule (Seconds (m_totalTime), &Dispersion::Report, this);
  Simulator::Stop (Seconds (m_totalTime));

  ConnectMxmlWriter ();
  
  Simulator::Run ();

  for (NodeContainer::Iterator nodeIter = movingNodes.Begin ();
  		nodeIter != movingNodes.End (); ++nodeIter) {
  	Ptr<ConstantVelocityMobilityModel> model =
  			(*nodeIter)->GetObject<ConstantVelocityMobilityModel> ();
		NS_LOG_DEBUG ("Position of moving node " << ": " << model->GetPosition ());
		NS_LOG_DEBUG ("Velocity of moving node " << ": " << model->GetVelocity ());
  }

  Simulator::Destroy ();
  return 0;
}

void
Dispersion::ConnectMxmlWriter ()
{
	map<MxmlWriter::Option, string> config;
	
	config[MxmlWriter::FILE_NAME] = "traces/trace.mxml";
	config[MxmlWriter::PROGRAM_NAME] = "five-nodes-mesh";
	config[MxmlWriter::PROCESS_NAME] = "Dispersion test";
	config[MxmlWriter::PROCESS_INSTANCE] = "1";

  MxmlWriter::GetInstance().Connect (config);
}

void
Dispersion::Report () {
	if (m_xml) {
		std::ostringstream oss;
		oss << "traces/fivenodes-report-server.xml";
		std::cout << "Printing mesh point server device diagnostics to "
				<< oss.str () << "\n";
		std::ofstream ofs;
		ofs.open (oss.str().c_str());
		if (!ofs.is_open()) {
			std::cout << "Error: Can't open file " << oss.str () << "\n";
			return;
		}
		mesh.Report (serverDevices.Get(0), ofs);
		ofs.close ();

		unsigned n (1);

		for (NetDeviceContainer::Iterator devIter = clientDevices.Begin ();
				devIter != clientDevices.End (); ++devIter, ++n) {
			std::ostringstream os;
			os << "traces/fivenodes-report-" << n << ".xml";
			std::cout << "Printing mesh point client device #" << n
					<< " diagnostics to " << os.str () << "\n";
			std::ofstream of;
			of.open (os.str ().c_str ());
			if (!of.is_open ()) {
				std::cerr << "Error: Can't open file " << os.str () << "\n";
				return;
			}
			mesh.Report (*devIter, of);
			of.close ();
		}
	}
}
