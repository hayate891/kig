// argsparser.h
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_MISC_ARGSPARSER_H
#define KIG_MISC_ARGSPARSER_H

#include <vector>

#include "../objects/common.h"

class ObjectImpType;

class ArgsChecker
{
public:
  virtual ~ArgsChecker();
  enum { Invalid = 0, Valid = 1, Complete = 2 };

  virtual int check( const Objects& os ) const = 0;
};

class CheckOneArgs
  : public ArgsChecker
{
public:
  int check( const Objects& os ) const;
};

class ArgParser
  : public ArgsChecker
{
public:
  struct spec { const ObjectImpType* type; const char* usetext; };
private:
  // the args spec..
  std::vector<spec> margs;
  // sometimes a random object is requested: any type goes.  Those
  // requests require some special treatment.  This vector holds the
  // usetexts for those requests..
  std::vector<const char*> manyobjsspec;

  spec findSpec( const ObjectImp* o, const Args& parents ) const;
public:
  ArgParser( const struct spec* args, int n );
  ArgParser( const std::vector<spec>& args );
  ArgParser( const std::vector<spec>& args, const std::vector<const char*> anyobjsspec );
  ArgParser();

  void initialize( const std::vector<spec>& args );
  void initialize( const struct spec* args, int n );
  // returns a new ArgParser that wants the same args, except for the
  // ones of the given type..
  ArgParser without( const ObjectImpType* type ) const;
  // checks if os matches the argument list this parser should parse..
  int check( const Objects& os ) const;
  int check( const Args& os ) const;
  // returns the usetext for the argument that o would be used for,
  // if sel were used as parents..
  // o should be in sel...
  const char* usetext( const ObjectImp* o, const Args& sel ) const;

  // this reorders the objects or args so that they are in the same
  // order as the requested arguments..
  Objects parse( const Objects& os ) const;
  Args parse( const Args& os ) const;

  // returns the minimal ObjectImp ID that o needs to inherit in order
  // to be useful..  o should be part of parents.
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
};

#endif
