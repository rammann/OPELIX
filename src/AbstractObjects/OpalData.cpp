//
// Class OpalData
//   The global OPAL structure.
//   The OPAL object holds all global data required for a OPAL execution.
//   In particular it contains the main Directory, which allows retrieval
//   of command objects by their name.  For other data refer to the
//   implementation file.
//
// Copyright (c) 200x - 2021, Paul Scherrer Institut, Villigen PSI, Switzerland
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
#include "AbstractObjects/OpalData.h"

#include "AbstractObjects/Attribute.h"
#include "AbstractObjects/Directory.h"
#include "AbstractObjects/Expressions.h"
#include "AbstractObjects/Object.h"
#include "AbstractObjects/ValueDefinition.h"
#include "Attributes/Attributes.h"
#include "OpalParser/FileStream.h"
#include "OpalParser/OpalParser.h"
#include "PartBunch/PartBunch.h"
#include "Physics/Units.h"
#include "Structure/BoundaryGeometry.h"
#include "Structure/DataSink.h"
#include "Utilities/OpalException.h"
#include "Utilities/Options.h"
#include "ValueDefinitions/RealVariable.h"
#include "ValueDefinitions/StringConstant.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <set>

// Struct OpalDataImpl.
// ------------------------------------------------------------------------
struct OpalDataImpl {
    OpalDataImpl();
    ~OpalDataImpl();

    // The main object directory.
    Directory mainDirectory;

    // The value of the global momentum.
    ValueDefinition* referenceMomentum;

    // The flag telling that something has changed.
    bool modified;

    // The set of expressions to be invalidated when something changes.
    std::set<AttributeBase*> exprDirectory;
    typedef std::set<AttributeBase*>::iterator exprIterator;

    bool hasPriorRun_m;

    // true if we restart a simulation
    bool isRestart_m;

    // Where to resume in a restart run
    int restartStep_m;

    // Where to resume in a restart run
    std::string restartFn_m;

    // true if the name of a restartFile is specified
    bool hasRestartFile_m;

    // dump frequency as found in restart file
    int restart_dump_freq_m;

    // Input file name
    std::string inputFn_m;

    std::set<std::string> outFiles_m;

    /// Mode for writing files
    OpalData::OpenMode openMode_m = OpalData::OpenMode::WRITE;

    bool hasBunchAllocated_m;
    // The particle bunch to be tracked.
    PartBunch_t* bunch_m;

    bool hasDataSinkAllocated_m;

    DataSink* dataSink_m;

    // In units of seconds. This is half of tEmission
    double gPhaseShift_m;

    BoundaryGeometry* bg_m;

    std::vector<MaxPhasesT> maxPhases_m;

    bool isInOPALCyclMode_m;
    bool isInOPALTMode_m;
    bool isOptimizerFlag_m;
    bool isInPrepState_m;

    std::vector<std::string> arguments_m;
};

OpalDataImpl::OpalDataImpl()
    : mainDirectory(),
      referenceMomentum(0),
      modified(false),
      hasPriorRun_m(false),
      isRestart_m(false),
      restartStep_m(0),
      hasRestartFile_m(false),
      restart_dump_freq_m(1),
      hasBunchAllocated_m(false),
      hasDataSinkAllocated_m(false),
      gPhaseShift_m(0.0),
      isInOPALCyclMode_m(false),
      isInOPALTMode_m(false),
      isOptimizerFlag_m(false),
      isInPrepState_m(false) {
    bunch_m    = nullptr;
    dataSink_m = nullptr;
    bg_m       = nullptr;
}

OpalDataImpl::~OpalDataImpl() {
    delete bunch_m;
    delete bg_m;
    delete dataSink_m;

    mainDirectory.erase();
    exprDirectory.clear();
}

bool OpalData::isInstantiated = false;
OpalData* OpalData::instance  = nullptr;

OpalData* OpalData::getInstance() {
    if (!isInstantiated) {
        instance       = new OpalData();
        isInstantiated = true;
        return instance;
    } else {
        return instance;
    }
}

void OpalData::deleteInstance() {
    delete instance;
    instance       = nullptr;
    isInstantiated = false;
}

OpalData::OpalData() { p = new OpalDataImpl(); }

OpalData::~OpalData() { delete p; }

