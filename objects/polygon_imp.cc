// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>

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

#include "polygon_imp.h"

#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"

#include "../misc/common.h"
#include "../misc/coordinate.h"
#include "../misc/kigpainter.h"
#include "../misc/kigtransform.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"

#include <klocale.h>

#include <math.h>

PolygonImp::PolygonImp( const std::vector<Coordinate>& points )
{
  mpoints = points;
}

PolygonImp::~PolygonImp()
{
}

ObjectImp* PolygonImp::transform( const Transformation& t ) const
{
/*mp:
 * any projective transformation makes sense for a polygon,
 * since segments transform into segments (but see below...)
 * of course regular polygons will no longer be
 * regular if t is not homothetic.
 * for projective transformations the polygon could transform to
 * an unbounded nonconnected polygon; this happens if some side
 * of the polygon crosses the critical line that maps to infinity
 * this can be easily checked using the getProjectiveIndicator
 * function
 */
//  if ( ! t.isHomothetic() )
//    return new InvalidImp();

  if ( ! t.isAffine() )     /* in this case we need a more extensive test */
  {
    double maxp = -1.0;
    double minp = 1.0;
    for ( uint i = 0; i < mpoints.size(); ++i )
    {
      double p = t.getProjectiveIndicator( mpoints[i] );
      if ( p > maxp ) maxp = p;
      if ( p < minp ) minp = p;
    }
    if ( maxp > 0 && minp < 0 ) return new InvalidImp;
  }
  std::vector<Coordinate> np;
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    Coordinate nc = t.apply( mpoints[i] );
    if ( !nc.valid() )
      return new InvalidImp;
    np.push_back( nc );
  }
  return new PolygonImp( np );
}

void PolygonImp::draw( KigPainter& p ) const
{
  p.drawPolygon( mpoints );
}

#define selectpolygonwithinside 1
#ifdef selectpolygonwithinside
bool PolygonImp::contains( const Coordinate& p, int, const KigWidget& ) const
{
  // (algorithm sent to me by domi)
  // We intersect with the horizontal ray from point to the right and
  // count the number of intersections.  That, along with some
  // minor optimalisations of the intersection test..
  bool inside_flag = false;
  double cx = p.x;
  double cy = p.y;

  Coordinate prevpoint = mpoints.back();
  bool prevpointbelow = mpoints.back().y >= cy;
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    Coordinate point = mpoints[i];
    bool pointbelow = point.y >= cy;
    if ( prevpointbelow != pointbelow )
    {
      // possibility of intersection: points on different side from
      // the X axis
      bool rightofpt = point.x >= cx;
      if ( rightofpt == ( prevpoint.x >= cx ) )
      {
        // points on same side of Y axis -> easy to test intersection
        // intersection iff one point to the right of c
        if ( rightofpt )
          inside_flag = !inside_flag;
      }
      else
      {
        // points on different sides of Y axis -> we need to calculate
        // the intersection.
        if ( ( point.x - ( point.y - cy )*( prevpoint.x-point.x )/( prevpoint.y - point.y ) ) >= cx )
          inside_flag = !inside_flag;
      }
    }
    prevpoint = point;
    prevpointbelow = pointbelow;
  }
  return inside_flag;
}
#else
bool PolygonImp::contains( const Coordinate& p, int width, const KigWidget& w ) const
{
  bool ret = false;
  uint reduceddim = mpoints.size() - 1;
  for ( uint i = 0; i < reduceddim; ++i )
  {
    ret |= isOnSegment( p, mpoints[i], mpoints[i+1], w.screenInfo().normalMiss( width ) );
  }
  ret |= isOnSegment( p, mpoints[reduceddim], mpoints[0], w.screenInfo().normalMiss( width ) );

  return ret;
}
#endif

bool PolygonImp::inRect( const Rect& r, int width, const KigWidget& w ) const
{
  bool ret = false;
  uint reduceddim = mpoints.size() - 1;
  for ( uint i = 0; i < reduceddim; ++i )
  {
    SegmentImp* s = new SegmentImp( mpoints[i], mpoints[i+1] );
    ret |= lineInRect( r, mpoints[i], mpoints[i+1], width, s, w );
    delete s;
    s = 0;
  }
  SegmentImp* t = new SegmentImp( mpoints[reduceddim], mpoints[0] );
  ret |= lineInRect( r, mpoints[reduceddim], mpoints[0], width, t, w );
  delete t;
  t = 0;

  return ret;
}

