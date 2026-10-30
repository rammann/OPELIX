//
// Namespace Configure
//   The OPAL configurator.
//   This class must be modified to configure the commands to be contained
//   in an executable OPAL program. For each command keyword it registers a
//   function in OpalData that creates a new object; the OPAL parser calls it
//   for every statement with that keyword. It also defines the built-in
//   objects (constants, P0, the line markers #S and #E, UNNAMED_BEAM).
//   This class could be part of the class OpalData.  It is separated from
//   that class and opale into a special module in order to reduce
//   dependencies between modules.
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
#include "OpalConfigure/Configure.h"
#include "AbstractObjects/OpalData.h"

#include "Distribution/Distribution.h"

// Basic action commands.
#include "BasicActions/Call.h"
#include "BasicActions/DumpEMFields.h"
#include "BasicActions/Echo.h"
#include "BasicActions/Help.h"
#include "BasicActions/Option.h"
#include "BasicActions/PSystem.h"
#include "BasicActions/Quit.h"
#include "BasicActions/Stop.h"
#include "BasicActions/System.h"
#include "BasicActions/Title.h"
#include "BasicActions/Value.h"

// Macro command.
#include "OpalParser/MacroCmd.h"

// Commands introducing a special mode.
#include "Track/CofCmd.h"
#include "Track/TrackCmd.h"

// Table-related commands.
#include "Structure/Beam.h"
#include "Structure/BinningCmd.h"
#include "Structure/EmissionSource.h"
#include "Structure/FieldSolverCmd.h"

// Value definitions commands.
#include "ValueDefinitions/BoolConstant.h"
#include "ValueDefinitions/RealConstant.h"
#include "ValueDefinitions/RealVariable.h"
#include "ValueDefinitions/RealVector.h"
#include "ValueDefinitions/StringConstant.h"

// Element commands.
#include "Elements/OpalBox.h"
#include "Elements/OpalCavity.h"
#include "Elements/OpalCollimator.h"
#include "Elements/OpalConstantEFieldCavity.h"
#include "Elements/OpalConstantFocusing.h"
#include "Elements/OpalCyclotronSector.h"
#include "Elements/OpalDrift.h"
#include "Elements/OpalFieldmapElement.h"
#include "Elements/OpalLaser.h"
#include "Elements/OpalMarker.h"
#include "Elements/OpalMonitor.h"
#include "Elements/OpalMultipole.h"
#include "Elements/OpalMultipoleT.h"
#include "Elements/OpalPolynomialTimeDependence.h"
#include "Elements/OpalProbe.h"
#include "Elements/OpalQuadrupole.h"
#include "Elements/OpalRBend.h"
#include "Elements/OpalSBend.h"
#include "Elements/OpalScalingFFAMagnet.h"
#include "Elements/OpalSinusoidalTimeDependence.h"
#include "Elements/OpalSolenoid.h"
#include "Elements/OpalSplineTimeDependence.h"
#include "Elements/OpalTravelingWave.h"
#include "Elements/OpalVariableRFCavity.h"
#include "Lines/Ring.h"

// EndFieldModels
#include "Elements/OpalAsymmetricEnge.h"
#include "Elements/OpalEnge.h"
#include "Elements/OpalTanh.h"

// Structure-related commands.
#include "Lines/EmissionSourceList.h"
#include "Lines/Line.h"

#include "changes.h"

#include <memory>

// Modify these methods to add new commands.
// ------------------------------------------------------------------------

namespace {

    /// Register the input keyword of class T.
    //  Building one object reads its keyword and records which keyword owns
    //  which attribute, for the hints in parse error messages.
    template <class T>
    void add() {
        OpalData::getInstance()->addKeyword(T("").getKeyword(), [](const std::string& name) {
            return std::make_unique<T>(name);
        });
    }

    void makeActions() {
        add<Call>();
        add<DumpEMFields>();
        add<Echo>();
        add<Help>();
        add<Option>();
        add<Stop>();
        add<Quit>();
        add<PSystem>();
        add<System>();
        add<Title>();
        add<TrackCmd>();
        add<CofCmd>();
        add<Value>();
    }

    void makeDefinitions() {
        add<MacroCmd>();

        add<Beam>();
        add<EmissionSource>();
        add<FieldSolverCmd>();
        add<Distribution>();
        add<EmissionSourceList>();
        add<BinningCmd>();
    }

    void makeElements() {
        add<OpalCavity>();
        add<OpalTravelingWave>();
        add<OpalConstantEFieldCavity>();
        add<OpalConstantFocusing>();
        add<OpalCyclotronSector>();
        add<OpalTrimCoil>();
        add<OpalBox>();
        add<OpalCollimator>();
        add<OpalDrift>();
        add<OpalLaser>();
        add<OpalMonitor>();
        add<OpalMarker>();
        add<OpalProbe>();
        add<OpalMultipole>();
        add<OpalQuadrupole>();
        add<OpalRBend>();
        add<OpalSBend>();
        add<OpalScalingFFAMagnet>();
        add<OpalFieldmapElement>();
        add<OpalSolenoid>();
        add<Line>();
        add<Ring>();
        add<OpalPolynomialTimeDependence>();
        add<OpalSinusoidalTimeDependence>();
        add<OpalSplineTimeDependence>();
        add<OpalMultipoleT>();
        add<OpalVariableRFCavity>();

        add<OpalEnge>();
        add<OpalTanh>();
        add<OpalAsymmetricEnge>();
    }

    /// Define the objects that exist before the input is read.
    void makeBuiltins() {
        RealConstant::defineBuiltins();
        RealVariable::defineBuiltins();
        StringConstant::defineBuiltins();

        // The begin and end markers that Line puts around every beam line.
        for (const char* name : {"#S", "#E"}) {
            OpalData::getInstance()->define(new OpalMarker(name));
        }

        // The beam TRACK uses when BEAM is not given.
        auto defBeam = std::make_unique<Beam>("UNNAMED_BEAM");
        try {
            defBeam->update();
            OpalData::getInstance()->define(defBeam.release());
        } catch (...) {
        }
    }
};  // namespace

namespace Configure {
    void configure() {
        makeDefinitions();
        makeElements();
        makeActions();
        makeBuiltins();
        Versions::fillChanges();
    }
};  // namespace Configure
