// object.cc
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

#include "object.h"

#include "object_type.h"
#include "object_imp.h"
#include "curve_imp.h"
#include "bogus_imp.h"
#include "../misc/kigpainter.h"
#include "../misc/calcpaths.h"
#include "../kig/kig_part.h"

#include <qpen.h>
#include <assert.h>
#include <functional>
#include <algorithm>

using namespace std;

RealObject::RealObject( const ObjectType* type, const Objects& parents )
  : ObjectWithParents( type->sortArgs( parents ) ),
    mcolor( Qt::blue ), mselected( false ), mshown( true ),
    mwidth( -1 ), mtype( type ), mimp( new InvalidImp )
{
}

RealObject::~RealObject()
{
  delete mimp;
}

void RealObject::draw( KigPainter& p, bool ss ) const
{
  if ( ! mshown ) return;
  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( mselected && ss ? Qt::red : mcolor );
  p.setPen( QPen ( mselected && ss ? Qt::red : mcolor,  1) );
  p.setWidth( mwidth );
  mimp->draw( p );
}

bool RealObject::contains( const Coordinate& p, const KigWidget& w ) const
{
  return mimp->contains( p, mwidth, w );
}

bool RealObject::inRect( const Rect& r, const KigWidget& w ) const
{
  return mimp->inRect( r, mwidth, w );
}

void RealObject::move( const Coordinate& to, const KigDocument& d )
{
  mtype->move( this, to, d );
}

void ObjectWithParents::calc( const KigDocument& d )
{
  using namespace std;
  Args a;
  a.reserve( mparents.size() );
  transform( mparents.begin(), mparents.end(),
             back_inserter( a ), mem_fun( &Object::imp ) );
  calc( a, d );
}

bool RealObject::canMove() const
{
  return mtype->canMove();
}

Object::~Object()
{
  assert( mchildren.empty() );
}

bool Object::hasimp( const ObjectImpType* type ) const
{
  assert( imp() );
  return imp()->inherits( type );
}

const uint Object::numberOfProperties() const
{
  return imp()->numberOfProperties();
}

ObjectImp* Object::property( uint which, const KigDocument& w ) const
{
  return imp()->property( which, w );
}

const QCStringList Object::propertiesInternalNames() const
{
  return imp()->propertiesInternalNames();
}

const QCStringList Object::properties() const
{
  return imp()->properties();
}

const Objects Object::children() const
{
  return mchildren;
}

const Objects Object::getAllChildren() const
{
  Objects ret;
  // objects to iterate over...
  Objects objs = mchildren;
  // contains the objects to iterate over the next time around...
  Objects objs2;
  while( !objs.empty() )
  {
    for( Objects::iterator i = objs.begin();
         i != objs.end(); ++i )
    {
      ret.upush( *i );
      objs2 |= (*i)->children();
    };
    objs = objs2;
    objs2.clear();
  };
  return ret;
}

void Object::addChild( Object* o )
{
  mchildren.push_back( o );
}

void Object::delChild( Object* o )
{
  // the amount of children we have works like a reference count.  If
  // there are none, we commit suicide..
  mchildren.remove_first( o );
  if ( mchildren.empty() )
    delete this;
}

void ObjectWithParents::addParents( const Objects& os )
{
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    addParent( *i );
}

void ObjectWithParents::addParent( Object* o )
{
  mparents.push_back( o );
  o->addChild( this );
}

void ObjectWithParents::delParent( Object* o )
{
  mparents.remove( o );
  o->delChild( this );
}

void ReferenceObject::clearParents()
{
  for ( Objects::iterator i = mparents.begin(); i != mparents.end(); ++i )
    (*i)->delChild( this );
  mparents.clear();
}

void ObjectWithParents::setParents( const Objects& parents )
{
  for ( uint i = 0; i < parents.size(); ++i )
    parents[i]->addChild( this );
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->delChild( this );
  mparents = parents;
}

Objects ObjectWithParents::parents() const
{
  return mparents;
}

ObjectWithParents::~ObjectWithParents()
{
  // tell our parents that we're dead...
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->delChild( this );
}

const ObjectImp* RealObject::imp() const
{
  return mimp;
}

const ObjectType* RealObject::type() const
{
  return mtype;
}

void RealObject::calc( const Args& a, const KigDocument& d )
{
  // don't delete the imp until we have a new one to replace it
  // with..
  ObjectImp* imp = mtype->calc( a, d );
  delete mimp;
  mimp = imp;
}

Object::Object()
{
}

ObjectWithParents::ObjectWithParents( const Objects& parents )
  : mparents( parents )
{
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->addChild( this );
}

DataObject::DataObject( ObjectImp* imp )
  : mimp( imp )
{
}

DataObject::~DataObject()
{
  delete mimp;
}

const ObjectImp* DataObject::imp() const
{
  return mimp;
}

Objects DataObject::parents() const
{
  return Objects();
}

void DataObject::draw( KigPainter&, bool ) const
{
}

bool DataObject::contains( const Coordinate&, const KigWidget& ) const
{
  return false;
}

