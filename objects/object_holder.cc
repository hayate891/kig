// object_holder.cc
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#include "object_holder.h"

#include "bogus_imp.h"
#include "object_calcer.h"
#include "object_drawer.h"

#include "../misc/coordinate.h"

ObjectHolder::ObjectHolder( ObjectCalcer* calcer )
  : mcalcer( calcer ), mdrawer( new ObjectDrawer ), mnamecalcer( 0 )
{
}

ObjectHolder::ObjectHolder( ObjectCalcer* calcer, ObjectDrawer* drawer,
                            ObjectConstCalcer* namecalcer )
  : mcalcer( calcer ), mdrawer( drawer ), mnamecalcer( namecalcer )
{
  assert( !namecalcer || namecalcer->imp()->inherits( StringImp::stype() ) );
}

ObjectHolder::ObjectHolder( ObjectCalcer* calcer, ObjectDrawer* drawer )
  : mcalcer( calcer ), mdrawer( drawer ), mnamecalcer( 0 )
{
}

ObjectHolder::~ObjectHolder()
{
  delete mdrawer;
}

const ObjectImp* ObjectHolder::imp() const
{
  return mcalcer->imp();
}

const ObjectCalcer* ObjectHolder::calcer() const
{
  return mcalcer.get();
}

const ObjectDrawer* ObjectHolder::drawer() const
{
  return mdrawer;
}

const ObjectConstCalcer* ObjectHolder::nameCalcer() const
{
  return mnamecalcer.get();
}

void ObjectHolder::setDrawer( ObjectDrawer* d )
{
  delete switchDrawer( d );
}

void ObjectHolder::calc( const KigDocument& d )
{
  mcalcer->calc( d );
}

void ObjectHolder::draw( KigPainter& p, bool selected ) const
{
  mdrawer->draw( *imp(), p, selected );
}

bool ObjectHolder::contains( const Coordinate& p, const KigWidget& w ) const
{
  return mdrawer->contains( *imp(), p, w );
}

bool ObjectHolder::inRect( const Rect& r, const KigWidget& w ) const
{
  return mdrawer->inRect( *imp(), r, w );
}

ObjectCalcer* ObjectHolder::calcer()
{
  return mcalcer.get();
}

ObjectDrawer* ObjectHolder::drawer()
{
  return mdrawer;
}

ObjectConstCalcer* ObjectHolder::nameCalcer()
{
  return mnamecalcer.get();
}

const Coordinate ObjectHolder::moveReferencePoint() const
{
  return mcalcer->moveReferencePoint();
}

void ObjectHolder::move( const Coordinate& to, const KigDocument& doc )
{
  mcalcer->move( to, doc );
}

bool ObjectHolder::canMove() const
{
  return mcalcer->canMove();
}

ObjectDrawer* ObjectHolder::switchDrawer( ObjectDrawer* d )
{
  ObjectDrawer* tmp = mdrawer;
  mdrawer = d;
  return tmp;
}

bool ObjectHolder::shown() const
{
  return mdrawer->shown();
}

const QString ObjectHolder::name() const
{
  if ( mnamecalcer )
  {
    assert( mnamecalcer->imp()->inherits( StringImp::stype() ) );
    return static_cast<const StringImp*>( mnamecalcer->imp() )->data();
  }
  else
    return QString::null;
}

void ObjectHolder::setNameCalcer( ObjectConstCalcer* namecalcer )
{
  assert( !mnamecalcer );
  mnamecalcer = namecalcer;
}

QString ObjectHolder::selectStatement() const
{
  const QString n = name();
  if ( n.isNull() )
    return i18n( imp()->type()->selectStatement() );
  else
    return i18n( imp()->type()->selectNameStatement() ).arg( n );
}