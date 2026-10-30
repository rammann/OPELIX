//
// Class Track
//   Hold data for tracking.
//   Acts as a communication area between the various tracking commands.
//
// Copyright (c) 2008 - 2020, Paul Scherrer Institut, Villigen PSI, Switzerland
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

#ifndef OPAL_Track_HH
#define OPAL_Track_HH
#include <optional>
#include "Algorithms/ClosedOrbitInitialState.h"
#include "Algorithms/PartData.h"
#include "PartBunch/PartBunch.h"
#include "Track/TrackCmd.h"
#include "Track/TrackParser.h"

#include <string>
#include <vector>

class BeamSequence;

class Track {
public:
    Track(BeamSequence*, const PartData&, const std::vector<double>& dt,
          const std::vector<unsigned long long>& maxtsteps, int stepsperturn, double zStart,
          const std::vector<double>& zStop, const std::vector<std::string>& beamNames = {});
    ~Track();

    /// The reference data.
    PartData reference;

    /// The lattice to be tracked through.
    BeamSequence* use;

    /// The parser used during tracking.
    TrackParser parser;

    /// The block of track data.
    static Track* block;

    /// The initial timestep
    std::vector<double> dT;

    /** Optional kinetic-energy stop [GeV]; zero disables it (SINGLEGAP RING only).
     * Explicit input must be positive and finite. RUN transfers it to the tracker
     * in eV; it is incompatible with explicit TURNS, restart or multiple DT segments.
     * The first qualifying complete kick ends tracking, not a nominal turn or
     * an artificial interpolation of the RF energy increment.
     */
    double kineticEnergyStopGeV = 0;

    /// Independent snapshot selected by TRACK INITIALORBIT.
    std::optional<ClosedOrbitInitialState> initialOrbit;

    /// Maximal number of timesteps
    std::vector<unsigned long long> localTimeSteps;

    /// The timsteps per revolution period. ONLY available for OPAL-cycl.
    int stepsPerTurn;

    /// The location at which the simulation starts
    double zstart;

    /// The location at which the simulation stops
    std::vector<double> zstop;

    /// The names of beams selected on the enclosing TRACK command.
    /// (If RUN::BEAM is omitted, TrackRun resolves from this list.)
    std::vector<std::string> beamNames_m;

private:
    // Not implemented.
    Track();
    Track(const Track&);
    void operator=(const Track&);
};

#endif  // OPAL_Track_HH
