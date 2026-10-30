#ifndef OPALX_FlaggedElmPtr_HH
#define OPALX_FlaggedElmPtr_HH

// ------------------------------------------------------------------------
// $RCSfile: FlaggedElmPtr.h,v $
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

#include <memory>
#include <utility>
#include "AbsBeamline/ElementBase.h"

class BeamlineVisitor;

// Class FlaggedElmPtr
// ------------------------------------------------------------------------
/// A section of a beam line.
//  A Beamline is built as a list of FlaggedElmPtr objects. Each holds a shared pointer to
//  its element and a reflection flag: if true, this section of the line is reversed.

class FlaggedElmPtr {
public:
    FlaggedElmPtr();

    /// Assign an element to this beamline position.
    explicit FlaggedElmPtr(std::shared_ptr<ElementBase> elem, bool reflected = false);

    /// Apply visitor.
    //  If any error occurs, this method throws an exception.
    void accept(BeamlineVisitor&) const;

    /// Get the element pointer.
    ElementBase* getElement() const { return itsElement.get(); }

    /// Set the element pointer; reuses the owning shared pointer if the element has one.
    void setElement(ElementBase* elem);

    void setElement(std::shared_ptr<ElementBase> elem) { itsElement = std::move(elem); }

    /// Get clone counter.
    //  Return the value set by the last call to [tt]setCounter()[/tt].
    int getCounter() const { return itsCounter; }

    /// Set clone counter.
    //  The value set is the entire responsibility of any algorithm using it.
    void setCounter(int count) const { itsCounter = count; }

    /// Get reflection flag.
    bool getReflectionFlag() const { return isReflected; }

    /// Set reflection flag.
    void setReflectionFlag(bool flag) const { isReflected = flag; }

private:
    /// The element at this position.
    std::shared_ptr<ElementBase> itsElement;

    /// Clone counter.
    //  This value can be set and interrogated by an algorithm.
    mutable int itsCounter;

    /// The reflection flag.
    //  If true, the portion of the line pointed at by [b]itsElement[/b]
    //  is reflected, i.e. its elements occur in reverse order.
    mutable bool isReflected;
};

#endif  // OPALX_FlaggedElmPtr_HH
