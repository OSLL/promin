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

#include <sstream>

#include "audit-trail-entry.h"

using namespace std;

AuditTrailEntry::AuditTrailEntry (
		const std::string& workflowModelElement,
		const std::string& eventType,
		const std::string& originator) : 
				TiXmlElement ("AuditTrailEntry"),
				data ("Data")
{
	LinkEndChild (&data);
	
  TiXmlElement * workflowElem = new TiXmlElement ("WorkflowModelElement");
  workflowElem->LinkEndChild (new TiXmlText (workflowModelElement.c_str()));
  LinkEndChild (workflowElem);  
	
  TiXmlElement * eventTypeElem = new TiXmlElement ("EventType");
  eventTypeElem->LinkEndChild (new TiXmlText (eventType.c_str()));
  LinkEndChild (eventTypeElem);  
	
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime ( &rawtime );
  TiXmlElement * timestampElem = new TiXmlElement ("Timestamp");
  timestampElem->LinkEndChild (new TiXmlText (asctime (timeinfo)));
  LinkEndChild (timestampElem);

  TiXmlElement * originatorTypeElem = new TiXmlElement ("Originator");
  originatorTypeElem->LinkEndChild (new TiXmlText (originator.c_str()));
  LinkEndChild (originatorTypeElem);  
}

void 
AuditTrailEntry::AddData (const std::string& name, const std::string& content)
{
  TiXmlElement * attribute = new TiXmlElement ("Attribute");
  attribute->SetAttribute ("name", name.c_str());
  attribute->LinkEndChild (new TiXmlText (content.c_str()));
  data.LinkEndChild (attribute);
}

