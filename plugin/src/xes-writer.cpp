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

#include "xes-writer.h"

XesWriter::~XesWriter()
{
  m_tiDocument.SaveFile(m_traceFileName.c_str());
}

TiXmlElement*
XesWriter::createExtension(const char* name, const char* prefix, const char* uri)
{
  TiXmlElement * extension = new TiXmlElement("extension");

  extension->SetAttribute("name", name);
  extension->SetAttribute("prefix", prefix);
  extension->SetAttribute("uri", uri);

  return extension;
}

TiXmlElement*
XesWriter::createStringProperty(const char* key, const char* value)
{
  TiXmlElement * property = new TiXmlElement("string");

  property->SetAttribute("key", key);
  property->SetAttribute("value", value);

  return property;
}

TiXmlElement*
XesWriter::createDateProperty(const char* key, const char* value)
{
  TiXmlElement * property = new TiXmlElement("date");

  property->SetAttribute("key", key);
  property->SetAttribute("value", value);

  return property;
}

TiXmlElement*
XesWriter::createClassifier(const char* name, const char* keys)
{
  TiXmlElement * classifier = new TiXmlElement("classifier");

  classifier->SetAttribute("name", name);
  classifier->SetAttribute("keys", keys);

  return classifier;
}

TiXmlElement*
XesWriter::createEventGlobalScope()
{
  TiXmlElement * eventScope = new TiXmlElement("global");

  eventScope->SetAttribute("scope", "event");
  eventScope->LinkEndChild(createStringProperty("concept:name", "__INVALID__"));
  eventScope->LinkEndChild(createStringProperty("org:resource", "resource"));
  eventScope->LinkEndChild(createDateProperty("time:timestamp", "2011-04-13T18:27:00.515+02:00"));
  eventScope->LinkEndChild(createStringProperty("Position", "string"));
  eventScope->LinkEndChild(createStringProperty("Velocity", "string"));
  eventScope->LinkEndChild(createStringProperty("Size", "string"));
  eventScope->LinkEndChild(createStringProperty("Tags", "string"));

  return eventScope;
}

TiXmlElement*
XesWriter::createTraceGlobalScope()
{
  TiXmlElement * eventScope = new TiXmlElement("global");

  eventScope->SetAttribute("scope", "trace");
  eventScope->LinkEndChild(createStringProperty("concept:name", "__INVALID__"));

  return eventScope;
}

void
XesWriter::InitializeXmlDocument(const std::string& srcProgName,
    const std::string& processName, const std::string& procInstName)
{
  TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "utf-8", "");

  m_tiDocument.SetTabSize(2);
  m_tiDocument.LinkEndChild(decl);

  TiXmlElement * log = new TiXmlElement("log");

  log->SetAttribute("xes.version", "1.0");
  log->SetAttribute("xmlns", "http://code.deckfour.org/xes");
  log->SetAttribute("xes.creator", "NS-3 process mining plugin");
  m_tiDocument.LinkEndChild(log);

  log->LinkEndChild(createExtension("Concept", "concept", "http://code.deckfour.org/xes/concept.xesext"));
  log->LinkEndChild(createExtension("Time", "time", "http://code.deckfour.org/xes/time.xesext"));
  log->LinkEndChild(createExtension("Organizational", "org", "http://code.deckfour.org/xes/org.xesext"));
  log->LinkEndChild(createTraceGlobalScope());
  log->LinkEndChild(createEventGlobalScope());

  m_pProcInstTiElement = new TiXmlElement("trace");

  m_pProcInstTiElement->LinkEndChild(createStringProperty("concept:name", procInstName.c_str()));
  log->LinkEndChild(m_pProcInstTiElement);
}

void
XesWriter::AddAuditEntry(const AuditTrailEntry& entry)
{
  TiXmlElement * eventElem = new TiXmlElement ("event");

  eventElem->LinkEndChild (createStringProperty("concept:name", entry.m_workflowModelElement.c_str()));
  eventElem->LinkEndChild (createStringProperty("org:resource", entry.m_originator.c_str()));

  time_t rawtime;
  struct tm timeinfo;
  char timeString[256];

  time (&rawtime);
  gmtime_r(&rawtime, &timeinfo);
  strftime(timeString, sizeof(timeString), "%FT%T%z", &timeinfo);

  eventElem->LinkEndChild(createDateProperty("time:timestamp", timeString));

  for(std::map<std::string, std::string>::const_iterator iter = entry.m_data.begin();
      iter !=entry.m_data.end(); ++iter)
    eventElem->LinkEndChild (createStringProperty(iter->first.c_str(), iter->second.c_str()));

  m_pProcInstTiElement->LinkEndChild(eventElem);
}
