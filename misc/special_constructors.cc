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

#include "special_constructors.h"

#include "calcpaths.h"
#include "common.h"
#include "conic-common.h"
#include "guiaction.h"
#include "kigpainter.h"

#include "../kig/kig_part.h"
#include "../modes/construct_mode.h"
#include "../objects/bogus_imp.h"
#include "../objects/circle_imp.h"
#include "../objects/conic_imp.h"
#include "../objects/conic_types.h"
#include "../objects/cubic_imp.h"
#include "../objects/intersection_types.h"
#include "../objects/line_imp.h"
#include "../objects/line_type.h"
#include "../objects/locus_imp.h"
#include "../objects/object_calcer.h"
#include "../objects/object_drawer.h"
#include "../objects/object_factory.h"
#include "../objects/object_holder.h"
#include "../objects/object_imp.h"
#include "../objects/object_type.h"
#include "../objects/other_imp.h"
#include "../objects/other_type.h"
#include "../objects/point_imp.h"
#include "../objects/polygon_imp.h"
#include "../objects/tangent_type.h"
#include "../objects/centerofcurvature_type.h"
#include "../objects/polygon_type.h"

#include <qpen.h>

#include <klocale.h>

#include <algorithm>
#include <functional>

class ConicConicIntersectionConstructor
  : public StandardConstructorBase
{
protected:
  ArgsParser mparser;
public:
  ConicConicIntersectionConstructor();
  ~ConicConicIntersectionConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );

  bool isTransform() const;
};

class ConicLineIntersectionConstructor
  : public MultiObjectTypeConstructor
{
public:
  ConicLineIntersectionConstructor();
  ~ConicLineIntersectionConstructor();
};

class ArcLineIntersectionConstructor
  : public MultiObjectTypeConstructor
{
public:
  ArcLineIntersectionConstructor();
  ~ArcLineIntersectionConstructor();
};

ConicRadicalConstructor::ConicRadicalConstructor()
  : StandardConstructorBase(
    I18N_NOOP( "Radical Lines for Conics" ),
    I18N_NOOP( "The lines constructed through the intersections "
               "of two conics.  This is also defined for "
               "non-intersecting conics." ),
    "conicsradicalline", mparser ),
    mtype( ConicRadicalType::instance() ),
    mparser( mtype->argsParser().without( IntImp::stype() ) )
{
}

ConicRadicalConstructor::~ConicRadicalConstructor()
{
}

void ConicRadicalConstructor::drawprelim(
  const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& doc ) const
{
  if ( parents.size() == 2 && parents[0]->imp()->inherits( ConicImp::stype() ) &&
       parents[1]->imp()->inherits( ConicImp::stype() ) )
  {
    Args args;
    std::transform( parents.begin(), parents.end(),
                    std::back_inserter( args ), std::mem_fun( &ObjectCalcer::imp ) );
    for ( int i = -1; i < 2; i += 2 )
    {
      IntImp root( i );
      IntImp zeroindex( 1 );
      args.push_back( &root );
      args.push_back( &zeroindex );
      ObjectImp* data = mtype->calc( args, doc );
      drawer.draw( *data, p, true );
      delete data; data = 0;
      args.pop_back();
      args.pop_back();
    };
  };
}

std::vector<ObjectHolder*> ConicRadicalConstructor::build( const std::vector<ObjectCalcer*>& os, KigDocument&, KigWidget& ) const
{
  using namespace std;
  std::vector<ObjectHolder*> ret;
  ObjectCalcer* zeroindexcalcer = new ObjectConstCalcer( new IntImp( 1 ) );
  for ( int i = -1; i < 2; i += 2 )
  {
    std::vector<ObjectCalcer*> args;
    std::copy( os.begin(), os.end(), back_inserter( args ) );
    args.push_back( new ObjectConstCalcer( new IntImp( i ) ) );
    // we use only one zeroindex dataobject, so that if you switch one
    // radical line around, then the other switches along..
    args.push_back( zeroindexcalcer );
    ret.push_back(
      new ObjectHolder( new ObjectTypeCalcer( mtype, args ) ) );
  };
  return ret;
}

