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

#include <ns3/core-module.h>
#include <ns3/applications-module.h>
#include <ns3/internet-module.h>
#include <ns3/mesh-helper.h>

class Dispersion
{
public:

  Dispersion();
  void Configure (int argc, char ** argv);
  int Run();

private:

  static const int MOVING_NODES_NUMBER = 4;

  double      m_initialRadius;
  double      m_movingSpeed;
  double      m_totalTime;
  double      m_randomStart;
  double      m_packetInterval;
  uint16_t     m_packetSize;
  std::string  m_stack;
  std::string  m_root;
  bool        m_pcap;
  bool        m_xml;

  ns3::MeshHelper mesh;
  ns3::NodeContainer nodes;
  ns3::NodeContainer movingNodes;
  ns3::NodeContainer centralNode;
  ns3::NetDeviceContainer serverDevices;
  ns3::NetDeviceContainer clientDevices;
  ns3::Ipv4InterfaceContainer serverInterfaces;
  ns3::Ipv4InterfaceContainer clientInterfaces;
  ns3::Ptr<ns3::UdpEchoServer> serverApp;
  ns3::Ptr<ns3::UdpEchoClient> clientApp;

  /*!
   * \brief Create nodes and split them to static and moving
   */
  void CreateNodes ();
  void InstallDevices ();
  void SetupMobility ();
  void InstallInternetStack ();
  void InstallApplications ();
  void ConnectMxmlWriter ();

  /*!
   * \brief Report all nodes statistics into separate xml files
   */
  void Report ();
};
