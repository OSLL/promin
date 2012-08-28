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

#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include <ns3/core-module.h>
#include <ns3/mobility-module.h>

/*!
 * \brief Callback actions
 * \note Callback signature is hard - so std::string, not const std::string&
 */
namespace Action
{
  void DefaultAction();
  void UdpEchoTxAction(std::string context, ns3::Ptr<ns3::Packet const> packet);

  void PhyTxBeginAction(std::string context, ns3::Ptr<ns3::Packet const> packet);
  void PhyTxEndAction(std::string context, ns3::Ptr<ns3::Packet const> packet);
  void PhyTxDropAction(std::string context, ns3::Ptr<ns3::Packet const> packet);
  void PhyRxBeginAction(std::string context, ns3::Ptr<ns3::Packet const> packet);
  void PhyRxEndAction(std::string context, ns3::Ptr<ns3::Packet const> packet);
  void PhyRxDropAction(std::string context, ns3::Ptr<ns3::Packet const> packet);

  void CourseChangeAction(std::string context, ns3::Ptr<ns3::MobilityModel const> model);
}

#endif