static const struct ArgsParser::spec argsspecpp[] =
{
  { PointImp::stype(), I18N_NOOP( "Moving Point" ),
    I18N_NOOP( "Select the moving point, which will be moved around while drawing the locus..." ), false },
  { PointImp::stype(), I18N_NOOP( "Following Point" ),
    I18N_NOOP( "Select the following point, whose locations the locus will be drawn through..." ), true }
};

LocusConstructor::LocusConstructor()
  : StandardConstructorBase( I18N_NOOP( "Locus" ), I18N_NOOP( "A locus" ),
                             "locus", margsparser ),
    margsparser( argsspecpp, 2 )
{
}

LocusConstructor::~LocusConstructor()
{
}

void LocusConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                   const KigDocument& ) const
{
  // this function is rather ugly, but it is necessary to do it this
  // way in order to play nice with Kig's design..

  if ( parents.size() != 2 ) return;
  const ObjectTypeCalcer* constrained = dynamic_cast<ObjectTypeCalcer*>( parents.front() );
  const ObjectCalcer* moving = parents.back();
  if ( ! constrained || ! constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    // moving is in fact the constrained point.. swap them..
    moving = parents.front();
    constrained = dynamic_cast<const ObjectTypeCalcer*>( parents.back() );
    assert( constrained );
  };
  assert( constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) );

  const ObjectImp* oimp = constrained->parents().back()->imp();
  if( !oimp->inherits( CurveImp::stype() ) )
    oimp = constrained->parents().front()->imp();
  assert( oimp->inherits( CurveImp::stype() ) );
  const CurveImp* cimp = static_cast<const CurveImp*>( oimp );

  ObjectHierarchy hier( constrained, moving );

  LocusImp limp( cimp->copy(), hier );
  drawer.draw( limp, p, true );
}

const int LocusConstructor::wantArgs(
 const std::vector<ObjectCalcer*>& os, const KigDocument&, const KigWidget&
 ) const
{
  int ret = margsparser.check( os );
  if ( ret == ArgsParser::Invalid ) return ret;
  else if ( os.size() != 2 ) return ret;
  if ( dynamic_cast<ObjectTypeCalcer*>( os.front() ) &&
       static_cast<ObjectTypeCalcer*>( os.front() )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    std::set<ObjectCalcer*> children = getAllChildren( os.front() );
    return children.find( os.back() ) != children.end() ? ret : ArgsParser::Invalid;
  }
  if ( dynamic_cast<ObjectTypeCalcer*>( os.back() ) &&
       static_cast<ObjectTypeCalcer*>( os.back() )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    std::set<ObjectCalcer*> children = getAllChildren( os.back() );
    return children.find( os.front() ) != children.end() ? ret : ArgsParser::Invalid;
  }
  return ArgsParser::Invalid;
}

std::vector<ObjectHolder*> LocusConstructor::build( const std::vector<ObjectCalcer*>& parents, KigDocument&, KigWidget& ) const
{
  std::vector<ObjectHolder*> ret;
  assert( parents.size() == 2 );

  ObjectTypeCalcer* constrained = dynamic_cast<ObjectTypeCalcer*>( parents.front() );
  ObjectCalcer* moving = parents.back();
  if ( ! constrained || ! constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    // moving is in fact the constrained point.. swap them..
    moving = parents.front();
    constrained = dynamic_cast<ObjectTypeCalcer*>( parents.back() );
    assert( constrained );
  };
  assert( constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) );

  ret.push_back( ObjectFactory::instance()->locus( constrained, moving ) );
  return ret;
}

