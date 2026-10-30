//
// Class Option
//   The OPTION command.
//   The user interface allowing setting of OPAL options.
//   The actual option flags are contained in namespace Options.
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
#include "BasicActions/Option.h"
#include "Ippl.h"

#include "Algorithms/LinearTransferMapBuilder.h"
#include "Attributes/Attributes.h"
#include "OpalParser/FileStream.h"
#include "Utilities/OpalException.h"
#include "Utilities/Options.h"

#include "Utility/Inform.h"
#include "Utility/IpplInfo.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <limits>

extern Inform* gmsg;

using namespace Options;

namespace {
    // The attributes of class Option.
    enum {
        ECHO,
        INFO,
        TRACE,
        WARN,
        SEED,
        PSDUMPFREQ,
        STATDUMPFREQ,
        CHECKPOINTFREQ,
        STEPINFOFQ,
        PRINTRANKDISTRFQ,
        REPARTFREQ,
        RHODUMP,
        EBDUMP,
        RANKDUMP,
        AUTOPHASE,
        ENABLEHDF5,
        ENABLEVTK,
        ENABLELINEARTRANSFERMAPS,
        LINEARTRANSFERMAPRICHARDSON,
        LINEARTRANSFERMAPSTEPS,
        LINEARTRANSFERMAPINTEGRATOR,
        ASCIIDUMP,
        BOUNDPDESTROY,
        BEAMHALOBOUNDARY,
        VERSION,
        HALOSHIFT,
        MINBINEMITTED,
        COMPUTEPERCENTILES,
        QM_MODE,
        AGGRESSIVE_STATE_SYNC,
        LOADBALANCINGTHRESHOLD,
        LOADBALANCINGPAIRWEIGHT,
        SIZE
    };
}  // namespace

