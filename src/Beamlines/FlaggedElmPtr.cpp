// ------------------------------------------------------------------------
// $RCSfile: FlaggedElmPtr.cpp,v $
// ------------------------------------------------------------------------
// $Revision: 1.1.1.1 $
// ------------------------------------------------------------------------
// Copyright: see Copyright.readme
// ------------------------------------------------------------------------
//
// Class: FlaggedElmPtr
//
// ------------------------------------------------------------------------
// Class category: Beamlines
// ------------------------------------------------------------------------
//
// $Date: 2000/03/27 09:32:34 $
// $Author: fci $
//
// ------------------------------------------------------------------------

#include "Beamlines/FlaggedElmPtr.h"
#include "AbsBeamline/BeamlineVisitor.h"

FlaggedElmPtr::FlaggedElmPtr() : itsElement(), itsCounter(0), isReflected(false) {}

FlaggedElmPtr::FlaggedElmPtr(std::shared_ptr<ElementBase> elem, bool reflected)
    : itsElement(std::move(elem)), itsCounter(0), isReflected(reflected) {}

void FlaggedElmPtr::accept(BeamlineVisitor& v) const { v.visitFlaggedElmPtr(*this); }

void FlaggedElmPtr::setElement(ElementBase* elem) {
    if (elem == itsElement.get()) {
        return;
    }
    if (!elem) {
        itsElement.reset();
        return;
    }
    auto shared = elem->weak_from_this();
    if (!shared.expired()) {
        itsElement = shared.lock();
        return;
    }
    itsElement.reset(elem);
}
