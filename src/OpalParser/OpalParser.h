#ifndef OPAL_OpalParser_HH
#define OPAL_OpalParser_HH

// ------------------------------------------------------------------------
// $RCSfile: OpalParser.h,v $
// ------------------------------------------------------------------------
// $Revision: 1.1.1.1 $
// ------------------------------------------------------------------------
// Copyright: see Copyright.readme
// ------------------------------------------------------------------------
//
// Class OpalParser:
//
// ------------------------------------------------------------------------
//
// $Date: 2000/03/27 09:33:43 $
// $Author: Andreas Adelmann $
//
// ------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "OpalParser/Parser.h"

class Object;
class Token;
class Statement;

// Class OpalParser
// ------------------------------------------------------------------------
/// The default parser for OPAL-9.
//  The parser reads a command name and keyword, creates a new object for
//  the keyword (see makeObject()) and calls the parser of that object.  If
//  that parser succeeds, it calls the object's execute() function.  As
//  required, it first updates the data structure to ensure that everything
//  is up-to-date.  Optionally, command execution is also traced.
//
//  Derived parsers may override makeObject(), thus changing the set
//  of recognized commands.

class OpalParser : public Parser {
public:
    OpalParser();
    virtual ~OpalParser();

    /// Parse and execute current statement.
    virtual void parse(Statement&) const;

    /// Read complete statement from a token stream.
    virtual Statement* readStatement(TokenStream*) const;

    /// Read current stream.
    //  Read, parse, and execute statements one at a time.
    virtual void run() const;

    /// Read given stream.
    //  Switch to given stream, then read, parse, and execute statements
    //  one at a time. Used for CALL statements and macros.
    //  Takes ownership of the given stream, including on exceptions.
    virtual void run(TokenStream*) const;

    /// Set stop flag.
    //  Causes [tt]run()[/tt] to return when the next statement should be
    //  read.
    void stop() const;

    /// Return next input token.
    static Token readToken();

protected:
    /// Execute or check the current command.
    void execute(Object*, const std::string&) const;

    /// Create a new object called [b]name[/b] for the input keyword [b]keyword[/b].
    //  Return nullptr, if [b]keyword[/b] is not a keyword of this parser.
    virtual std::unique_ptr<Object> makeObject(
            const std::string& keyword, const std::string& name) const;

    /// Parse executable command.
    virtual void parseAction(Statement&) const;

    /// Parse assignment statement.
    virtual void parseAssign(Statement&) const;

    /// Parse definition.
    virtual void parseDefine(Statement&) const;

    /// Check for end of statement.
    virtual void parseEnd(Statement&) const;

    /// Parse macro definition or call.
    virtual void parseMacro(const std::string& name, Statement&) const;

    /// Print help on named command.
    virtual void printHelp(const std::string&) const;

private:
    // Not implemented.
    OpalParser(const OpalParser&);
    void operator=(const OpalParser&);

    // Parse a bracketed token list into statement.
    static void parseBracketList(char close, Statement&);

    // Parse a token list into statement.
    static void parseTokenList(Statement&);

    // get hint on cause of error
    static std::string getHint(const std::string&, const std::string& = "attribute");

    // This flag is set by all commands which return from a mode.
    mutable bool stopFlag;

    // The stack of input streams ("CALL", "MACRO", etc.).
    static std::vector<std::shared_ptr<TokenStream> > inputStack;
};

#endif  // OPAL_OpalParser_HH
