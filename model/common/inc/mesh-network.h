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

#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/mesh-helper.h>
#include <ns3/netanim-module.h>

#include <string.h>
#include <auto_ptr.h>

class MeshNetwork
{
public:

  MeshNetwork(const std::string& networkName, size_t nodesNumber = 5,
              double totalTime = 1., double packetInterval = .1,
              uint16_t packetSize = 1024);
  void Configure(int argc, char ** argv);

protected:

  const std::string m_networkName;
  const size_t m_nodesNumber;

  double m_totalTime;
  double m_packetInterval;
  uint16_t m_packetSize;
  bool m_pcap;
  bool m_xml;

  ns3::MeshHelper m_meshHelper;
  ns3::NodeContainer m_nodes;
  ns3::NetDeviceContainer m_devices;
  ns3::Ipv4InterfaceContainer m_interfaces;

  std::auto_ptr<ns3::AnimationInterface> m_animator;

public:

  /*!
   * \brief Create nodes
   */
  void CreateNodes();
  void InstallDevices();
  void InstallInternetStack();
  void ConnectMxmlWriter();
  void ActivateAnimator();

  /*!
   * \brief Report all nodes statistics into separate xml files
   */
  void Report();
  void PrintNodePositions() const;
};