void OpalData::reset() {
    delete p;
    p                         = new OpalDataImpl();
    p->hasPriorRun_m          = false;
    p->isRestart_m            = false;
    p->hasRestartFile_m       = false;
    p->hasBunchAllocated_m    = false;
    p->hasDataSinkAllocated_m = false;
    p->gPhaseShift_m          = 0.0;
    p->maxPhases_m.clear();
    p->isInOPALCyclMode_m = false;
    p->isInOPALTMode_m    = false;
    p->isInPrepState_m    = false;
    p->isOptimizerFlag_m  = false;
}

bool OpalData::isInOPALCyclMode() { return p->isInOPALCyclMode_m; }

bool OpalData::isInOPALTMode() { return p->isInOPALTMode_m; }

bool OpalData::isOptimizerRun() { return p->isOptimizerFlag_m; }

void OpalData::setInOPALCyclMode() { p->isInOPALCyclMode_m = true; }

void OpalData::setInOPALTMode() { p->isInOPALTMode_m = true; }

void OpalData::setOptimizerFlag() { p->isOptimizerFlag_m = true; }

bool OpalData::isInPrepState() { return p->isInPrepState_m; }

void OpalData::setInPrepState(bool state) { p->isInPrepState_m = state; }

OpalData::PreparationState::PreparationState(OpalData& opalData) : opalData_m(opalData) {
    opalData_m.setInPrepState(true);
}

OpalData::PreparationState::~PreparationState() noexcept { finish(); }

void OpalData::PreparationState::finish() noexcept {
    if (active_m) {
        opalData_m.setInPrepState(false);
        active_m = false;
    }
}

OpalData::PreparationState OpalData::enterPreparationState() { return PreparationState(*this); }

bool OpalData::hasPriorTrack() { return p->hasPriorRun_m; }

void OpalData::setPriorTrack(const bool& value) { p->hasPriorRun_m = value; }

bool OpalData::inRestartRun() { return p->isRestart_m; }

void OpalData::setRestartRun(const bool& value) { p->isRestart_m = value; }

void OpalData::setRestartStep(int s) { p->restartStep_m = s; }

int OpalData::getRestartStep() { return p->restartStep_m; }

std::string OpalData::getRestartFileName() { return p->restartFn_m; }

void OpalData::setRestartFileName(std::string s) {
    p->restartFn_m      = s;
    p->hasRestartFile_m = true;
}

bool OpalData::hasRestartFile() { return p->hasRestartFile_m; }

void OpalData::setRestartDumpFreq(const int& N) { p->restart_dump_freq_m = N; }

int OpalData::getRestartDumpFreq() const { return p->restart_dump_freq_m; }

void OpalData::setOpenMode(OpenMode openMode) { p->openMode_m = openMode; }

OpalData::OpenMode OpalData::getOpenMode() const { return p->openMode_m; }

bool OpalData::hasBunchAllocated() { return p->hasBunchAllocated_m; }

void OpalData::bunchIsAllocated() { p->hasBunchAllocated_m = true; }

void OpalData::setPartBunch(PartBunch_t* b) { p->bunch_m = b; }

PartBunch_t* OpalData::getPartBunch() { return p->bunch_m; }

bool OpalData::hasDataSinkAllocated() { return p->hasDataSinkAllocated_m; }

void OpalData::setDataSink(DataSink* s) {
    p->dataSink_m             = s;
    p->hasDataSinkAllocated_m = true;
}

DataSink* OpalData::getDataSink() { return p->dataSink_m; }

void OpalData::setMaxPhase(std::string elName, double phi) {
    p->maxPhases_m.push_back(MaxPhasesT(elName, phi));
}

std::vector<MaxPhasesT>::iterator OpalData::getFirstMaxPhases() { return p->maxPhases_m.begin(); }

std::vector<MaxPhasesT>::iterator OpalData::getLastMaxPhases() { return p->maxPhases_m.end(); }

int OpalData::getNumberOfMaxPhases() { return p->maxPhases_m.size(); }

void OpalData::setGlobalPhaseShift(double shift) {
    /// units: (sec)
    p->gPhaseShift_m = shift;
}

double OpalData::getGlobalPhaseShift() {
    /// units: (sec)
    return p->gPhaseShift_m;
}

void OpalData::setGlobalGeometry(BoundaryGeometry* bg) { p->bg_m = bg; }

bool OpalData::hasGlobalGeometry() { return p->bg_m != nullptr; }

