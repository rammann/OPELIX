//
// Class RealConstant
//   The REAL CONSTANT definition.
//
// Copyright (c) 2000 - 2021, Paul Scherrer Institut, Villigen PSI, Switzerland
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
#include "ValueDefinitions/RealConstant.h"

#include "AbstractObjects/OpalData.h"
#include "Attributes/Attributes.h"
#include "BuildInfo.h"
#include "Physics/Physics.h"
#include "Physics/Units.h"
#include "Utilities/Options.h"
#include "Utility/IpplInfo.h"

#include <cmath>
#include <iostream>

RealConstant::RealConstant(const std::string& name)
    : ValueDefinition(
              name, 1, "REAL_CONSTANT",
              "The \"REAL CONSTANT\" statement defines a global "
              "real constant:\n"
              "\tREAL CONSTANT <name> = <real-expression>;\n") {
    itsAttr[0] = Attributes::makeReal("VALUE", "The constant value", 0.0);

    registerOwnership(AttributeHandler::STATEMENT);
}

RealConstant::RealConstant(const std::string& name, double value) : RealConstant(name) {
    Attributes::setReal(itsAttr[0], value);
    itsAttr[0].setReadOnly(true);
}

void RealConstant::defineBuiltins() {
    OpalData* opal = OpalData::getInstance();
    opal->define(new RealConstant("PI", Physics::pi));
    opal->define(new RealConstant("TWOPI", Physics::two_pi));
    opal->define(new RealConstant("RADDEG", Units::rad2deg));
    opal->define(new RealConstant("DEGRAD", Units::deg2rad));
    opal->define(new RealConstant("E", Physics::e));

    opal->define(new RealConstant("MUMASS", Physics::m_mu));
    opal->define(new RealConstant("EMASS", Physics::m_e));
    opal->define(new RealConstant("PMASS", Physics::m_p));
    opal->define(new RealConstant("HMMASS", Physics::m_hm));
    opal->define(new RealConstant("H2PMASS", Physics::m_h2p));
    opal->define(new RealConstant("DMASS", Physics::m_d));
    opal->define(new RealConstant("ALPHAMASS", Physics::m_alpha));
    opal->define(new RealConstant("CMASS", Physics::m_c));
    opal->define(new RealConstant("XEMASS", Physics::m_xe));
    opal->define(new RealConstant("UMASS", Physics::m_u));

    opal->define(new RealConstant("CLIGHT", Physics::c));
    opal->define(new RealConstant("AMU", Physics::amu));

    opal->define(new RealConstant(
            "OPALVERSION", buildinfo::version_major * 10000 + buildinfo::version_minor * 100
                                   + buildinfo::version_patch));
    opal->define(new RealConstant("RANK", ippl::Comm->rank()));
}

RealConstant::~RealConstant() {}

double RealConstant::getReal() const { return Attributes::getReal(itsAttr[0]); }

void RealConstant::print(std::ostream& os) const {
    os << "REAL CONST " << getOpalName() << '=' << itsAttr[0] << ';';
    os << std::endl;
}

void RealConstant::printValue(std::ostream& os) const { os << itsAttr[0]; }
