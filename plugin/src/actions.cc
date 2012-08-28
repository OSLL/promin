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

#include "actions.h"
#include "actions-support.h"
#include "mxml-writer.h"

namespace Action
{
  using namespace ns3;

  void
  DefaultAction()
  {
  }

  void
  UdpEchoTxAction(std::string context, Ptr<Packet const> packet)
  {
    MxmlWriter::GetInstance().AddAuditEntry(
        new PacketAuditTrialEntry ("UdpEchoTx", "complete", context, packet));
  }

  void
  PhyTxBeginAction(std::string context, Ptr<Packet const> packet)
  {
    MxmlWriter::GetInstance().AddAuditEntry(
        new PacketAuditTrialEntry ("PhyTxBegin", "complete", context, packet));
  }

  void
  PhyTxEndAction(std::string context, Ptr<Packet const> packet)
  {
    MxmlWriter::GetInstance().AddAuditEntry(
        new PacketAuditTrialEntry ("PhyTxEnd", "complete", context, packet));
  }

  void
  PhyTxDropAction(std::string context, Ptr<Packet const> packet)
  {
    MxmlWriter::GetInstance().AddAuditEntry(
        new PacketAuditTrialEntry ("PhyTxDrop", "complete", context, packet));
  }

  void
  PhyRxBeginAction(std::string context, Ptr<Packet const> packet)
  {
    MxmlWriter::GetInstance().AddAuditEntry(
        new PacketAuditTrialEntry("PhyRxBegin", "complete", context, packet));
  }

  void
  PhyRxEndAction(std::string context, Ptr<Packet const> packet)
  {
    MxmlWriter::GetInstance().AddAuditEntry(
        new PacketAuditTrialEntry ("PhyRxEnd", "complete", context, packet));
  }

  void
  PhyRxDropAction(std::string context, Ptr<Packet const> packet)
  {
    MxmlWriter::GetInstance().AddAuditEntry(
        new PacketAuditTrialEntry ("PhyRxDrop", "complete", context, packet));
  }

  void
  CourseChangeAction(std::string context, Ptr<MobilityModel const> model)
  {
    Vector position = model->GetPosition();
    std::stringstream pos_ss;
    pos_ss << context << " x = " << position.x << ", y = " << position.y;

    Vector velocity = model->GetVelocity();
    std::stringstream vel_ss;
    vel_ss << context << " x = " << velocity.x << ", y = " << velocity.y;

    AuditTrailEntry * entry = new AuditTrailEntry("CourseChange", "complete",
        context);
    entry->AddData("Velocity", vel_ss.str());
    entry->AddData("Velocity", pos_ss.str());
    MxmlWriter::GetInstance().AddAuditEntry(entry);
  }
}