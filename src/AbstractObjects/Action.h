#ifndef OPAL_Action_HH
#define OPAL_Action_HH

// ------------------------------------------------------------------------
// $RCSfile: Action.h,v $
// ------------------------------------------------------------------------
// $Revision: 1.1.1.1 $
// ------------------------------------------------------------------------
// Copyright: see Copyright.readme
// ------------------------------------------------------------------------
//
// Class: Action
//
// ------------------------------------------------------------------------
//
// $Date: 2000/03/27 09:33:34 $
// $Author: Andreas Adelmann $
//
// ------------------------------------------------------------------------

#include "AbstractObjects/Object.h"

// Class Action
// ------------------------------------------------------------------------
/// The base class for all OPAL actions.
//  It implements the common behavior of actions, it can also be used via
//  dynamic casting to determine whether an object represents an action.

class Action : public Object {
public:
    virtual ~Action();

    /// Return the object category as a string.
    //  Return the string "ACTION".
    virtual const std::string getCategory() const;

    /// Trace flag.
    //  If true, the object's execute() function should be traced.
    //  Always true for actions.
    virtual bool shouldTrace() const;

    /// Update flag.
    //  If true, the data structure should be updated before calling execute().
    //  Always true for actions.
    virtual bool shouldUpdate() const;

protected:
    /// Create an object called [b]name[/b] for the input keyword [b]keyword[/b].
    Action(const std::string& name, int size, const char* keyword, const char* help);

private:
    // Not implemented.
    Action();
    Action(const Action&);
    void operator=(const Action&);
};

#endif  // OPAL_Action_HH
