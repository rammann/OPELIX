
#ifndef OPAL_OPALASYMMETRICENGE_H
#define OPAL_OPALASYMMETRICENGE_H

#include "Elements/OpalElement.h"

/** OpalEnge provides user interface information for the Enge and AsymmetricEnge objects
 */
class OpalAsymmetricEnge : public OpalElement {
public:
    /** enum maps string to integer value for UI definitions */
    enum {
        START_HALF_LENGTH = COMMON,
        START_FRINGE_LENGTH,
        START_COEFFICIENTS,
        END_HALF_LENGTH,
        END_FRINGE_LENGTH,
        END_COEFFICIENTS,
        SIZE  // size of the enum
    };

    /** Create the element called name and define its UI parameters. */
    explicit OpalAsymmetricEnge(const std::string& name);

    /** Destructor does nothing */
    virtual ~OpalAsymmetricEnge() = default;

    /** Update the ScalingFFA with new parameters from UI parser */
    virtual void update();

private:
    // Not implemented.
    OpalAsymmetricEnge(const OpalAsymmetricEnge&);
    OpalAsymmetricEnge& operator=(const OpalAsymmetricEnge&);
};
#endif  // OPAL_OPALASYMMETRICENGE_H
