//
// Class TrackCmd
//   The class for the OPAL TRACK command.
//
// Copyright (c) 200x - 2022, Paul Scherrer Institut, Villigen PSI, Switzerland
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
#include "Track/TrackCmd.h"
#include <cmath>
#include "Track/CofCmd.h"

#include "AbstractObjects/BeamSequence.h"
#include "AbstractObjects/OpalData.h"
#include "Attributes/Attributes.h"
#include "Structure/Beam.h"
#include "Track/Track.h"
#include "Track/TrackParser.h"
#include "Utilities/OpalException.h"

namespace {
    // The attributes of class TrackCmd
    enum {
        LINE,          // The name of lattice to be tracked.
        BEAM,          // The name of beam to be used.
        BEAMS,         // The names of beams to be used (replaces BEAM for input).
        DT,            // The integration timestep in second.
        T0,            // The elapsed time (sec) of the bunch
        MAXSTEPS,      // The maximum timesteps we integrate
        ZSTART,        // Defines a z-location [m] where the reference particle starts
        ZSTOP,         // Defines a z-location [m], after which the simulation stops when the last
                       // particles passes
        STEPSPERTURN,  // Return the timsteps per revolution period. ONLY available for OPAL-cycl.
        EKINSTOP,      // Optional SINGLEGAP reference kinetic-energy target [GeV].
        INITIALORBIT,
        SIZE
    };
}  // namespace

TrackCmd::TrackCmd() : Action(SIZE, "TRACK", "The \"TRACK\" command initiates tracking.") {
    itsAttr[INITIALORBIT] = Attributes::makeString(
            "INITIALORBIT", "Named COF launch for orbit-local generated distributions.");
    itsAttr[LINE] = Attributes::makeString("LINE", "Name of lattice to be tracked.");

    itsAttr[BEAM] = Attributes::makeString("BEAM", "Name of beam to be used.", "UNNAMED_BEAM");

    itsAttr[BEAMS] =
            Attributes::makeStringArray("BEAMS", "Names of beams to be used (replaces BEAM).");

    itsAttr[DT] = Attributes::makeRealArray("DT", "The integration timestep in [s].");

    itsAttr[T0] = Attributes::makeReal("T0", "The elapsed time of the bunch in seconds", 0.0);

    itsAttr[MAXSTEPS] = Attributes::makeRealArray(
            "MAXSTEPS",
            "The maximum number of integration steps dt, should be larger ZSTOP/(beta*c average).");

    itsAttr[ZSTART] = Attributes::makeReal(
            "ZSTART", "Defines a z-location [m] where the reference particle starts.", 0.0);

    itsAttr[ZSTOP] = Attributes::makeRealArray(
            "ZSTOP",
            "Defines a z-location [m], after which the simulation stops when the last particles "
            "passes.");

    itsAttr[STEPSPERTURN] = Attributes::makeReal(
            "STEPSPERTURN", "The time steps per revolution period, only for opal-cycl.", 720);
    itsAttr[EKINSTOP] = Attributes::makeReal(
            "EKINSTOP", "Stop after the first complete RF kick reaching this kinetic energy [GeV].",
            0.0);

    registerOwnership(AttributeHandler::COMMAND);
    AttributeHandler::addAttributeOwner("TRACK", AttributeHandler::COMMAND, "RUN");
    AttributeHandler::addAttributeOwner("TRACK", AttributeHandler::COMMAND, "ENDTRACK");
}

TrackCmd::TrackCmd(const std::string& name, TrackCmd* parent) : Action(name, parent) {}

TrackCmd::~TrackCmd() {}

TrackCmd* TrackCmd::clone(const std::string& name) { return new TrackCmd(name, this); }

std::vector<double> TrackCmd::getDT() const {
    std::vector<double> dTs = Attributes::getRealArray(itsAttr[DT]);
    if (dTs.size() == 0) {
        dTs.push_back(1e-12);
    }
    for (double dt : dTs) {
        if (dt < 0.0) {
            throw OpalException(
                    "TrackCmd::getDT", "The time steps provided with DT have to be positive");
        }
    }
    return dTs;
}

double TrackCmd::getT0() const { return Attributes::getReal(itsAttr[T0]); }

double TrackCmd::getZStart() const { return Attributes::getReal(itsAttr[ZSTART]); }

std::vector<double> TrackCmd::getZStop() const {
    std::vector<double> zstop = Attributes::getRealArray(itsAttr[ZSTOP]);
    if (zstop.size() == 0) {
        zstop.push_back(1000000.0);
    }
    return zstop;
}

