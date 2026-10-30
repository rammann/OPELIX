#ifndef OPAL_OpalConstantEFieldCavity_HH
#define OPAL_OpalConstantEFieldCavity_HH

#include "Elements/OpalElement.h"

class OpalConstantEFieldCavity : public OpalElement {
public:
    enum { EX = COMMON, EY, EZ, SIZE };

    explicit OpalConstantEFieldCavity(const std::string& name);
    virtual ~OpalConstantEFieldCavity();

    virtual void update();

private:
    OpalConstantEFieldCavity(const OpalConstantEFieldCavity&);
    void operator=(const OpalConstantEFieldCavity&);
};

#endif  // OPAL_OpalConstantEFieldCavity_HH
