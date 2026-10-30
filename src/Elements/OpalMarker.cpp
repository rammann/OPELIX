// ------------------------------------------------------------------------
// $RCSfile: OpalMarker.cpp,v $
// ------------------------------------------------------------------------
// $Revision: 1.4 $
// ------------------------------------------------------------------------
// Copyright: see Copyright.readme
// ------------------------------------------------------------------------
//
// Class: OpalMarker
//   The class of OPAL markers.
//
// ------------------------------------------------------------------------
//
// $Date: 2002/01/22 15:16:02 $
// $Author: jsberg $
//
// ------------------------------------------------------------------------

#include "Elements/OpalMarker.h"
#include "AbsBeamline/Marker.h"
#include "Utilities/Options.h"

#include <iostream>

// Class OpalMarker
// ------------------------------------------------------------------------

OpalMarker::OpalMarker(const std::string& name)
    : OpalElement(name, COMMON, "MARKER", "The \"MARKER\" element defines a marker.") {
    setElement(new Marker(name));
}

OpalMarker::~OpalMarker() {}

void OpalMarker::update() {}