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

#include <iostream>

#include "abstract-events-writer.h"
#include "actions.h"
#include "actions-support.h"

void
AbstractEventsWriter::Connect(std::map<Option, std::string>& configuration)
{
  // TODO: check whether configuration contains everything
  m_traceFileName = configuration[FILE_NAME];
  InitializeXmlDocument(configuration[PROGRAM_NAME],
      configuration[PROCESS_NAME], configuration[PROCESS_INSTANCE]);
  ConnectCallbacks();
}

void
AbstractEventsWriter::ConnectCallbacks(CallbackFactory::CallbackType type_begin,
    CallbackFactory::CallbackType type_end)
{
  for (int type = type_begin + 1; type != type_end; ++type)
    ConnectCallback((CallbackFactory::CallbackType) type);
}

void
AbstractEventsWriter::ConnectCallbacks()
{
  std::auto_ptr<CallbackFactory> cb(new CallbackFactory());
  m_pCallbackFactory = cb;

  ConnectCallbacks(CallbackFactory::Mobility_Begin, CallbackFactory::Mobility_End);
  ConnectCallbacks(CallbackFactory::Udp_Begin, CallbackFactory::Udp_End);
  ConnectCallbacks(CallbackFactory::Phy_Begin, CallbackFactory::Phy_End);
}

void
AbstractEventsWriter::ConnectCallback(CallbackFactory::CallbackType type)
{
  CallbackRecord rec = m_pCallbackFactory->GetCallbackRecord(type);
  ns3::Config::Connect(rec.context, *rec.callback);
}
