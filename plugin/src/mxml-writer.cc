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

#include "mxml-writer.h"
#include "actions.h"
#include "actions-support.h"

using namespace std;
using namespace ns3;

MxmlWriter MxmlWriter::s_instance;

void
MxmlWriter::Connect (map<Option, string>& configuration)
{	
	// TODO: check whether configuration contains everything 
	m_traceFileName = configuration[FILE_NAME];
	InitializeXmlDocument(configuration[PROGRAM_NAME],
	                      configuration[PROCESS_NAME],
	                      configuration[PROCESS_INSTANCE]);
	ConnectCallbacks ();
}

MxmlWriter&
MxmlWriter::GetInstance ()
{
	// TODO: type of exception?
	return s_instance;
}

MxmlWriter::~MxmlWriter () 
{
 	m_tiDocument.SaveFile (m_traceFileName.c_str()); 
}

void
MxmlWriter::InitializeXmlDocument (const string& srcProgName,
		const string& processName, const string& procInstName)
{
	TiXmlDeclaration * decl = new TiXmlDeclaration ("1.0", "utf-8", "");

	m_tiDocument.SetTabSize(2);
	m_tiDocument.LinkEndChild (decl);
	
	TiXmlElement * workflowLog = new TiXmlElement ("WorkflowLog");

	workflowLog->SetAttribute ("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	workflowLog->SetAttribute("xsi:noNamespaceSchemaLocation",
	                            "http://is.tm.tue.nl/research/processmining/WorkflowLog.xsd");
	m_tiDocument.LinkEndChild (workflowLog);
	
	TiXmlElement * source = new TiXmlElement ("Source");

	source->SetAttribute ("program", srcProgName.c_str());
	workflowLog->LinkEndChild (source);

	TiXmlElement * process = new TiXmlElement ("Process");

	process->SetAttribute ("id", processName.c_str());
	workflowLog->LinkEndChild (process);

	m_pProcInstTiElement = new TiXmlElement ("ProcessInstance");

	m_pProcInstTiElement->SetAttribute ("id", procInstName.c_str());
	m_pProcInstTiElement->SetAttribute ("description", "NS-3 trace in MXML format");
	process->LinkEndChild (m_pProcInstTiElement);
}

void
MxmlWriter::ConnectCallbacks (CallbackFactory::CallbackType type_begin, 
		CallbackFactory::CallbackType type_end)
{
	for (int type = type_begin + 1; type != type_end; ++type)
		ConnectCallback ((CallbackFactory::CallbackType) type);
}

void
MxmlWriter::ConnectCallbacks ()
{
	auto_ptr<CallbackFactory> cb (new CallbackFactory());
	m_pCallbackFactory = cb;
	
	ConnectCallbacks (CallbackFactory::Mobility_Begin, CallbackFactory::Mobility_End);
	ConnectCallbacks (CallbackFactory::Udp_Begin, CallbackFactory::Udp_End);
	ConnectCallbacks (CallbackFactory::Phy_Begin, CallbackFactory::Phy_End);
}

void
MxmlWriter::ConnectCallback (CallbackFactory::CallbackType type)
{
	CallbackRecord rec = m_pCallbackFactory->GetCallbackRecord (type);
	Config::Connect (rec.context, *rec.callback);
}

void
MxmlWriter::AddAuditEntry (AuditTrailEntry * entry)
{
	m_pProcInstTiElement->LinkEndChild (entry);
}