Option::Option(const std::string& name)
    : Action(name, SIZE, "OPTION", "The \"OPTION\" statement defines OPAL execution options.") {
    itsAttr[ECHO] = Attributes::makeBool("ECHO", "If true, give echo of input", echo);

    itsAttr[INFO] = Attributes::makeBool("INFO", "If true, print information messages", info);

    itsAttr[TRACE] = Attributes::makeBool(
            "TRACE",
            "If true, print execution trace"
            "Must be the first option in the inputfile in "
            "order to render correct results",
            mtrace);

    itsAttr[WARN] = Attributes::makeBool("WARN", "If true, print warning messages", warn);

    itsAttr[SEED] = Attributes::makeReal(
            "SEED", "The seed for the random generator, -1 will use time(0) as seed ");

    itsAttr[PSDUMPFREQ] = Attributes::makeReal(
            "PSDUMPFREQ",
            "The frequency to dump the phase space, "
            "i.e. dump data when step%psDumpFreq==0; 0 disables phase space dumps. "
            "Its default value is 10.",
            psDumpFreq);

    itsAttr[STATDUMPFREQ] = Attributes::makeReal(
            "STATDUMPFREQ",
            "The frequency to dump statistical data "
            "(e.g. RMS beam quantities), i.e. dump data when step%statDumpFreq == 0, "
            "0 disables statistical dumps. Its default value is 10.",
            statDumpFreq);

    itsAttr[CHECKPOINTFREQ] = Attributes::makeReal(
            "CHECKPOINTFREQ",
            "The frequency to write a restart checkpoint after completed integration steps, "
            "i.e. checkpoint when GlobalTrackStep % CHECKPOINTFREQ == 0. "
            "A value of 0 disables checkpointing. Its default value is 0.",
            checkpointFreq);

    itsAttr[STEPINFOFQ] = Attributes::makeReal(
            "STEPINFOFQ",
            "The frequency to print per-step tracking status lines. "
            "A value of 0 disables these status lines; its default value is 1.",
            stepInfoFreq);

    itsAttr[PRINTRANKDISTRFQ] = Attributes::makeReal(
            "PRINTRANKDISTRFQ",
            "The frequency to print per-rank particle distribution tables. "
            "A value of 0 disables these tables; its default value is 0.",
            printRankDistrFreq);

    itsAttr[REPARTFREQ] = Attributes::makeReal(
            "REPARTFREQ",
            "The frequency to do particles repartition "
            "for better load balance between nodes, its "
            "default value is "
                    + std::to_string(repartFreq) + ".",
            repartFreq);

    itsAttr[MINBINEMITTED] = Attributes::makeReal(
            "MINBINEMITTED",
            "The number of bins that have to be emitted before the bins are squashed into "
            "a single bin; the default value is "
                    + std::to_string(minBinEmitted) + ".",
            minBinEmitted);

    itsAttr[RHODUMP] = Attributes::makeBool(
            "RHODUMP",
            "If true, in addition to the phase "
            "space the scalar rho field is also dumped (H5Block)",
            rhoDump);

    itsAttr[EBDUMP] = Attributes::makeBool(
            "EBDUMP",
            "If true, in addition to the phase space the "
            "E and B field at each particle is also dumped into the H5 file)",
            ebDump);

    itsAttr[RANKDUMP] = Attributes::makeBool(
            "RANKDUMP",
            "If true, the current MPI rank for each particle is dumped into the H5 file. "
            "Default: false.",
            rankDump);

    itsAttr[AUTOPHASE] = Attributes::makeReal(
            "AUTOPHASE",
            "If greater than zero OPAL is scanning "
            "the phases of each rf structure in order to get maximum "
            "acceleration. Defines the number of refinements of the "
            "search range",
            autoPhase);

    itsAttr[ENABLEHDF5] =
            Attributes::makeBool("ENABLEHDF5", "If true, HDF5 actions are enabled", enableHDF5);

    itsAttr[ENABLEVTK] = Attributes::makeBool(
            "ENABLEVTK", "If true, writing of VTK files are enabled", enableVTK);

    itsAttr[ENABLELINEARTRANSFERMAPS] = Attributes::makeBool(
            "ENABLELINEARTRANSFERMAPS",
            "If true, calculate first-order external-field transfer maps during design-orbit "
            "threading.",
            enableLinearTransferMaps);

    itsAttr[LINEARTRANSFERMAPRICHARDSON] = Attributes::makeReal(
            "LINEARTRANSFERMAPRICHARDSON",
            "Richardson levels (integer 0..4). Zero retains centered differences; each level "
            "adds twelve rays per segment. This does not change ray integration order.",
            linearTransferMapRichardsonLevels);
    itsAttr[LINEARTRANSFERMAPSTEPS] = Attributes::makeRealArray(
            "LINEARTRANSFERMAPSTEPS",
            "Six positive starting perturbations in (x,x',y,y',zeta,delta); positions in metres, "
            "slopes and delta dimensionless. The delta amplitude must be less than one.");
    Attributes::setRealArray(
            itsAttr[LINEARTRANSFERMAPSTEPS],
            std::vector<double>(linearTransferMapSteps.begin(), linearTransferMapSteps.end()));
    itsAttr[LINEARTRANSFERMAPINTEGRATOR] = Attributes::makePredefinedString(
            "LINEARTRANSFERMAPINTEGRATOR",
            "Integrator for map reference and shadow rays only: BORIS (alias LF2), "
            "fixed-step RK4 or fixed-step DOP853 (eighth-order formula, no adaptive controller). "
            "Independent of Richardson differentiation; does not change particle TRACK.",
            {"BORIS", "LF2", "RK4", "DOP853"}, linearTransferMapIntegrator);

    itsAttr[ASCIIDUMP] = Attributes::makeBool(
            "ASCIIDUMP", "If true, some of the elements dump in ASCII instead of HDF5", asciidump);

    itsAttr[BOUNDPDESTROY] = Attributes::makeReal(
            "BOUNDPDESTROY",
            "The number of sigmas away from the mean "
            "at which particles are deleted. Default 10",
            boundpDestroy);

    itsAttr[BEAMHALOBOUNDARY] = Attributes::makeReal(
            "BEAMHALOBOUNDARY",
            "Defines in terms of sigma where "
            "the halo starts. Default 0.0",
            beamHaloBoundary);

    itsAttr[VERSION] = Attributes::makeReal(
            "VERSION", "Version of OPAL for which input file was written", version);

    itsAttr[HALOSHIFT] = Attributes::makeReal(
            "HALOSHIFT", "Constant parameter to shift halo value (default: 0.0)", haloShift);

    itsAttr[COMPUTEPERCENTILES] = Attributes::makeBool(
            "COMPUTEPERCENTILES",
            "Flag to control whether the 68.27 "
            "(1 sigma for normal distribution), the 95.45 (2 sigmas), "
            "the 99.73 (3 sigmas) and the 99.994 (4 sigmas) percentiles "
            "for the beam size and the normalized emittance should "
            "be computed. Default: false",
            computePercentiles);

    itsAttr[QM_MODE] = Attributes::makeString(
            "QM_MODE",
            "Storage mode for particle charge/mass. "
            "SINGLE uses one shared value per container; "
            "ATTRIBUTES stores per-particle values (accessible via per-particle "
            "`Q`/`M` views).",
            useQMAttributes ? std::string("ATTRIBUTES") : std::string("SINGLE"));

    itsAttr[AGGRESSIVE_STATE_SYNC] = Attributes::makeBool(
            "AGGRESSIVE_STATE_SYNC",
            "If true, every mutation of the shared BunchStateHandler flags "
            "(moments-dirty, unitless-positions, emitting-now) "
            "performs an MPI allreduce so that all ranks converge to the same "
            "value. Guards against rank-local divergence at the cost of an extra "
            "collective on every state change. Default: false.",
            aggressiveStateSync);

    itsAttr[LOADBALANCINGTHRESHOLD] = Attributes::makeReal(
            "LOADBALANCINGTHRESHOLD",
            "The threshold for triggering load balancing. If the ratio difference of particles in "
            "a rank exceeds this threshold, load balancing will be triggered. Default is 0.05 "
            "(5%). This threshold is only tested every `repartFreq` steps.",
            loadBalancingThreshold);

    itsAttr[LOADBALANCINGPAIRWEIGHT] = Attributes::makeReal(
            "LOADBALANCINGPAIRWEIGHT",
            "Share of the ORB load-balancing weight that follows the P3M particle-particle pair "
            "count; the rest follows the particle count. 0 balances particle counts only, 1 pair "
            "counts only. Values above 0 require the P3M field solver. Default is 0.",
            loadBalancingPairWeight);

    registerOwnership(AttributeHandler::STATEMENT);

    FileStream::setEcho(echo);

    // Start from the current option values, so that options not given in this command keep the
    // value set by an earlier OPTION command.
    Attributes::setBool(itsAttr[ECHO], echo);
    Attributes::setBool(itsAttr[INFO], info);
    Attributes::setBool(itsAttr[TRACE], mtrace);
    Attributes::setBool(itsAttr[WARN], warn);
    Attributes::setReal(itsAttr[SEED], seed);
    Attributes::setReal(itsAttr[PSDUMPFREQ], psDumpFreq);
    Attributes::setReal(itsAttr[STATDUMPFREQ], statDumpFreq);
    Attributes::setReal(itsAttr[CHECKPOINTFREQ], checkpointFreq);
    Attributes::setReal(itsAttr[STEPINFOFQ], stepInfoFreq);
    Attributes::setReal(itsAttr[PRINTRANKDISTRFQ], printRankDistrFreq);
    Attributes::setReal(itsAttr[REPARTFREQ], repartFreq);
    Attributes::setReal(itsAttr[MINBINEMITTED], minBinEmitted);
    Attributes::setBool(itsAttr[RHODUMP], rhoDump);
    Attributes::setBool(itsAttr[EBDUMP], ebDump);
    Attributes::setBool(itsAttr[RANKDUMP], rankDump);
    Attributes::setReal(itsAttr[AUTOPHASE], autoPhase);
    Attributes::setBool(itsAttr[ENABLEHDF5], enableHDF5);
    Attributes::setBool(itsAttr[ENABLEVTK], enableVTK);
    Attributes::setBool(itsAttr[ENABLELINEARTRANSFERMAPS], enableLinearTransferMaps);
    Attributes::setReal(itsAttr[LINEARTRANSFERMAPRICHARDSON], linearTransferMapRichardsonLevels);
    Attributes::setRealArray(
            itsAttr[LINEARTRANSFERMAPSTEPS],
            std::vector<double>(linearTransferMapSteps.begin(), linearTransferMapSteps.end()));
    Attributes::setPredefinedString(
            itsAttr[LINEARTRANSFERMAPINTEGRATOR], linearTransferMapIntegrator);
    Attributes::setBool(itsAttr[ASCIIDUMP], asciidump);
    Attributes::setReal(itsAttr[BOUNDPDESTROY], boundpDestroy);
    Attributes::setReal(itsAttr[BEAMHALOBOUNDARY], beamHaloBoundary);
    Attributes::setReal(itsAttr[VERSION], version);
    Attributes::setReal(itsAttr[HALOSHIFT], haloShift);
    Attributes::setBool(itsAttr[COMPUTEPERCENTILES], computePercentiles);
    Attributes::setString(
            itsAttr[QM_MODE], useQMAttributes ? std::string("ATTRIBUTES") : std::string("SINGLE"));
    Attributes::setBool(itsAttr[AGGRESSIVE_STATE_SYNC], aggressiveStateSync);
    Attributes::setReal(itsAttr[LOADBALANCINGTHRESHOLD], loadBalancingThreshold);
    Attributes::setReal(itsAttr[LOADBALANCINGPAIRWEIGHT], loadBalancingPairWeight);
}

