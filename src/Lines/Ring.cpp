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

Ring::Ring(const std::string& name)
    : Line(name, "RING",
           "The \"RING\" statement defines a closed-topology beamline list.\n"
           "\t<name> : ring = (<list>)") {
    fetchLine()->setBeamlineTopology(BeamlineTopology::RING);
}

double Ring::getLength() const {
    double length = 0.0;
    for (const auto& element : *fetchLine()) {
        if (element->getType() != ElementType::MARKER) {
            length += element->getGeometry().getArcLength();
        }
    }
    return length;
}

const char* Ring::getSequenceKeyword() const { return "RING"; }

void Ring::prepareForTracking() {
    // A line may use the same element several times. Clone every occurrence before
    // tagging it with this RING's name; the markers #S and #E are left alone.
    for (auto& element : *fetchLine()) {
        if (!element->getName().empty() && element->getName().front() == '#') continue;
        element.reset(element->clone());
        element->setBeamlineMembership(BeamlineTopology::RING, getOpalName());
    }
}