bool DataObject::inRect( const Rect&, const KigWidget& ) const
{
  return false;
}

void DataObject::calc( const KigDocument& )
{
}

void Object::setParents( const Objects& )
{
  assert( false );
}

bool Object::inherits( int ) const
{
  return false;
}

bool RealObject::inherits( int type ) const
{
  return type == ID_RealObject;
}

ObjectImp* DataObject::switchImp( ObjectImp* imp )
{
  ObjectImp* ret = mimp;
  mimp = imp;
  return ret;
}

void DataObject::setImp( ObjectImp* imp )
{
  delete mimp;
  mimp = imp;
}

bool DataObject::inherits( int type ) const
{
  return type == ID_DataObject;
}

bool Object::valid() const
{
  return imp()->valid();
}

bool RealObject::shown() const
{
  return mshown;
}

bool DataObject::shown() const
{
  return false;
}

void RealObject::setType( const ObjectType* t )
{
  mtype = t;
}

void RealObject::setSelected( bool s )
{
  mselected = s;
}

void Object::setSelected( bool )
{
}

void Object::setShown( bool )
{
}

void RealObject::setShown( bool shown )
{
  mshown = shown;
}

PropertyObject::PropertyObject( Object* parent, int id )
  : Object(), mimp( new InvalidImp ), mparent( parent ), mpropid( id )
{
  mparent->addChild( this );
}

PropertyObject::~PropertyObject()
{
  mparent->delChild( this );
  delete mimp;
}

bool PropertyObject::inherits( int type ) const
{
  return type == ID_PropertyObject;
}

const ObjectImp* PropertyObject::imp() const
{
  return mimp;
}

Objects PropertyObject::parents() const
{
  return Objects( mparent );
}

void PropertyObject::draw( KigPainter&, bool ) const
{
}

bool PropertyObject::contains( const Coordinate&, const KigWidget& ) const
{
  return false;
}

bool PropertyObject::inRect( const Rect&, const KigWidget& ) const
{
  return false;
}

void PropertyObject::calc( const KigDocument& d )
{
  ObjectImp* imp = mparent->property( mpropid, d );
  delete mimp;
  mimp = imp;
}

bool PropertyObject::shown() const
{
  return false;
}

const Object* PropertyObject::parent() const
{
  return mparent;
}

int PropertyObject::propId() const
{
  return mpropid;
}

void Object::setColor( const QColor& )
{
}

void RealObject::setColor( const QColor& c )
{
  mcolor = c;
}

bool Object::canMove() const
{
  return false;
}

void Object::move( const Coordinate&, const KigDocument& )
{
  assert( false );
}

QColor Object::color() const
{
  assert( false );
  return Qt::black;
}

QColor RealObject::color() const
{
  return mcolor;
}

void RealObject::setWidth( int width )
{
  mwidth = width;
}

const ObjectImpType* RealObject::impRequirement( Object* o, const Objects& os ) const
{
  Args args;
  args.reserve( mparents.size() );
  transform( os.begin(), os.end(),
             back_inserter( args ), mem_fun( &Object::imp ) );
  assert( find( args.begin(), args.end(), o->imp() ) != args.end() );
  return mtype->impRequirement( o->imp(), args );
}

const ObjectImpType* Object::impRequirement( Object*, const Objects& ) const
{
  assert( false );
  return ObjectImp::stype();
}

const ObjectImpType* PropertyObject::impRequirement( Object*, const Objects& ) const
{
  return mparent->impRequirementForProperty( mpropid );
}

const ObjectImpType* Object::impRequirementForProperty( uint which ) const
{
  return imp()->impRequirementForProperty( which );
}

void RealObject::calc( const KigDocument& d )
{
  ObjectWithParents::calc( d );
}

const char* Object::iconForProperty( uint which ) const
{
  return imp()->iconForProperty( which );
}

Object* PropertyObject::parent()
{
  return mparent;
}

ReferenceObject::ReferenceObject( Object* o )
  : ObjectWithParents( Objects( o ) )
{
}

ReferenceObject::ReferenceObject( const Objects& os )
  : ObjectWithParents( os )
{
}

ReferenceObject::~ReferenceObject()
{
}

const ObjectImp* ReferenceObject::imp() const
{
  return new InvalidImp;
}

void ReferenceObject::draw( KigPainter&, bool ) const
{
  return;
}

bool ReferenceObject::inRect( const Rect&, const KigWidget& ) const
{
  return false;
}

bool ReferenceObject::shown() const
{
  return false;
}

void ReferenceObject::calc( const Args&, const KigDocument& )
{
  return;
}

bool ReferenceObject::contains( const Coordinate&, const KigWidget& ) const
{
  return false;
}

ReferenceObject::ReferenceObject()
  : ObjectWithParents( Objects() )
{
}

const Coordinate Object::moveReferencePoint() const
{
  return Coordinate::invalidCoord();
}

const Coordinate RealObject::moveReferencePoint() const
{
  return mtype->moveReferencePoint( this );
}

void RealObject::setParents( const Objects& parents )
{
  Objects sorted = mtype->sortArgs( parents );
  ObjectWithParents::setParents( sorted );
}