void OpalData::apply(const std::function<void(Object*)>& fun) {
    for (ObjectDir::iterator i = p->mainDirectory.begin(); i != p->mainDirectory.end(); ++i) {
        fun(&*i->second);
    }
}

void OpalData::create(Object* newObject) {
    // Test for existing node with same name.
    const std::string name = newObject->getOpalName();
    Object* oldObject      = p->mainDirectory.find(name);

    if (oldObject != nullptr) {
        throw OpalException(
                "OpalData::create()", "You cannot replace the object \"" + name + "\".");
    } else {
        p->mainDirectory.insert(name, newObject);
    }
}

void OpalData::define(Object* newObject) {
    // Test for existing node with same name.
    const std::string name = newObject->getOpalName();
    Object* oldObject      = p->mainDirectory.find(name);

    if (oldObject != nullptr && oldObject != newObject) {
        // Attempt to replace an object.
        if (oldObject->isBuiltin() || !oldObject->canReplaceBy(newObject)) {
            throw OpalException(
                    "OpalData::define()", "You cannot replace the object \"" + name + "\".");
        } else {
            if (Options::info) {
                *ippl::Info << "Replacing the object \"" << name << "\"." << endl;
            }

            // Replace all references to this object.
            for (ObjectDir::iterator i = p->mainDirectory.begin(); i != p->mainDirectory.end();
                 ++i) {
                (*i).second->replace(oldObject, newObject);
            }

            // Remove old object.
            erase(name);
        }
    }

    // Force re-evaluation of expressions.
    p->modified = true;
    p->mainDirectory.insert(name, newObject);

    // If this is a new definition of "P0", insert its definition.
    if (name == "P0") {
        if (ValueDefinition* p0 = dynamic_cast<ValueDefinition*>(newObject)) {
            setP0(p0);
        }
    }
}

void OpalData::erase(const std::string& name) {
    Object* oldObject = p->mainDirectory.find(name);

    if (oldObject != nullptr) {
        // Relink all children of "this" to "this->getParent()".
        for (ObjectDir::iterator i = p->mainDirectory.begin(); i != p->mainDirectory.end(); ++i) {
            Object* child = &*i->second;
            if (child->getParent() == oldObject) {
                child->setParent(oldObject->getParent());
            }
        }
        // Remove the object.
        p->mainDirectory.erase(name);
    }
}

Object* OpalData::find(const std::string& name) { return p->mainDirectory.find(name); }

double OpalData::getP0() const { return p->referenceMomentum->getReal() * Units::GeV2eV; }

void OpalData::makeDirty() { p->modified = true; }

void OpalData::registerExpression(AttributeBase* expr) { p->exprDirectory.insert(expr); }

void OpalData::unregisterExpression(AttributeBase* expr) { p->exprDirectory.erase(expr); }

void OpalData::setP0(ValueDefinition* p0) { p->referenceMomentum = p0; }

void OpalData::storeInputFn(const std::string& fn) { p->inputFn_m = fn; }

std::string OpalData::getAuxiliaryOutputDirectory() const { return "data"; }

std::string OpalData::getInputFn() { return p->inputFn_m; }

std::string OpalData::getInputBasename() {
    std::string& fn = p->inputFn_m;
    int const pdot  = fn.rfind(".");
    return fn.substr(0, pdot);
}

void OpalData::checkAndAddOutputFileName(const std::string& outfn) {
    if (p->outFiles_m.count(outfn) == 0) {
        p->outFiles_m.insert(outfn);
    } else if (!hasBunchAllocated()) {
        throw OpalException(
                "OpalData::checkAndAddOutputFileName",
                "Duplicate file name for output, '" + outfn + "', detected");
    }
}

void OpalData::update() {
    Inform msg("OpalData ");
    if (p->modified) {
        // Force re-evaluation of expressions.
        for (OpalDataImpl::exprIterator i = p->exprDirectory.begin(); i != p->exprDirectory.end();
             ++i) {
            (*i)->invalidate();
        }

        // Update all definitions.
        for (ObjectDir::iterator i = p->mainDirectory.begin(); i != p->mainDirectory.end(); ++i) {
            (*i).second->update();
        }

        // Definitions are up-to-date.
        p->modified = false;
    }
}

void OpalData::storeArguments(int argc, char* argv[]) {
    p->arguments_m.clear();
    for (int i = 0; i < argc; ++i) {
        p->arguments_m.push_back(argv[i]);
    }
}

std::vector<std::string> OpalData::getArguments() { return p->arguments_m; }