QString LocusConstructor::useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& os,
                                   const KigDocument&, const KigWidget& ) const
{
  if ( dynamic_cast<const ObjectTypeCalcer*>( &o ) &&
       static_cast<const ObjectTypeCalcer&>( o ).type()->inherits( ObjectType::ID_ConstrainedPointType ) &&
       ( os.empty() || !dynamic_cast<ObjectTypeCalcer*>( os[0] ) ||
         !static_cast<const ObjectTypeCalcer*>( os[0] )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
    ) return i18n( "Moving Point" );
  else return i18n( "Dependent Point" );
}

void ConicRadicalConstructor::plug( KigPart*, KigGUIAction* )
{
}

void LocusConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool ConicRadicalConstructor::isTransform() const
{
  return mtype->isTransform();
}

bool LocusConstructor::isTransform() const
{
  return false;
}

/*
 * construction of polygon vertices
 */

static const struct ArgsParser::spec argsspecpv[] =
{
  { PolygonImp::stype(), I18N_NOOP( "Polygon" ),
    I18N_NOOP( "Construct the vertices of this polygon..." ), true }
};

PolygonVertexTypeConstructor::PolygonVertexTypeConstructor()
  : StandardConstructorBase( I18N_NOOP( "Vertices of a Polygon" ), 
       I18N_NOOP( "The vertices of a polygon." ),
       "polygonvertices", margsparser ),
    mtype( PolygonVertexType::instance() ),
    margsparser( argsspecpv, 1 )
{
}

PolygonVertexTypeConstructor::~PolygonVertexTypeConstructor()
{
}

void PolygonVertexTypeConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                   const KigDocument& ) const
{
  if ( parents.size() != 1 ) return;

  const PolygonImp* polygon = dynamic_cast<const PolygonImp*>( parents.front()->imp() );
  const std::vector<Coordinate> points = polygon->points();

  int sides = points.size();
  for ( int i = 0; i < sides; ++i )
  {
    PointImp point = PointImp( points[i] );
    drawer.draw( point, p, true );
  }
}

std::vector<ObjectHolder*> PolygonVertexTypeConstructor::build( const std::vector<ObjectCalcer*>& parents, KigDocument&, KigWidget& ) const
{
  std::vector<ObjectHolder*> ret;
  assert( parents.size() == 1 );
  const PolygonImp* polygon = dynamic_cast<const PolygonImp*>( parents.front()->imp() );
  const std::vector<Coordinate> points = polygon->points();

  int sides = points.size();

  for ( int i = 0; i < sides; ++i )
  {
    ObjectConstCalcer* d = new ObjectConstCalcer( new IntImp( i ) );
    std::vector<ObjectCalcer*> args( parents );
    args.push_back( d );
    ret.push_back( new ObjectHolder( new ObjectTypeCalcer( mtype, args ) ) );
  }
  return ret;
}

void PolygonVertexTypeConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool PolygonVertexTypeConstructor::isTransform() const
{
  return false;
}

/*
 * poligon by center and vertex
 */

//static const ArgsParser::spec argsspectc[] = {
//  { PointImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true },
//  { PointImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true }
//};

PoligonBCVConstructor::PoligonBCVConstructor(
  const char* descname, const char* desc, const char* iconfile,
  int sides )
  : StandardConstructorBase( descname, desc,
                             iconfile, mparser ),
    mtype( PoligonBCVType::instance() ), 
    mparser( mtype->argsParser().without( IntImp::stype() ) ),
    msides( sides )
{
}

PoligonBCVConstructor::~PoligonBCVConstructor()
{
}

void PoligonBCVConstructor::drawprelim( const ObjectDrawer& drawer, 
                      KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                      const KigDocument& doc) const
{
  if ( parents.size() != 2 ) return;
  assert ( parents[0]->imp()->inherits( PointImp::stype() ) &&
           parents[1]->imp()->inherits( PointImp::stype() ) );

  Args args;
  std::transform( parents.begin(), parents.end(),
               std::back_inserter( args ), std::mem_fun( &ObjectCalcer::imp ) );

  args.push_back( new IntImp( msides ) );
  ObjectImp* data = mtype->calc( args, doc );
  drawer.draw( *data, p, true );
  delete data;
  data = 0;
}

std::vector<ObjectHolder*> PoligonBCVConstructor::build(
  const std::vector<ObjectCalcer*>& os, KigDocument&, KigWidget& ) const
{
  assert( os.size() == 2 );
  std::vector<ObjectHolder*> ret;
  ObjectConstCalcer* sidesint = new ObjectConstCalcer( new IntImp( msides ) );

  std::vector<ObjectCalcer*> args = os;
  args.push_back( sidesint );
  ret.push_back(
    new ObjectHolder(
      new ObjectTypeCalcer(
        PoligonBCVType::instance(), args ) ) );

  return ret;
}

QString PoligonBCVConstructor::useText( const ObjectCalcer&, const std::vector<ObjectCalcer*>&,
                                          const KigDocument&, const KigWidget& ) const
{
  if ( msides == 5 )
    return i18n( "Construct the pentagon with this center" );

  return i18n( "Construct the polygon with this center" );
}

void PoligonBCVConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool PoligonBCVConstructor::isTransform() const
{
  return false;
}

