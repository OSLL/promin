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

#include <stdexcept>

#include <ns3/network-module.h>

#include "callback-factory.h"
#include "actions.h"

namespace ns3
{
  template <typename R, typename T1, typename T2>
  Callback<R,T1,T2>* MakeCallbackPtr (R (*fnPtr)(T1,T2)) {
    return new Callback<R,T1,T2> (fnPtr, true, true);
  }
  template <typename R>
  Callback<R>* MakeCallbackPtr (R (*fnPtr)()) {
    return new Callback<R> (fnPtr, true, true);
  }
} // namespace ns3

using namespace Action;
using namespace ns3;

CallbackRecord
CallbackFactory::MakeCallbackRecord (CallbackBase * callback, std::string context, std::string name)
{
	CallbackRecord rec (callback, context, name);
	return rec;
}
CallbackRecord
CallbackFactory::MakeDefaultCallbackRecord (std::string name)
{
	return MakeCallbackRecord (MakeCallbackPtr (&DefaultAction), "/", name);
}

CallbackFactory::CallbackFactory()
{ 
  /*** Udp echo callbacks ***/
  for(int i = Udp_Begin + 1; i < Udp_End; ++i)
  {
    CallbackType cb = static_cast<CallbackType>(i);
    m_callbacks[cb] = MakeCallbackRecord(GetPacketCallback(cb), GetPath(cb), GetFriendlyName(cb));
  }

  /*** Phy callbacks ***/
  for(int i = Phy_Begin + 1; i < Phy_End; ++i)
  {
    CallbackType cb = static_cast<CallbackType>(i);
    m_callbacks[cb] = MakeCallbackRecord(GetPacketCallback(cb), GetPath(cb), GetFriendlyName(cb));
  }
 
  /*** Mobility callbacks ***/
  m_callbacks[CourseChange] = MakeCallbackRecord (MakeCallbackPtr (&CourseChangeAction),
                                                  GetPath(CourseChange), GetFriendlyName(CourseChange));
}

const CallbackRecord&
CallbackFactory::GetCallbackRecord (CallbackType type)
{
	if (!m_callbacks.count(type))
		throw BadCallbackTypeException();

	return m_callbacks[type];
}

CallbackBase *
CallbackFactory::GetPacketCallback(CallbackType type)
{
  static const std::pair<CallbackType, void (*)(std::string, Ptr<Packet const>)> _c[] =
  {
    std::make_pair(UdpEchoTx, &UdpEchoTxAction),
    std::make_pair(PhyTxBegin, &PhyTxBeginAction),
    std::make_pair(PhyTxDrop, &PhyTxDropAction),
    std::make_pair(PhyTxEnd, &PhyTxEndAction),
    std::make_pair(PhyRxBegin, &PhyRxBeginAction),
    std::make_pair(PhyRxDrop, &PhyRxDropAction),
    std::make_pair(PhyRxEnd, &PhyRxEndAction),
  };
  static const std::map<CallbackType, void (*)(std::string, Ptr<Packet const>)> callbacks(_c, _c + sizeof(_c)/sizeof(*_c));

  if (!callbacks.count(type))
    throw std::domain_error("Callback function not defined");

  return MakeCallbackPtr(callbacks.find(type)->second);
}

const std::string&
CallbackFactory::GetPath (CallbackType type)
{
  static const std::string udpContext = "/NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/";
  static const std::string phyContext = "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/";
  static const std::string mobilityContext = "/NodeList/*/$ns3::MobilityModel/";

  static const std::pair<CallbackType, std::string> _c[] =
  {
    std::make_pair(UdpEchoTx, udpContext + "Tx"),
    std::make_pair(PhyTxBegin, phyContext + "PhyTxBegin"),
    std::make_pair(PhyTxDrop, phyContext + "PhyTxDrop"),
    std::make_pair(PhyTxEnd, phyContext + "PhyTxEnd"),
    std::make_pair(PhyRxBegin, phyContext + "PhyRxBegin"),
    std::make_pair(PhyRxDrop, phyContext + "PhyRxDrop"),
    std::make_pair(PhyRxEnd, phyContext + "PhyRxEnd"),
    std::make_pair(CourseChange, mobilityContext + "CourseChange"),
  };
  static const std::map<CallbackType, std::string> paths(_c, _c + sizeof(_c)/sizeof(*_c));

  if (!paths.count(type))
    throw std::domain_error("Path not defined");

  return paths.find(type)->second;
}

const std::string&
CallbackFactory::GetFriendlyName (CallbackType type)
{
  static const std::pair<CallbackType, std::string> _c[] =
  {
    std::make_pair(UdpEchoTx, "UdpEchoTx"),
    std::make_pair(PhyTxBegin, "PhyTxBegin"),
    std::make_pair(PhyTxDrop, "PhyTxDrop"),
    std::make_pair(PhyTxEnd, "PhyTxEnd"),
    std::make_pair(PhyRxBegin, "PhyRxBegin"),
    std::make_pair(PhyRxDrop, "PhyRxDrop"),
    std::make_pair(PhyRxEnd, "PhyRxEnd"),
    std::make_pair(CourseChange, "CourseChange"),
  };
  static const std::map<CallbackType, std::string> names(_c, _c + sizeof(_c)/sizeof(*_c));

  if (!names.count(type))
    throw std::domain_error("Name not defined");

  return names.find(type)->second;
}
