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

#ifndef __CALLBACK_FACTORY_H__
#define __CALLBACK_FACTORY_H__

#include <exception>
#include <string>

#include <ns3/core-module.h>
#include "audit-trail-entry.h"

struct CallbackRecord
{
	CallbackRecord () {}
	
	CallbackRecord (ns3::CallbackBase * callback,
			const std::string& context, const std::string& name) :
				callback (callback), context (context), name (name) {}
	
	ns3::CallbackBase * callback;
	std::string context;
	std::string name;
};

struct BadCallbackTypeException : public std::exception
{
   virtual const char* what() const throw ()
   {
  	 return "Bad callback type";
   }
};

class CallbackFactory
{

public:

  enum CallbackType
  {
  	Begin,
  	
    Udp_Begin,
    UdpEchoTx,
    Udp_End,
    
    Phy_Begin,
    PhyTxBegin,
    PhyTxEnd,
    PhyTxDrop,
    PhyRxBegin,
    PhyRxEnd,
    PhyRxDrop,
    Phy_End,

    Mobility_Begin,
    CourseChange,
    Mobility_End,

    End,
  };

  CallbackFactory();

  const CallbackRecord& GetCallbackRecord (CallbackType type);

private:

  std::map<CallbackType, CallbackRecord> m_callbacks;

  CallbackRecord MakeDefaultCallbackRecord (std::string name);
  CallbackRecord MakeCallbackRecord (ns3::CallbackBase * callback, 
  		std::string context, std::string name);
  ns3::CallbackBase * GetPacketCallback (CallbackType type);
  const std::string& GetPath (CallbackType type);
  const std::string& GetFriendlyName (CallbackType type);
};

#endif
