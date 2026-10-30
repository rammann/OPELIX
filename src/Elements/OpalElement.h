//
// Class OpalElement
//   Base class for all beam line elements.
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
#ifndef OPAL_OpalElement_HH
#define OPAL_OpalElement_HH

#include "AbstractObjects/Element.h"

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

class Attribute;
class Statement;

class OpalElement : public Element {
public:
    /// The common attributes for all elements.
    enum {
        TYPE,                       // The design type.
        APERT,                      // The aperture data.
        LENGTH,                     // The element length.
        ELEMEDGE,                   // The position of the element (in path length)
        WAKEF,                      // The wake function to be used
        PARTICLEMATTERINTERACTION,  // The particle mater interaction handler to be used
        X,       // The x-coordinate of the location of the element in floor coordinates
        Y,       // The y-coordinate of the location of the element in floor coordinates
        Z,       // The z-coordinate of the location of the element in floor coordinates
        THETA,   // The rotation about the y-axis
        PHI,     // The rotation about the x-axis
        PSI,     // The rotation about the z-axis
        DX,      // Misalignment in x (local coordinate system)
        DY,      // Misalignment in y (local coordinate system)
        DZ,      // Misalignment in z (local coordinate system)
        DTHETA,  // The rotation around y axis in rad.
        DPHI,    // The rotation around x axis in rad.
        DPSI,    // The rotation around s axis in rad.
        OUTFN,   // Output filename
        DELETEONTRANSVERSEEXIT,  // Flag whether particles should be deleted if exit transversally
        COMMON
    };

    virtual ~OpalElement();

    /// Return element length.
    virtual double getLength() const;

    // return the element aperture vector
    std::pair<ApertureType, std::vector<double>> getApert() const;

    /// Parse the element.
    //  This special version for elements handles unknown attributes by
    //  appending them to the attribute list.
    virtual void parse(Statement&);

    /// Print the object.
    //  This special version handles special printing in OPAL-8 format.
    virtual void print(std::ostream&) const;

    /// @brief Transfer the parsed attributes onto the OPALX ElementBase
    /// Processes the state common to every element:
    ///     - Aperture
    ///     - Placement (either ELEMEDGE or X/Y/Z/THETA/PHI/PSI)
    ///     - Misalignment
    /// @note Each element overrides update() to specialise its geometry and
    /// field data, this base version is also called.
    /// @note Final placement is resolved by PlacementResolver, not here.
    virtual void update();

protected:
    /// Create the element called [b]name[/b] for the input keyword [b]keyword[/b].
    OpalElement(const std::string& name, int size, const char* keyword, const char* help);

    void registerOwnership() const;

private:
    // Not implemented.
    OpalElement();
    void operator=(const OpalElement&);

    /// Enforce that an element is placed by exactly one method.
    //  @param hasElemEdge true if ELEMEDGE was set (placement along the reference path).
    //  @param has6DPose   true if any of X, Y, Z, THETA, PHI was set (6D lab-frame pose).
    //                     PSI is shared between both modes and is excluded from this test.
    //  Throws OpalException if both or neither method is given.
    void validatePlacement(bool hasElemEdge, bool has6DPose) const;

    // The original size of the attribute list.
    int itsSize;
};

#endif  // OPAL_OpalElement_HH
