//
// Class ProbeRep
//   Representation for Probe.
//
// Copyright (c) 200x - 2020, Paul Scherrer Institut, Villigen PSI, Switzerland
// All rights reserved
//
// This file is part of OPAL.
//
// OPAL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// You should have received a copy of the GNU General Public License
// along with OPAL. If not, see <https://www.gnu.org/licenses/>.
//
#include "BeamlineCore/ProbeRep.h"

ProbeRep::ProbeRep() : Probe(), geometry(), active(true) {}

ProbeRep::ProbeRep(const ProbeRep& right) : Probe(right), geometry(right.geometry), active(true) {}

ProbeRep::ProbeRep(const std::string& name) : Probe(name), geometry(), active(true) {}

ProbeRep::~ProbeRep() {}

ElementBase* ProbeRep::clone() const { return new ProbeRep(*this); }

Geometry& ProbeRep::getGeometry() { return geometry; }

const Geometry& ProbeRep::getGeometry() const { return geometry; }

void ProbeRep::setActive(bool flag) { active = flag; }
