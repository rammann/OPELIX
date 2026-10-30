//
// Class Ring
//   The RING definition.
//
// Copyright (c) 2026, Paul Scherrer Institut, Villigen PSI, Switzerland
// All rights reserved
//
// This file is part of OPALX.
//
#include "Lines/Ring.h"

#include <algorithm>

namespace {
    double designLength(const Beamline& line) {
        double length = 0.0;
        for (const auto& member : line) {
            const auto* element = member.getElement();
            if (!element || element->getType() == ElementType::MARKER) continue;
            if (const auto* nested = dynamic_cast<const Beamline*>(element)) {
                length += designLength(*nested);
            } else {
                length += element->getGeometry().getArcLength();
            }
        }
        return length;
    }

    void cloneAndAssignMembership(Beamline& line, const std::string& ringName) {
        // A member list may share prototypes, even after copying the outer sequence.
        // Isolate every occurrence before tagging it; nested LINEs retain the outer
        // ringName so that membership describes the tracked RING, not a parent chain.
        for (auto& member : line) {
            ElementBase* element = member.getElement();
            if (!element || (!element->getName().empty() && element->getName().front() == '#')) {
                continue;
            }

            member.setElement(element->clone());
            element = member.getElement();
            element->setBeamlineMembership(BeamlineTopology::RING, ringName);

            if (auto* nestedLine = dynamic_cast<Beamline*>(element)) {
                cloneAndAssignMembership(*nestedLine, ringName);
            }
        }
    }
}  // namespace

Ring::Ring(const std::string& name)
    : Line(name, "RING",
           "The \"RING\" statement defines a closed-topology beamline list.\n"
           "\t<name> : ring = (<list>)") {
    // The membership name must not be empty; an unnamed ring uses its keyword.
    fetchLine()->setBeamlineMembership(BeamlineTopology::RING, name.empty() ? getKeyword() : name);
}

void Ring::parse(Statement& stat) { parseDefinition(stat, false); }

double Ring::getLength() const { return designLength(*fetchLine()); }

const char* Ring::getSequenceKeyword() const { return "RING"; }

void Ring::prepareForTracking() {
    Beamline* line = fetchLine();
    line->setBeamlineMembership(BeamlineTopology::RING, getOpalName());
    cloneAndAssignMembership(*line, getOpalName());
}
