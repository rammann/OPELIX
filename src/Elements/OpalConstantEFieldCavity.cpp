#include "Elements/OpalConstantEFieldCavity.h"
#include "AbsBeamline/ConstantEFieldCavity.h"
#include "Attributes/Attributes.h"
#include "Physics/Units.h"

OpalConstantEFieldCavity::OpalConstantEFieldCavity(const std::string& name)
    : OpalElement(
              name, SIZE, "CONSTANTEFIELDCAVITY",
              "The \"CONSTANTEFIELDCAVITY\" element defines a constant accelerating "
              "electric field in z-direction.") {
    itsAttr[EX] = Attributes::makeReal("EX", "Electric field strength Ex in MV/m", 0.0);
    itsAttr[EY] = Attributes::makeReal("EY", "Electric field strength Ey in MV/m", 0.0);
    itsAttr[EZ] = Attributes::makeReal("EZ", "Electric field strength Ez in MV/m", 0.0);

    registerOwnership();

    setElement(new ConstantEFieldCavity(name));
}

OpalConstantEFieldCavity::~OpalConstantEFieldCavity() {}

void OpalConstantEFieldCavity::update() {
    OpalElement::update();

    ConstantEFieldCavity* rep = dynamic_cast<ConstantEFieldCavity*>(getElement());
    if (rep) {
        double length = getLength();
        double exMV   = Attributes::getReal(itsAttr[EX]);
        double eyMV   = Attributes::getReal(itsAttr[EY]);
        double ezMV   = Attributes::getReal(itsAttr[EZ]);
        rep->getGeometry().setElementLength(length);
        rep->setEx(exMV * Units::MVpm2Vpm);
        rep->setEy(eyMV * Units::MVpm2Vpm);
        rep->setEz(ezMV * Units::MVpm2Vpm);
    }
}
