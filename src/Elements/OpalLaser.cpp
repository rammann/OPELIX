#include "Elements/OpalLaser.h"

#include "AbsBeamline/Laser.h"
#include "Attributes/Attributes.h"
#include "Utilities/OpalException.h"

#include <cmath>
#include <sstream>
#include <vector>

namespace {
    double getRequiredPositiveReal(
            const std::vector<Attribute>& attrs, int index, const std::string& where) {
        if (!attrs[index]) {
            throw OpalException(where, "\"" + attrs[index].getName() + "\" must be set.");
        }

        const double value = Attributes::getReal(attrs[index]);
        if (value <= 0.0) {
            throw OpalException(
                    where, "\"" + attrs[index].getName() + "\" must be greater than 0.");
        }

        return value;
    }

    Vector_t<double, 3> getVector3(
            const Attribute& attr, const std::string& where, const std::string& semanticName) {
        if (!attr) {
            throw OpalException(where, "\"" + attr.getName() + "\" must be set.");
        }

        const std::vector<double> values = Attributes::getRealArray(attr);
        if (values.size() != 3) {
            std::ostringstream msg;
            msg << "\"" << attr.getName() << "\" must contain exactly 3 values for " << semanticName
                << "; got " << values.size() << ".";
            throw OpalException(where, msg.str());
        }

        return Vector_t<double, 3>(values[0], values[1], values[2]);
    }
}  // namespace

OpalLaser::OpalLaser(const std::string& name)
    : OpalElement(
              name, SIZE, "LASER",
              "The \"LASER\" element defines a passive analytic laser pulse for OPALX.") {
    itsAttr[WAVELENGTH]  = Attributes::makeReal("WAVELENGTH", "Laser wavelength in m.");
    itsAttr[PULSEENERGY] = Attributes::makeReal("PULSEENERGY", "Total laser pulse energy in J.");
    itsAttr[PULSELENGTH] = Attributes::makeReal("PULSELENGTH", "Laser pulse duration in s.");
    itsAttr[WAISTX]      = Attributes::makeReal("WAISTX", "Horizontal laser waist in m.");
    itsAttr[WAISTY]      = Attributes::makeReal("WAISTY", "Vertical laser waist in m.");
    itsAttr[DIR]         = Attributes::makeRealArray("DIR", "Laser propagation direction vector.");
    itsAttr[STOKES]      = Attributes::makeRealArray(
            "STOKES", "Normalized Stokes polarization vector {xi1, xi2, xi3}.");

    registerOwnership();

    setElement(new Laser(name));
}

OpalLaser::~OpalLaser() {}

void OpalLaser::update() {
    constexpr const char* where = "OpalLaser::update()";

    OpalElement::update();

    auto* laser = dynamic_cast<Laser*>(getElement());
    if (!laser) {
        return;
    }

    // Skip propagation silently when none of the required attributes is set.
    //
    /// \todo Refactor to follow the standard OPAL element pattern:
    //   1. Register every attribute with a sentinel default in the constructor
    //      (e.g. Attributes::makeReal("WAVELENGTH", "...", 0.0)) so that
    //      Attributes::getReal() always returns a well-defined value.
    //      OpalSolenoid and OpalCavity are good examples.
    //   2. In update(), read all attributes unconditionally with
    //      Attributes::getReal() / Attributes::getRealArray() and forward them
    //      to Laser without throwing. No guard is needed because
    //      zero/sentinel values are harmless.
    //   3. Move the "must be positive / must be set" validation into Laser,
    //      e.g. in initialise() which is called only when the element is
    //      actually inserted into a tracked beamline.  That is the right layer
    //      to enforce physical constraints, and it keeps OpalLaser::update()
    //      consistent with every other OpalElement subclass.
    const bool anyConfigured = itsAttr[WAVELENGTH] || itsAttr[PULSEENERGY] || itsAttr[PULSELENGTH]
                               || itsAttr[WAISTX] || itsAttr[WAISTY] || itsAttr[DIR];
    if (!anyConfigured) {
        return;
    }

    const double wavelength  = getRequiredPositiveReal(itsAttr, WAVELENGTH, where);
    const double pulseEnergy = getRequiredPositiveReal(itsAttr, PULSEENERGY, where);
    const double pulseLength = getRequiredPositiveReal(itsAttr, PULSELENGTH, where);
    const double waistX      = getRequiredPositiveReal(itsAttr, WAISTX, where);
    const double waistY      = getRequiredPositiveReal(itsAttr, WAISTY, where);

    Vector_t<double, 3> direction = getVector3(itsAttr[DIR], where, "the propagation direction");
    const double directionNorm    = std::sqrt(dot(direction, direction));
    if (directionNorm <= 0.0) {
        throw OpalException(where, "\"DIR\" must be a non-zero vector.");
    }
    direction /= directionNorm;

    Vector_t<double, 3> stokes(0.0);
    if (itsAttr[STOKES]) {
        stokes                   = getVector3(itsAttr[STOKES], where, "the Stokes vector");
        const double stokesNorm2 = dot(stokes, stokes);
        if (stokesNorm2 > 1.0 + 1.0e-12) {
            throw OpalException(where, "\"STOKES\" must satisfy xi1^2 + xi2^2 + xi3^2 <= 1.");
        }
    }

    const double length = itsAttr[LENGTH] ? Attributes::getReal(itsAttr[LENGTH]) : 0.0;
    if (length < 0.0) {
        throw OpalException(where, "\"L\" must be greater than or equal to 0.");
    }

    laser->getGeometry().setElementLength(length);
    laser->setWavelength(wavelength);
    laser->setPulseEnergy(pulseEnergy);
    laser->setPulseLength(pulseLength);
    laser->setWaistX(waistX);
    laser->setWaistY(waistY);
    laser->setDirection(direction);
    laser->setStokes(stokes);
}
