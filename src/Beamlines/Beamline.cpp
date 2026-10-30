// ------------------------------------------------------------------------
// $RCSfile: Beamline.cpp,v $
// ------------------------------------------------------------------------
// $Revision: 1.1.1.1 $
// ------------------------------------------------------------------------
// Copyright: see Copyright.readme
// ------------------------------------------------------------------------
//
// Class: Beamline
//   Represents a sequence of elements.
//
// ------------------------------------------------------------------------
// Class category: Beamlines
// ------------------------------------------------------------------------
//
// $Date: 2000/03/27 09:32:34 $
// $Author: fci $
//
// ------------------------------------------------------------------------

#include "Beamlines/Beamline.h"

Beamline::Beamline(const std::string& name)
    : itsName_m(name),
      itsOrigin_m(0),
      itsCoordTrafoTo_m(1.0, 0.0, 0.0, 0.0),
      itsTopology_m(BeamlineTopology::LINE) {}

const std::string& Beamline::getName() const { return itsName_m; }

void Beamline::setOrigin3D(const Vector_t<double, 3>& ori) { itsOrigin_m = ori; }

Vector_t<double, 3> Beamline::getOrigin3D() const { return itsOrigin_m; }

void Beamline::setInitialDirection(const Quaternion& trafoTo) { itsCoordTrafoTo_m = trafoTo; }

Quaternion Beamline::getInitialDirection() const { return itsCoordTrafoTo_m; }

void Beamline::setBeamlineTopology(BeamlineTopology topology) { itsTopology_m = topology; }

BeamlineTopology Beamline::getBeamlineTopology() const { return itsTopology_m; }