/*
 * ConicConic intersection...
 */

static const ArgsParser::spec argsspectc[] = {
  { ConicImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true },
  { ConicImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true }
};

ConicConicIntersectionConstructor::ConicConicIntersectionConstructor()
  : StandardConstructorBase( "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
                             "curvelineintersection", mparser ),
    mparser( argsspectc, 2 )
{
}

ConicConicIntersectionConstructor::~ConicConicIntersectionConstructor()
{
}

void ConicConicIntersectionConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                                    const KigDocument& ) const
{
  if ( parents.size() != 2 ) return;
  assert ( parents[0]->imp()->inherits( ConicImp::stype() ) &&
           parents[1]->imp()->inherits( ConicImp::stype() ) );
  const ConicCartesianData conica =
    static_cast<const ConicImp*>( parents[0]->imp() )->cartesianData();
  const ConicCartesianData conicb =
    static_cast<const ConicImp*>( parents[1]->imp() )->cartesianData();
  bool ok = true;
  for ( int wr = -1; wr < 2; wr += 2 )
  {
    LineData radical = calcConicRadical( conica, conicb, wr, 1, ok );
    if ( ok )
    {
      for ( int wi = -1; wi < 2; wi += 2 )
      {
        Coordinate c = calcConicLineIntersect( conica, radical, 0.0, wi );
        if ( c.valid() ) {
          PointImp pi( c );
          drawer.draw( pi, p, true );
        }
      };
    };
  };
}

std::vector<ObjectHolder*> ConicConicIntersectionConstructor::build(
  const std::vector<ObjectCalcer*>& os, KigDocument& doc, KigWidget& ) const
{
  assert( os.size() == 2 );
  std::vector<ObjectHolder*> ret;
  ObjectCalcer* conica = os[0];
  ObjectConstCalcer* zeroindexdo = new ObjectConstCalcer( new IntImp( 1 ) );

  for ( int wr = -1; wr < 2; wr += 2 )
  {
    std::vector<ObjectCalcer*> args = os;
    args.push_back( new ObjectConstCalcer( new IntImp( wr ) ) );
    args.push_back( zeroindexdo );
    ObjectTypeCalcer* radical =
      new ObjectTypeCalcer( ConicRadicalType::instance(), args );
    radical->calc( doc );
    for ( int wi = -1; wi < 2; wi += 2 )
    {
      args.clear();
      args.push_back( conica );
      args.push_back( radical );
      args.push_back( new ObjectConstCalcer( new IntImp( wi ) ) );
      ret.push_back(
        new ObjectHolder(
          new ObjectTypeCalcer(
            ConicLineIntersectionType::instance(), args ) ) );
    };
  };
  return ret;
}

void ConicConicIntersectionConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool ConicConicIntersectionConstructor::isTransform() const
{
  return false;
}

ConicLineIntersectionConstructor::ConicLineIntersectionConstructor()
  : MultiObjectTypeConstructor(
    ConicLineIntersectionType::instance(),
    "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
    "curvelineintersection", -1, 1 )
{
}

ConicLineIntersectionConstructor::~ConicLineIntersectionConstructor()
{
}

ArcLineIntersectionConstructor::ArcLineIntersectionConstructor()
  : MultiObjectTypeConstructor(
    ArcLineIntersectionType::instance(),
    "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
    "curvelineintersection", -1, 1 )
{
}

ArcLineIntersectionConstructor::~ArcLineIntersectionConstructor()
{
}

QString ConicRadicalConstructor::useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>&,
                                          const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( CircleImp::stype() ) )
    return i18n( "Construct the Radical Lines of This Circle" );
  else
    return i18n( "Construct the Radical Lines of This Conic" );
}

GenericIntersectionConstructor::GenericIntersectionConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Intersect" ),
    I18N_NOOP( "The intersection of two objects" ),
    "curvelineintersection" )
{
  // intersection type..
  // There is one "toplevel" object_constructor, that is composed
  // of multiple subconstructors..  First we build the
  // subconstructors:
  SimpleObjectTypeConstructor* lineline =
    new SimpleObjectTypeConstructor(
      LineLineIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "curvelineintersection" );

  ObjectConstructor* lineconic =
    new ConicLineIntersectionConstructor();

  ObjectConstructor* arcline =
    new ArcLineIntersectionConstructor();

  MultiObjectTypeConstructor* linecubic =
    new MultiObjectTypeConstructor(
      LineCubicIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "curvelineintersection", 1, 2, 3 );

  ObjectConstructor* conicconic =
    new ConicConicIntersectionConstructor();

  MultiObjectTypeConstructor* circlecircle =
    new MultiObjectTypeConstructor(
      CircleCircleIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "circlecircleintersection", -1, 1 );

  merge( lineline );
  merge( circlecircle );
  merge( lineconic );
  merge( linecubic );
  merge( conicconic );
  merge( arcline );
}

