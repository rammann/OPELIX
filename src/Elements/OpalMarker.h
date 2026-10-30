#ifndef OPAL_OpalMarker_HH
#define OPAL_OpalMarker_HH

// ------------------------------------------------------------------------
// $RCSfile: OpalMarker.h,v $
// ------------------------------------------------------------------------
// $Revision: 1.1.1.1 $
// ------------------------------------------------------------------------
// Copyright: see Copyright.readme
// ------------------------------------------------------------------------
//
// Class: OpalMarker
//
// ------------------------------------------------------------------------
//
// $Date: 2000/03/27 09:33:39 $
// $Author: Andreas Adelmann $
//
// ------------------------------------------------------------------------

#include "Elements/OpalElement.h"

// Class OpalMarker
// ------------------------------------------------------------------------
/// The MARKER element.

class OpalMarker : public OpalElement {
public:
    /// Create the element called name.
    explicit OpalMarker(const std::string& name);

    virtual ~OpalMarker();

    /// Update the embedded OPALX marker.
    virtual void update();

private:
    // Not implemented.
    OpalMarker(const OpalMarker&);
    void operator=(const OpalMarker&);
};

#endif  // OPAL_OpalMarker_HH
