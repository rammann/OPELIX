#ifndef OPALX_Beamline_HH
#define OPALX_Beamline_HH

// ------------------------------------------------------------------------
// $RCSfile: Beamline.h,v $
// ------------------------------------------------------------------------
// $Revision: 1.1.1.1 $
// ------------------------------------------------------------------------
// Copyright: see Copyright.readme
// ------------------------------------------------------------------------
//
// Class: Beamline
//
// ------------------------------------------------------------------------
// Class category: Beamlines
// ------------------------------------------------------------------------
//
// $Date: 2000/03/27 09:32:34 $
// $Author: fci $
//
// ------------------------------------------------------------------------

#include <list>
#include <string>

#include "AbsBeamline/ElementBase.h"
#include "Algorithms/Quaternion.hpp"
#include "Beamlines/FlaggedElmPtr.h"

// Class Beamline
// ------------------------------------------------------------------------
/// A sequence of beam line elements: a list of FlaggedElmPtr objects.
//  A beam line carries no field model of its own. Its length is the sum of the lengths of its
//  elements; its geometry exists only to satisfy the ElementBase interface.

class Beamline : public ElementBase, public std::list<FlaggedElmPtr> {
public:
    Beamline();

    /// Constructor with given name.
    explicit Beamline(const std::string& name);

    /// Copy the element list; the base ElementBase state is not copied.
    Beamline(const Beamline& right);

    ~Beamline() override;

    /// Apply BeamlineVisitor to this line.
    void accept(BeamlineVisitor&) const override;

    /// Apply visitor to all elements of the line.
    //  If the parameter [b]r2l[/b] is true, the line is traversed from
    //  right (s=C) to left (s=0).
    //  If any error occurs, this method may throw an exception.
    void iterate(BeamlineVisitor&, bool r2l) const;

    /// Make a deep copy: every element is cloned.
    Beamline* clone() const override;

    /// Make structure copy.
    //  A new list of the same elements; nested lines are copied the same way.
    Beamline* copyStructure() override;

    /// Set sharable flag.
    //  The whole beamline and the elements depending on [b]this[/b] are
    //  marked as sharable.  After this call a [b]copyStructure()[/b] call
    //  reuses the element.
    void makeSharable() override;

    /// Placeholder geometry to satisfy the ElementBase interface.
    Geometry& getGeometry() override;
    const Geometry& getGeometry() const override;

    /// Get arc length.
    //  Return the length of the geometry, measured along the design orbit.
    double getArcLength() const;

    /// Get design length.
    //  Return the length of the geometry, measured along the design polygone.
    double getElementLength() const;

    /// Get beamline type
    ElementType getType() const override;

    /// Append an element.
    void append(const FlaggedElmPtr&);

    void setOrigin3D(const Vector_t<double, 3>& ori);
    Vector_t<double, 3> getOrigin3D() const;

    void setInitialDirection(const Quaternion& rot);
    Quaternion getInitialDirection() const;

    void initialise(PartBunch_t*) override {}
    void finalise() override {}
    void getFieldExtent(double& zBegin, double& zEnd) const override {
        zBegin = 0.0;
        zEnd   = getGeometry().getElementLength();
    }

private:
    // Not implemented.
    void operator=(const Beamline&);

    /// Placeholder geometry to satisfy the ElementBase interface.
    Geometry itsGeometry_m;

    Vector_t<double, 3> itsOrigin_m;

    Quaternion itsCoordTrafoTo_m;
};

#endif  // OPALX_Beamline_HH
