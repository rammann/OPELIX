//
// Class Ring
//   The RING definition. A RING uses LINE list syntax, but declares a closed
//   topology and does not permit repetition or reflection modifiers.
//
// Copyright (c) 2026, Paul Scherrer Institut, Villigen PSI, Switzerland
// All rights reserved
//
// This file is part of OPALX.
//
#ifndef OPAL_Ring_HH
#define OPAL_Ring_HH

#include "Lines/Line.h"

/**
 * @brief RING sequence declaration: ordered lattice members with periodic topology.
 *
 * A RING is not a physical element. Its embedded beamline and prepared occurrences
 * carry BeamlineMembership metadata; TrackRun reads the root topology to enable
 * periodic tracking. This declaration alone does not enforce geometrical closure
 * or find a closed orbit.
 */
class Ring : public Line {
public:
    /// Create the empty ring called [b]name[/b]; the parser fills it.
    explicit Ring(const std::string& name);

    ~Ring() override = default;

    /**
     * @brief Clone and tag occurrences before the runtime tracking lattice is built.
     *
     * Called by TrackRun through BeamSequence. Each element, including those copied in
     * from nested LINEs, is cloned and then tagged with this RING's name. Repeated uses
     * of an element become independent objects; the original elements and other rings
     * keep their own metadata. Internal names starting with '#' are skipped.
     *
     * ElementBase cloning preserves membership but starts with no calculated maps
     * and a false overlap flag. Calling this again replaces occurrence objects again;
     * clients must not rely on occurrence pointer identity across preparation.
     */
    void prepareForTracking() override;

    /**
     * @brief Design circumference in metres, summed over nominal occurrence arc lengths.
     *
     * Sums every non-marker element, including explicit drifts and elements used more
     * than once. No implicit gaps are inferred from placement, and field-support extensions
     * never contribute. Rectangular-bend chord lengths are converted by Geometry::getArcLength().
     * This is not the length of a tracked or closed orbit and does not validate 3D closure.
     */
    double getLength() const override;

protected:
    const char* getSequenceKeyword() const override;

private:
    Ring(const Ring&)           = delete;
    void operator=(const Ring&) = delete;
};

#endif  // OPAL_Ring_HH
