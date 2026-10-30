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

#include "AbsBeamline/BeamlineVisitor.h"

Beamline::Beamline()
    : ElementBase(),
      std::list<FlaggedElmPtr>(),
      itsOrigin_m(0),
      itsCoordTrafoTo_m(1.0, 0.0, 0.0, 0.0) {
    shareFlag = false;
}

Beamline::Beamline(const std::string& name)
    : ElementBase(name),
      std::list<FlaggedElmPtr>(),
      itsOrigin_m(0),
      itsCoordTrafoTo_m(1.0, 0.0, 0.0, 0.0) {
    shareFlag = false;
}

Beamline::Beamline(const Beamline& rhs)
    : ElementBase(),
      std::list<FlaggedElmPtr>(rhs),
      itsOrigin_m(rhs.itsOrigin_m),
      itsCoordTrafoTo_m(rhs.itsCoordTrafoTo_m) {
    shareFlag = false;
    setBeamlineMembership(rhs.getBeamlineTopology(), rhs.getBeamlineOwnerName());
}

Beamline::~Beamline() {}

void Beamline::accept(BeamlineVisitor& visitor) const { visitor.visitBeamline(*this); }

void Beamline::iterate(BeamlineVisitor& visitor, bool r2l) const {
    if (r2l) {
        for (auto op = rbegin(); op != rend(); ++op) {
            op->accept(visitor);
        }
    } else {
        for (const FlaggedElmPtr& op : *this) {
            op.accept(visitor);
        }
    }
}

Beamline* Beamline::clone() const {
    Beamline* line = new Beamline(getName());
    for (const FlaggedElmPtr& op : *this) {
        // Make copy of the FlaggedElmPtr containing a deep copy of its child.
        FlaggedElmPtr newObj(op);
        newObj.setElement(op.getElement()->clone());
        line->append(newObj);
    }
    line->itsOrigin_m       = itsOrigin_m;
    line->itsCoordTrafoTo_m = itsCoordTrafoTo_m;
    line->setBeamlineMembership(getBeamlineTopology(), getBeamlineOwnerName());
    return line;
}

Beamline* Beamline::copyStructure() {
    if (isSharable()) {
        return this;
    }
    Beamline* line = new Beamline(getName());
    for (const FlaggedElmPtr& op : *this) {
        // The copy constructor ensures proper transmission of data.
        FlaggedElmPtr newObj(op);
        newObj.setElement(op.getElement()->copyStructure());
        line->append(newObj);
    }
    line->itsOrigin_m       = itsOrigin_m;
    line->itsCoordTrafoTo_m = itsCoordTrafoTo_m;
    line->setBeamlineMembership(getBeamlineTopology(), getBeamlineOwnerName());
    return line;
}

void Beamline::makeSharable() {
    shareFlag = true;
    for (const FlaggedElmPtr& op : *this) {
        op.getElement()->makeSharable();
    }
}

Geometry& Beamline::getGeometry() { return itsGeometry_m; }

const Geometry& Beamline::getGeometry() const { return itsGeometry_m; }

double Beamline::getArcLength() const {
    double length = 0.0;
    for (const FlaggedElmPtr& op : *this) {
        length += op.getElement()->getGeometry().getArcLength();
    }
    return length;
}

double Beamline::getElementLength() const {
    double length = 0.0;
    for (const FlaggedElmPtr& op : *this) {
        length += op.getElement()->getGeometry().getElementLength();
    }
    return length;
}

ElementType Beamline::getType() const { return ElementType::BEAMLINE; }

void Beamline::append(const FlaggedElmPtr& obj) { push_back(obj); }

void Beamline::setOrigin3D(const Vector_t<double, 3>& ori) { itsOrigin_m = ori; }

Vector_t<double, 3> Beamline::getOrigin3D() const { return itsOrigin_m; }

void Beamline::setInitialDirection(const Quaternion& trafoTo) { itsCoordTrafoTo_m = trafoTo; }

Quaternion Beamline::getInitialDirection() const { return itsCoordTrafoTo_m; }
