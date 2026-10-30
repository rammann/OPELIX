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

#include "Track/Track.h"

#include "PartBunch/PartBunch.h"
#include "Utilities/Options.h"

Track* Track::block = 0;

Track::Track(
        BeamSequence* u, const PartData& ref, const std::vector<double>& dt,
        const std::vector<unsigned long long>& maxtsteps, int stepsperturn, double zStart,
        const std::vector<double>& zStop, const std::vector<std::string>& beamNames)
    : reference(ref),
      use(u),
      parser(),
      dT(dt),
      localTimeSteps(maxtsteps),
      stepsPerTurn(stepsperturn),
      zstart(zStart),
      zstop(zStop),
      beamNames_m(beamNames) {}

Track::~Track() {}
