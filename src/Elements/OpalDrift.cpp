//
// Class OpalDrift
//   The class of OPAL drift spaces.
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
#include "Elements/OpalDrift.h"
#include "AbsBeamline/Drift.h"
#include "Attributes/Attributes.h"

OpalDrift::OpalDrift(const std::string& name)
    : OpalElement(name, SIZE, "DRIFT", "The \"DRIFT\" element defines a drift space.") {
    itsAttr[GEOMETRY] = Attributes::makeString("GEOMETRY", "BoundaryGeometry for Drifts");

    itsAttr[NSLICES] = Attributes::makeReal(
            "NSLICES", "The number of slices/ steps for this element in Map Tracking", 1);

    registerOwnership();

    setElement(new Drift(name));
}

OpalDrift::~OpalDrift() {}

bool OpalDrift::isDrift() const { return true; }

void OpalDrift::update() {
    OpalElement::update();

    //    Drift* drf = static_cast<Drift*>(getElement());

    // drf->getGeometry().setElementLength(Attributes::getReal(itsAttr[LENGTH]));

    auto drf = getElement();
    if (drf) {
        auto L = Attributes::getReal(itsAttr[LENGTH]);
        drf->getGeometry().setElementLength(L);
    } else
        std::cout << "error drf->setElementLength " << std::endl;
}
