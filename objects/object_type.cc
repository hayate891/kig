// object_type.cc
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

#include "object_type.h"

#include "bogus_imp.h"

#include "../misc/i18n.h"
#include "../misc/coordinate.h"

#include <qstringlist.h>

const char* ObjectType::fullName() const
{
  return mfulltypename;
}

ObjectType::~ObjectType()
{
}

ObjectType::ObjectType( const char fulltypename[] )
  : mfulltypename( fulltypename )
{
}

bool ObjectType::canMove() const
{
  return false;
}

void ObjectType::move( RealObject*, const Coordinate&, const KigDocument& ) const
{
  assert( false );
}

bool ObjectType::inherits( int ) const
{
  return false;
}

ArgsParserObjectType::ArgsParserObjectType( const char fulltypename[],
                                            const struct ArgsParser::spec argsspec[],
                                            int n )
  : ObjectType( fulltypename ), margsparser( argsspec, n )
{
}

const ObjectImpType* ArgsParserObjectType::impRequirement( const ObjectImp* o, const Args& parents ) const
{
  return margsparser.impRequirement( o, parents );
}

const ArgsParser& ArgsParserObjectType::argsParser() const
{
  return margsparser;
}

bool ObjectType::isTransform() const
{
  return false;
}

QStringList ObjectType::specialActions() const
{
  return QStringList();
}

void ObjectType::executeAction( int, RealObject*, KigDocument&, KigWidget&,
                                NormalMode& ) const
{
  assert( false );
}

const Coordinate ObjectType::moveReferencePoint( const RealObject* ) const
{
  assert( false );
  return Coordinate::invalidCoord();
}

Objects ArgsParserObjectType::sortArgs( const Objects& args ) const
{
  return margsparser.parse( args );
}

DummyObjectType* DummyObjectType::instance()
{
  static DummyObjectType ret;
  return &ret;
}

ObjectImp* DummyObjectType::calc( const Args&, const KigDocument& ) const
{
  return new InvalidImp;
}

DummyObjectType::DummyObjectType()
  : ObjectType( "DummyObjectType" )
{

}

const ObjectImpType* DummyObjectType::impRequirement( const ObjectImp*, const Args& ) const
{
  return ObjectImp::stype();
}

const ObjectImpType* DummyObjectType::resultId() const
{
  return InvalidImp::stype();
}

Objects DummyObjectType::sortArgs( const Objects& args ) const
{
  return args;
}

DummyObjectType::~DummyObjectType()
{
}


