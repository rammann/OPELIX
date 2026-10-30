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
#ifndef OPAL_Configure_HH
#define OPAL_Configure_HH

namespace Configure {

    /// Configure all commands.
    extern void configure();
};  // namespace Configure

#endif  // OPAL_Configure_HH