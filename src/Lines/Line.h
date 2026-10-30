//
// Class Line
//   The LINE definition.
//   A Line contains an OPALX Beamline which represents the
//   sequence of elements in the line.  The line is always flat in the sense
//   that nested anonymous lines are flattened.
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
#ifndef OPAL_Line_HH
#define OPAL_Line_HH

#include "AbsBeamline/ElementBase.h"
#include "AbstractObjects/BeamSequence.h"
#include "Beamlines/Beamline.h"

#include <memory>

class TokenStream;

class Line : public BeamSequence {
    friend class LineTemplate;

public:
    /// Create the empty line called [b]name[/b]; the parser fills it.
    explicit Line(const std::string& name);

    virtual ~Line();

    /// Return line length.
    virtual double getLength() const;

    /// Make a line template.
    //  The template gets the name [b]name[/b], [b]is[/b] is ignored,
    //  and the formals and the line list are read from [b]stat[/b].
    virtual Object* makeTemplate(const std::string& name, TokenStream& is, Statement& stat);

    /// Parse the line object.
    //  Read the definition from [b]stat[/b].
    virtual void parse(Statement& stat);

    /// Print the line.
    virtual void print(std::ostream& stream) const;

protected:
    /// Constructor for LINE-compatible sequence statements.
    Line(const std::string& name, const char* keyword, const char* help);

    /// Parse a LINE-compatible definition.
    void parseDefinition(Statement& stat);

    /// Return the keyword used when printing this sequence.
    virtual const char* getSequenceKeyword() const;

    // Return the beam line: the elements of this sequence, in input order.
    virtual Beamline* fetchLine() const;

private:
    // Not implemented.
    Line(const Line&);
    void operator=(const Line&);

    // Parse sub-list.
    void parseList(Statement&);

    // The elements of this line; nested lines are copied in as their elements.
    std::unique_ptr<Beamline> line_m;
};

#endif  // OPAL_Line_HH
