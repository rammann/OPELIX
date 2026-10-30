//
// Class OpalRBend
//   The RBEND element.
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
#ifndef OPAL_OpalRBend_HH
#define OPAL_OpalRBend_HH

#include "Elements/OpalBend.h"

class OpalRBend : public OpalBend {
public:
    /// Create the element called name.
    explicit OpalRBend(const std::string& name);

    virtual ~OpalRBend();

    /// Update the embedded CLASSIC bend.
    virtual void update();

private:
    // Not implemented.
    OpalRBend(const OpalRBend&);
    void operator=(const OpalRBend&);
};

#endif  // OPAL_OpalRBend_HH