std::vector<unsigned long long> TrackCmd::getMaxSteps() const {
    std::vector<double> maxsteps_d = Attributes::getRealArray(itsAttr[MAXSTEPS]);
    std::vector<unsigned long long> maxsteps_i;
    if (maxsteps_d.size() == 0) {
        maxsteps_i.push_back(10ul);
    }
    for (double numSteps : maxsteps_d) {
        if (numSteps < 0) {
            throw OpalException(
                    "TrackCmd::getMAXSTEPS",
                    "The number of steps provided with MAXSTEPS has to be positive");
        } else {
            unsigned long long value = numSteps;
            maxsteps_i.push_back(value);
        }
    }

    return maxsteps_i;
}

int TrackCmd::getStepsPerTurn() const { return (int)Attributes::getReal(itsAttr[STEPSPERTURN]); }

void TrackCmd::execute() {
    // Find BeamSequence
    BeamSequence* theLineToTrack = BeamSequence::find(Attributes::getString(itsAttr[LINE]));

    // Resolve beams (BEAMS has precedence over BEAM).
    std::vector<std::string> beamNames = Attributes::getStringArray(itsAttr[BEAMS]);
    if (beamNames.empty()) {
        beamNames.push_back(Attributes::getString(itsAttr[BEAM]));
    }
    if (beamNames.empty()) {
        throw OpalException("TrackCmd::execute", "Neither \"BEAMS\" nor \"BEAM\" was specified.");
    }

    std::vector<Beam*> beams;
    beams.reserve(beamNames.size());
    for (const auto& name : beamNames) {
        if (name.empty()) {
            throw OpalException("TrackCmd::execute", "Empty beam name in \"BEAMS\".");
        }
        beams.push_back(Beam::find(name));  // fail fast
    }

    // Current tracker supports only a single beam; use the first one for now.
    Beam* beam = beams.front();

    std::vector<double> dt                   = getDT();
    double t0                                = getT0();
    std::vector<unsigned long long> maxsteps = getMaxSteps();
    int stepsperturn                         = getStepsPerTurn();
    double zstart                            = getZStart();
    std::vector<double> zstop                = getZStop();

    size_t numTracks = dt.size();
    numTracks        = std::max(numTracks, maxsteps.size());
    numTracks        = std::max(numTracks, zstop.size());
    for (size_t i = dt.size(); i < numTracks; ++i) {
        dt.push_back(dt.back());
    }
    for (size_t i = maxsteps.size(); i < numTracks; ++i) {
        maxsteps.push_back(maxsteps.back());
    }
    for (size_t i = zstop.size(); i < numTracks; ++i) {
        zstop.push_back(zstop.back());
    }

    /// \todo track block needs to be removed
    /// \todo here the tracker is constructed

    const double kineticStop = Attributes::getReal(itsAttr[EKINSTOP]);
    if (!itsAttr[EKINSTOP].defaultUsed() && (!std::isfinite(kineticStop) || kineticStop <= 0))
        throw OpalException("TrackCmd::execute", "EKINSTOP must be finite and positive [GeV].");
    std::optional<ClosedOrbitInitialState> initialOrbit;
    if (!itsAttr[INITIALORBIT].defaultUsed()) {
        if (beams.size() != 1 || OpalData::getInstance()->inRestartRun() || zstart != 0)
            throw OpalException("INITIALORBIT", "Requires one beam, a fresh run and ZSTART=0.");
        const std::string initialOrbitName = Attributes::getString(itsAttr[INITIALORBIT]);
        const auto& initial                = CofCmd::findResult(initialOrbitName);
        if (!beam->hasExplicitEnergy())
            throw OpalException("INITIALORBIT", "BEAM must specify the same energy as COF.");
        initial.validate(
                theLineToTrack->getOpalName(), beam->getParticleName(), beam->getReference());
        if (!itsAttr[T0].defaultUsed() && t0 != initial.time)
            throw OpalException("INITIALORBIT", "TRACK T0 conflicts with the COF launch time.");
        initialOrbit = initial;
    }
    Track::block = new Track(
            theLineToTrack, beam->getReference(), dt, maxsteps, stepsperturn, zstart, zstop,
            beamNames);
    Track::block->kineticEnergyStopGeV = kineticStop;
    Track::block->initialOrbit         = std::move(initialOrbit);

    Track::block->parser.run();

    // Clean up.
    delete Track::block;
    Track::block = nullptr;
}
