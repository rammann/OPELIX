#ifndef OPAL_OpalConstantFocusing_HH
#define OPAL_OpalConstantFocusing_HH

#include "Elements/OpalElement.h"

class OpalConstantFocusing : public OpalElement {
public:
    enum { STRENGTH = COMMON, RADIUS, SIZE };

    explicit OpalConstantFocusing(const std::string& name);
    ~OpalConstantFocusing() override;
    void update() override;

private:
    OpalConstantFocusing(const OpalConstantFocusing&);
    void operator=(const OpalConstantFocusing&);
};

#endif  // OPAL_OpalConstantFocusing_HH
