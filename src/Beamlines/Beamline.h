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
#include <memory>
#include <string>

#include "AbsBeamline/ElementBase.h"
#include "Algorithms/Quaternion.hpp"

/**
 * @brief The elements of a LINE or RING definition, in input order.
 *
 * A LINE used inside another LINE is copied in as its elements, so the list
 * holds only elements, never other lines. The same element may appear in
 * several lines; the tracker works on clones.
 */
class Beamline : public std::list<std::shared_ptr<ElementBase>> {
public:
    /// Create an empty line called [b]name[/b].
    explicit Beamline(const std::string& name);

    /// Return the name of the LINE or RING.
    const std::string& getName() const;

    /// Set the position of the particle source.
    void setOrigin3D(const Vector_t<double, 3>& ori);

    /// Return the position of the particle source.
    Vector_t<double, 3> getOrigin3D() const;

    /// Set the initial direction of the particle source.
    void setInitialDirection(const Quaternion& rot);

    /// Return the initial direction of the particle source.
    Quaternion getInitialDirection() const;

    /// Set whether this is an open LINE or a closed RING.
    void setBeamlineTopology(BeamlineTopology topology);

    /// Return whether this is an open LINE or a closed RING.
    BeamlineTopology getBeamlineTopology() const;

private:
    std::string itsName_m;
    Vector_t<double, 3> itsOrigin_m;
    Quaternion itsCoordTrafoTo_m;
    BeamlineTopology itsTopology_m;
};

#endif  // OPALX_Beamline_HH