Option::~Option() {}

void Option::execute() {
    // Validate the complete map configuration before publishing any of its globals.
    LinearTransferMapBuilder::Settings mapSettings;
    const double levels = Attributes::getReal(itsAttr[LINEARTRANSFERMAPRICHARDSON]);
    if (!std::isfinite(levels) || levels < 0.0 || std::floor(levels) != levels
        || levels > LinearTransferMapBuilder::Settings::maximumRichardsonLevels)
        throw OpalException(
                "Option::execute",
                "LINEARTRANSFERMAPRICHARDSON must be an integer from 0 through 4.");
    mapSettings.richardsonLevels = static_cast<unsigned>(levels);
    const auto steps             = Attributes::getRealArray(itsAttr[LINEARTRANSFERMAPSTEPS]);
    if (steps.size() != 6)
        throw OpalException(
                "Option::execute", "LINEARTRANSFERMAPSTEPS requires exactly six values.");
    std::copy(steps.begin(), steps.end(), mapSettings.finiteDifferenceSteps.begin());
    mapSettings.integrationMethod = ExternalFieldRayTracker::parseIntegrationMethod(
            Attributes::getString(itsAttr[LINEARTRANSFERMAPINTEGRATOR]));
    mapSettings.validate();

    // Store the option flags.
    echo       = Attributes::getBool(itsAttr[ECHO]);
    info       = Attributes::getBool(itsAttr[INFO]);
    mtrace     = Attributes::getBool(itsAttr[TRACE]);
    warn       = Attributes::getBool(itsAttr[WARN]);
    rhoDump    = Attributes::getBool(itsAttr[RHODUMP]);
    ebDump     = Attributes::getBool(itsAttr[EBDUMP]);
    rankDump   = Attributes::getBool(itsAttr[RANKDUMP]);
    enableHDF5 = Attributes::getBool(itsAttr[ENABLEHDF5]);
    enableVTK  = Attributes::getBool(itsAttr[ENABLEVTK]);

    enableLinearTransferMaps          = Attributes::getBool(itsAttr[ENABLELINEARTRANSFERMAPS]);
    linearTransferMapRichardsonLevels = mapSettings.richardsonLevels;
    linearTransferMapSteps            = mapSettings.finiteDifferenceSteps;
    linearTransferMapIntegrator =
            ExternalFieldRayTracker::integrationMethodName(mapSettings.integrationMethod);

    asciidump = Attributes::getBool(itsAttr[ASCIIDUMP]);
    version   = Attributes::getReal(itsAttr[VERSION]);
    seed      = Attributes::getReal(itsAttr[SEED]);

    haloShift                = Attributes::getReal(itsAttr[HALOSHIFT]);
    computePercentiles       = Attributes::getBool(itsAttr[COMPUTEPERCENTILES]);
    const std::string qmMode = Attributes::getString(itsAttr[QM_MODE]);
    if (qmMode == "ATTRIBUTES") {
        useQMAttributes = true;
    } else if (qmMode == "SINGLE") {
        useQMAttributes = false;
    } else {
        throw OpalException(
                "Option::execute",
                "Unsupported QM_MODE '" + qmMode + "'. Use \"SINGLE\" or \"ATTRIBUTES\".");
    }

    aggressiveStateSync = Attributes::getBool(itsAttr[AGGRESSIVE_STATE_SYNC]);

    // Set threshold and check if in the valid range [0, 1]
    loadBalancingThreshold = Attributes::getReal(itsAttr[LOADBALANCINGTHRESHOLD]);
    if (loadBalancingThreshold < 0.0 || loadBalancingThreshold > 1.0) {
        throw OpalException(
                "Option::execute",
                "LOADBALANCINGTHRESHOLD must be in the range [0, 1]. Current value: "
                        + std::to_string(loadBalancingThreshold));
    }

    loadBalancingPairWeight = Attributes::getReal(itsAttr[LOADBALANCINGPAIRWEIGHT]);
    if (!(loadBalancingPairWeight >= 0.0 && loadBalancingPairWeight <= 1.0)) {
        throw OpalException(
                "Option::execute",
                "LOADBALANCINGPAIRWEIGHT must be in the range [0, 1]. Current value: "
                        + std::to_string(loadBalancingPairWeight));
    }

    if (itsAttr[SEED]) {
        seed = int(Attributes::getReal(itsAttr[SEED]));
    }

    if (itsAttr[PSDUMPFREQ]) {
        psDumpFreq = int(Attributes::getReal(itsAttr[PSDUMPFREQ]));
        if (psDumpFreq < 0) {
            throw OpalException("Option::execute", "PSDUMPFREQ must be non-negative.");
        }
    }

    if (itsAttr[STATDUMPFREQ]) {
        statDumpFreq = int(Attributes::getReal(itsAttr[STATDUMPFREQ]));
        if (statDumpFreq < 0) {
            throw OpalException("Option::execute", "STATDUMPFREQ must be non-negative.");
        }
    }

    if (itsAttr[CHECKPOINTFREQ]) {
        checkpointFreq = int(Attributes::getReal(itsAttr[CHECKPOINTFREQ]));
        if (checkpointFreq < 0) {
            throw OpalException("Option::execute", "CHECKPOINTFREQ must be non-negative.");
        }
    }

    if (itsAttr[STEPINFOFQ]) {
        stepInfoFreq = int(Attributes::getReal(itsAttr[STEPINFOFQ]));
        if (stepInfoFreq < 0) stepInfoFreq = 0;
    }

    if (itsAttr[PRINTRANKDISTRFQ]) {
        printRankDistrFreq = int(Attributes::getReal(itsAttr[PRINTRANKDISTRFQ]));
        if (printRankDistrFreq < 0) printRankDistrFreq = 0;
    }

    if (itsAttr[REPARTFREQ]) {
        repartFreq = int(Attributes::getReal(itsAttr[REPARTFREQ]));
    }

    if (itsAttr[MINBINEMITTED]) {
        minBinEmitted = int(Attributes::getReal(itsAttr[MINBINEMITTED]));
    }

    if (itsAttr[AUTOPHASE]) {
        autoPhase = int(Attributes::getReal(itsAttr[AUTOPHASE]));
    }

    if (itsAttr[BOUNDPDESTROY]) {
        /*
         * Historically, BOUNDPDESTROYFQ was used as a positive frequency and
         * clamped to values >= 1 here. In OPALX, the same parameter now also
         * encodes the N-sigma boundary used in ParticleContainer::
         * markParticlesOutside(N). A value <= 0 is treated as "disabled"
         * by markParticlesOutside, so we must *not* clamp it to 1 anymore.
         */
        boundpDestroy = Attributes::getReal(itsAttr[BOUNDPDESTROY]);
    }

    if (itsAttr[BEAMHALOBOUNDARY]) {
        beamHaloBoundary = Attributes::getReal(itsAttr[BEAMHALOBOUNDARY]);
    } else {
        beamHaloBoundary = 0;
    }

    // Set message flags.
    FileStream::setEcho(echo);
}
