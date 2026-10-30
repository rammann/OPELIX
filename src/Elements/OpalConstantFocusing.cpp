#include "Elements/OpalConstantFocusing.h"

#include "AbsBeamline/ConstantFocusing.h"
#include "Attributes/Attributes.h"
#include "Utilities/OpalException.h"

OpalConstantFocusing::OpalConstantFocusing()
    : OpalElement(
              SIZE, "CONSTANTFOCUSING",
              "The \"CONSTANTFOCUSING\" element applies linear 3D focusing scaled from the "
              "initial space-charge field.") {
    itsAttr[STRENGTH] = Attributes::makeReal(
            "STRENGTH", "Dimensionless multiple of the initial mean space-charge field.", 1.0);
    itsAttr[RADIUS] = Attributes::makeReal("RADIUS", "Initial beam radius in m.", 0.0);

    registerOwnership();
    setElement(new ConstantFocusing("CONSTANTFOCUSING"));
}

OpalConstantFocusing::OpalConstantFocusing(const std::string& name, OpalConstantFocusing* parent)
    : OpalElement(name, parent) {
    setElement(new ConstantFocusing(name));
}

OpalConstantFocusing::~OpalConstantFocusing() {}

OpalConstantFocusing* OpalConstantFocusing::clone(const std::string& name) {
    return new OpalConstantFocusing(name, this);
}

void OpalConstantFocusing::update() {
    OpalElement::update();

    const double radius = Attributes::getReal(itsAttr[RADIUS]);
    if (!itsAttr[RADIUS].defaultUsed() && radius <= 0.0) {
        throw OpalException(
                "OpalConstantFocusing::update", "CONSTANTFOCUSING requires RADIUS > 0.");
    }

    auto* rep = dynamic_cast<ConstantFocusing*>(getElement());
    if (rep != nullptr) {
        rep->getGeometry().setElementLength(getLength());
        rep->setStrength(Attributes::getReal(itsAttr[STRENGTH]));
        rep->setRadius(radius);
    }
}