GenericIntersectionConstructor::~GenericIntersectionConstructor()
{
}

bool GenericIntersectionConstructor::isIntersection() const
{
  return true;
}

QString GenericIntersectionConstructor::useText(
  const ObjectCalcer& o, const std::vector<ObjectCalcer*>&,
  const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( CircleImp::stype() ) )
    return i18n( "Intersect with This Circle" );
  else if ( o.imp()->inherits( ConicImp::stype() ) )
    return i18n( "Intersect with This Conic" );
  else if ( o.imp()->inherits( AbstractLineImp::stype() ) )
    return i18n( "Intersect with This Line" );
  else if ( o.imp()->inherits( CubicImp::stype() ) )
    return i18n( "Intersect with This Cubic Curve" );
  else if ( o.imp()->inherits( ArcImp::stype() ) )
    return i18n( "Intersect with This Arc" );
  else assert( false );
  return QString::null;
}

static const ArgsParser::spec argsspecMidPointOfTwoPoints[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct Midpoint of This Point and Another One" ),
    I18N_NOOP( "Select the first of the points of which you want to construct the midpoint..." ), false },
  { PointImp::stype(), I18N_NOOP( "Construct the midpoint of this point and another one" ),
    I18N_NOOP( "Select the other of the points of which to construct the midpoint..." ), false }
};

MidPointOfTwoPointsConstructor::MidPointOfTwoPointsConstructor()
  : StandardConstructorBase( "Mid Point",
                             "Construct the midpoint of two points",
                             "bisection", mparser ),
    mparser( argsspecMidPointOfTwoPoints, 2 )
{
}

MidPointOfTwoPointsConstructor::~MidPointOfTwoPointsConstructor()
{
}

void MidPointOfTwoPointsConstructor::drawprelim(
  const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
  const KigDocument& ) const
{
  if ( parents.size() != 2 ) return;
  assert( parents[0]->imp()->inherits( PointImp::stype() ) );
  assert( parents[1]->imp()->inherits( PointImp::stype() ) );
  const Coordinate m =
    ( static_cast<const PointImp*>( parents[0]->imp() )->coordinate() +
      static_cast<const PointImp*>( parents[1]->imp() )->coordinate() ) / 2;
  drawer.draw( PointImp( m ), p, true );
}

std::vector<ObjectHolder*> MidPointOfTwoPointsConstructor::build(
  const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& ) const
{
  ObjectTypeCalcer* seg = new ObjectTypeCalcer( SegmentABType::instance(), os );
  seg->calc( d );
  int index = seg->imp()->propertiesInternalNames().findIndex( "mid-point" );
  assert( index != -1 );
  ObjectPropertyCalcer* prop = new ObjectPropertyCalcer( seg, index );
  prop->calc( d );
  std::vector<ObjectHolder*> ret;
  ret.push_back( new ObjectHolder( prop ) );
  return ret;
}

void MidPointOfTwoPointsConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool MidPointOfTwoPointsConstructor::isTransform() const
{
  return false;
}

TestConstructor::TestConstructor( const ArgsParserObjectType* type, const char* descname,
                                  const char* desc, const char* iconfile )
  : StandardConstructorBase( descname, desc, iconfile, type->argsParser() ),
    mtype( type )
{
}

TestConstructor::~TestConstructor()
{
}

void TestConstructor::drawprelim( const ObjectDrawer&, KigPainter&, const std::vector<ObjectCalcer*>&,
                                  const KigDocument& ) const
{
  // not used, only here because of the wrong
  // ObjectConstructor-GUIAction design.  See the TODO
}

