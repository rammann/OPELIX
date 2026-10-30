
#ifndef OPAL_OPALENGE_H
#define OPAL_OPALENGE_H

#include "Elements/OpalElement.h"

/** OpalEnge provides user interface information for the Enge and AsymmetricEnge objects
 */
class OpalEnge : public OpalElement {
public:
    /** enum maps string to integer value for UI definitions */
    enum {
        LENGTH = COMMON,
        FRINGE_LENGTH,
        COEFFICIENTS,
        SIZE  // size of the enum
    };

    /** Create the element called name and define its UI parameters. */
    explicit OpalEnge(const std::string& name);

    /** Destructor does nothing */
    virtual ~OpalEnge() {}

    /** Update the ScalingFFA with new parameters from UI parser */
    virtual void update();

private:
    // Not implemented.
    OpalEnge(const OpalEnge&);
    void operator=(const OpalEnge&);
};
#endif  // OPAL_OPALENGE_H
