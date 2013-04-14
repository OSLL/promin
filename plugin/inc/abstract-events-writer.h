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

#ifndef __ABSTRACT_EVENTS_WRITER__
#define __ABSTRACT_EVENTS_WRITER__

#include <string>
#include <auto_ptr.h>
#include <map>

#include "callback-factory.h"

class AbstractEventsWriter
{
protected:

  std::string m_traceFileName;
  std::auto_ptr<CallbackFactory> m_pCallbackFactory;

  virtual void InitializeXmlDocument (const std::string& srcProgName,
      const std::string& processName, const std::string& procInstName) = 0;

  void ConnectCallbacks (CallbackFactory::CallbackType, CallbackFactory::CallbackType type_end);
  void ConnectCallback (CallbackFactory::CallbackType type);
  void ConnectCallbacks ();

public:

  enum Option
  {
    FILE_NAME,
    PROGRAM_NAME,
    PROCESS_NAME,
    PROCESS_INSTANCE,
  };

  void Connect(std::map<Option, std::string>& configuration);
  virtual void AddAuditEntry(const AuditTrailEntry& entry) = 0;

  virtual ~AbstractEventsWriter()
    {
    }
};

#endif