bool PolygonImp::valid() const
{
  return true;
}

const uint PolygonImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 3;
}

const QCStringList PolygonImp::propertiesInternalNames() const
{
  QCStringList l = Parent::propertiesInternalNames();
  l += "polygon-perimeter";
  l += "polygon-surface";
  l += "center-of-mass";
  assert( l.size() == PolygonImp::numberOfProperties() );
  return l;
}

const QCStringList PolygonImp::properties() const
{
  QCStringList l = Parent::properties();
  l += I18N_NOOP( "Perimeter" );
  l += I18N_NOOP( "Surface" );
  l += I18N_NOOP( "Center of Mass" );
  assert( l.size() == PolygonImp::numberOfProperties() );
  return l;
}

const ObjectImpType* PolygonImp::impRequirementForProperty( uint which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return PolygonImp::stype();
}

const char* PolygonImp::iconForProperty( uint which ) const
{
  assert( which < PolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "circumference"; // perimeter
  else if ( which == Parent::numberOfProperties() + 1 )
    return "areaCircle"; // surface
  else if ( which == Parent::numberOfProperties() + 2 )
    return "baseCircle"; // center of mass
  else assert( false );
  return "";
}

ObjectImp* PolygonImp::property( uint which, const KigDocument& w ) const
{
  assert( which < PolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
  {
    double circumference = 0.;
    // circumference
    for ( uint i = 0; i < mpoints.size(); ++i )
    {
      uint prev = ( i + mpoints.size() - 1 ) % mpoints.size();
      circumference += ( mpoints[i] - mpoints[prev] ).length();
    }
    return new DoubleImp( circumference );
  }
  else if ( which == Parent::numberOfProperties() + 1)
  {
    double surface2 = 0.0;
    Coordinate prevpoint = mpoints.back();
    for ( uint i = 0; i < mpoints.size(); ++i )
    {
      Coordinate point = mpoints[i];
      surface2 += ( point.x - prevpoint.x ) * ( point.y + prevpoint.y ); 
      prevpoint = point;
    }
    return new DoubleImp( fabs( surface2 / 2 ) );
  }
  else if ( which == Parent::numberOfProperties() + 2)
  {
    Coordinate sum = Coordinate( 0, 0);
    Coordinate prevpoint = mpoints.back();
    for ( uint i = 0; i < mpoints.size(); ++i )
    {
      Coordinate point = mpoints[i];
      sum += point;
      prevpoint = point;
    }
    return new PointImp( sum / mpoints.size() );
  }
  else assert( false );
  return new InvalidImp;
}

const std::vector<Coordinate> PolygonImp::points() const
{
  std::vector<Coordinate> np;
  np.reserve( mpoints.size() );
  std::copy( mpoints.begin(), mpoints.end(), std::back_inserter( np ) );
  return np;
}

const uint PolygonImp::npoints() const
{
  return mpoints.size();
}

PolygonImp* PolygonImp::copy() const
{
  return new PolygonImp( mpoints );
}

void PolygonImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool PolygonImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( PolygonImp::stype() ) &&
    static_cast<const PolygonImp&>( rhs ).points() == mpoints;
}

const ObjectImpType* PolygonImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "polygon",
    I18N_NOOP( "polygon" ),
    I18N_NOOP( "Select this polygon" ),
    I18N_NOOP( "Select polygon %1" ),
    I18N_NOOP( "Remove a Polygon" ),
    I18N_NOOP( "Add a Polygon" ),
    I18N_NOOP( "Move a Polygon" ),
    I18N_NOOP( "Attach to this polygon" ),
    I18N_NOOP( "Show a Polygon" ),
    I18N_NOOP( "Hide a Polygon" )
    );
  return &t;
}

const ObjectImpType* PolygonImp::type() const
{
  return PolygonImp::stype();
}

bool PolygonImp::isPropertyDefinedOnOrThroughThisImp( uint which ) const
{
  assert( which < PolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

Rect PolygonImp::surroundingRect() const
{
  Rect r( 0., 0., 0., 0. );
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    r.setContains( mpoints[i] );
  }
  return r;
}