std::vector<ObjectHolder*> TestConstructor::build( const std::vector<ObjectCalcer*>&, KigDocument&,
                                                   KigWidget& ) const
{
  // not used, only here because of the wrong
  // ObjectConstructor-GUIAction design.  See the TODO
  std::vector<ObjectHolder*> ret;
  return ret;
}

void TestConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool TestConstructor::isTransform() const
{
  return false;
}

bool TestConstructor::isTest() const
{
  return true;
}

BaseConstructMode* TestConstructor::constructMode( KigPart& doc )
{
  return new TestConstructMode( doc, mtype );
}

const int TestConstructor::wantArgs( const std::vector<ObjectCalcer*>& os,
                                     const KigDocument& d, const KigWidget& v ) const
{
  int ret = StandardConstructorBase::wantArgs( os, d, v );
  if ( ret == ArgsParser::Complete ) ret = ArgsParser::Valid;
  return ret;
}

QString GenericIntersectionConstructor::selectStatement(
  const std::vector<ObjectCalcer*>& sel, const KigDocument&,
  const KigWidget& ) const
{
  if ( sel.size() == 0 )
    return i18n( "Select the first object to intersect..." );
  else
    return i18n( "Select the second object to intersect..." );
}

TangentConstructor::TangentConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Tangent" ),
    I18N_NOOP( "The line tangent to a curve" ),
    "tangent" )
{
  SimpleObjectTypeConstructor* conic =
    new SimpleObjectTypeConstructor(
      TangentConicType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "tangentconic" );

  SimpleObjectTypeConstructor* arc =
    new SimpleObjectTypeConstructor(
      TangentArcType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "tangentarc" );

  SimpleObjectTypeConstructor* cubic =
    new SimpleObjectTypeConstructor(
      TangentCubicType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "tangentcubic" );

  SimpleObjectTypeConstructor* curve =
    new SimpleObjectTypeConstructor(
      TangentCurveType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "tangentcurve" );

  merge( conic );
  merge( arc );
  merge( cubic );
  merge( curve );
}

TangentConstructor::~TangentConstructor()
{
}

QString TangentConstructor::useText(
  const ObjectCalcer& o, const std::vector<ObjectCalcer*>&,
  const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( CircleImp::stype() ) )
    return i18n( "Tangent to This Circle" );
  else if ( o.imp()->inherits( ConicImp::stype() ) )
    return i18n( "Tangent to This Conic" );
  else if ( o.imp()->inherits( ArcImp::stype() ) )
    return i18n( "Tangent to This Arc" );
  else if ( o.imp()->inherits( CubicImp::stype() ) )
    return i18n( "Tangent to This Cubic Curve" );
//  else assert( false );
  return QString::null;
}

QString TangentConstructor::selectStatement(
  const std::vector<ObjectCalcer*>& sel, const KigDocument&,
  const KigWidget& ) const
{
  if ( sel.size() == 0 )
    return i18n( "Select the object..." );
  else
    return i18n( "Select the point for the tangent to go through..." );
}

/*
 * center of curvature of a curve
 */

CocConstructor::CocConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Center Of Curvature" ),
    I18N_NOOP( "The center of the osculating circle to a curve" ),
    "centerofcurvature" )
{
  SimpleObjectTypeConstructor* conic =
    new SimpleObjectTypeConstructor(
      CocConicType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "cocconic" );

  SimpleObjectTypeConstructor* cubic =
    new SimpleObjectTypeConstructor(
      CocCubicType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "coccubic" );

  SimpleObjectTypeConstructor* curve =
    new SimpleObjectTypeConstructor(
      CocCurveType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "coccurve" );

  merge( conic );
  merge( cubic );
  merge( curve );
}

CocConstructor::~CocConstructor()
{
}

QString CocConstructor::useText(
  const ObjectCalcer& o, const std::vector<ObjectCalcer*>&,
  const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( ConicImp::stype() ) )
    return i18n( "Center of Curvature of This Conic" );
  else if ( o.imp()->inherits( CubicImp::stype() ) )
    return i18n( "Center of Curvature of This Cubic Curve" );
//  else assert( false );
  return QString::null;
}

QString CocConstructor::selectStatement(
  const std::vector<ObjectCalcer*>& sel, const KigDocument&,
  const KigWidget& ) const
{
  if ( sel.size() == 0 )
    return i18n( "Select the object..." );
  else
    return i18n( "Select the point where to compute the center of curvature..." );
}
