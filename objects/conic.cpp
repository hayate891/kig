/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#include "conic.h"

#include "point.h"

#include "../misc/common.h"
#include "../misc/kigpainter.h"
#include "../misc/coordinate_system.h"
#include "../misc/i18n.h"
#include "../kig/kig_view.h"
#include "../kig/kig_part.h"

#include <kdebug.h>
#include <qpen.h>
#include <math.h>

Conic::Conic()
{
};

Conic::~Conic()
{
};

bool Conic::contains (const Coordinate& o, const ScreenInfo& si ) const
{
  Coordinate pos;
  double costheta, sintheta, len, ecosthetamtheta0, rho;

  const ConicPolarEquationData d = polarEquationData();

  Coordinate focus1 = d.focus1;
  double ecostheta0 = d.ecostheta0;
  double esintheta0 = d.esintheta0;
  double pdimen = d.pdimen;

  pos = Coordinate(o) - focus1;
  len = pos.length();
  costheta = pos.x / len;
  sintheta = pos.y / len;

  ecosthetamtheta0 = costheta*ecostheta0 + sintheta*esintheta0;
  rho = pdimen / (1.0 - ecosthetamtheta0);

  if ( fabs(len - rho) <= si.normalMiss() ) return true;
  rho = - pdimen / ( 1.0 + ecosthetamtheta0 );
  return fabs( len - rho ) <= si.normalMiss();
};

void Conic::draw (KigPainter& p, bool ss) const
{
  p.setPen(QPen ( selected && ss ? Qt::red : mColor, 1));
  p.setBrush( Qt::NoBrush );

  p.drawConic( polarEquationData() );
};

bool Conic::inRect ( const Rect& ) const
{
  // not implemented yet, i'm thinking: take the diagonals of the
  // rect, their intersections with the circle, and check their
  // positions...
  return false;
};

Coordinate Conic::getPoint (double p) const
{
  double rho, costheta, sintheta;

  const ConicPolarEquationData d = polarEquationData();

  costheta = cos(p * 2 * M_PI);
  sintheta = sin(p * 2 * M_PI);
  rho = d.pdimen / (1 - costheta* d.ecostheta0 - sintheta* d.esintheta0);
  return d.focus1 + Coordinate (costheta, sintheta) * rho;
};

double Conic::getParam (const Coordinate& p) const
{
  const ConicPolarEquationData d = polarEquationData();
  Coordinate tmp = p - d.focus1;
  double l = tmp.length();
  double theta = atan2(tmp.y, tmp.x);
  double costheta = cos(theta);
  double sintheta = sin(theta);
  double rho1 = d.pdimen / (1 - costheta * d.ecostheta0 - sintheta * d.esintheta0);
  double rho2 = - d.pdimen / (1 + costheta * d.ecostheta0 + sintheta * d.esintheta0);
  if (fabs(rho1 - l) < fabs(rho2 - l))
    return fmod(theta / ( 2 * M_PI ) + 1, 1);
  else
    return fmod(theta / ( 2 * M_PI ) + 0.5, 1);
};

EllipseBFFP::EllipseBFFP( const Objects& os )
  : ConicBFFP( os )
{
};

void EllipseBFFP::calc()
{
  calcCommon( 1 );
};

Objects EllipseBFFP::getParents() const
{
  Objects objs;
  objs.push_back( focus1 );
  objs.push_back( focus2 );
  objs.push_back( poc );
  return objs;
}

EllipseBFFP::EllipseBFFP(const EllipseBFFP& c)
  : ConicBFFP( c )
{
}

const QCString Conic::sBaseTypeName()
{
  return I18N_NOOP("conic");
}

const QString EllipseBFFP::sDescriptiveName()
{
  return i18n("Ellipse by focuses and point");
}

const QString EllipseBFFP::sDescription()
{
  return i18n(
    "An ellipse constructed by its focuses and a point on its border"
    );
}

const char* EllipseBFFP::sActionName()
{
  return "objects_new_ellipsebffp";
}

Conic::Conic( const Conic& c )
  : Curve( c )
{
}

Object::WantArgsResult ConicBFFP::sWantArgs( const Objects& os )
{
  uint size = os.size();
  assert( size > 0 );
  if ( size > 3 ) return NotGood;
  for ( uint i = 0; i < size; ++i )
    if ( ! os[i]->toPoint() )
      return NotGood;
  return size == 3 ? Complete : NotComplete;
}

QString EllipseBFFP::sUseText( const Objects& os, const Object* )
{
  switch ( os.size() )
  {
  case 0:
    return i18n( "Ellipse with this focus" );
  case 1:
    return i18n( "Ellipse with this focus" );
  case 2:
    return i18n( "Ellipse through this point" );
  default:
    assert( false );
  };
}

void ConicBFFP::sDrawPrelimCommon( KigPainter& p, const Objects& args, int type)
{
  if ( args.size() < 2 ) return;

  assert( type == 1 || type == -1 );
  assert( args.size() < 4 );

  std::vector<Coordinate> cargs;
  for ( Objects::const_iterator i = args.begin(); i != args.end(); ++i )
  {
    assert( (*i)->toPoint() );
    cargs.push_back( (*i)->toPoint()->getCoord() );
  };

  ConicPolarEquationData data = calcConicBFFP( cargs, type );

  p.setPen(QPen (Qt::red, 1));
  p.drawConic( data );
}

Conic* Conic::toConic()
{
  return this;
}

const Conic* Conic::toConic() const
{
  return this;
}

void ConicB5P::calc()
{
  std::vector<Coordinate> points;

  mvalid = true;
  for ( Point** ipt = pts; ipt < pts + 5; ++ipt )
    mvalid &= (*ipt)->valid();
  if ( mvalid )
  {
    std::transform( pts, pts + 5, std::back_inserter( points ),
                    std::mem_fun( &Point::getCoord ) );
    cequation = calcConicThroughPoints( points, zerotilt, parabolaifzt, ysymmetry );
    pequation = ConicPolarEquationData( cequation );
  }
}

const char* ConicB5P::sActionName()
{
  return "objects_new_conicb5p";
}

Objects ConicB5P::getParents() const
{
  Objects objs ( pts, pts+5 );
  return objs;
}

ConicB5P::ConicB5P(const ConicB5P& c)
  : Conic( c ), pequation( c.pequation ), cequation( c.cequation )
{
  for ( int i = 0; i != 5; ++i )
  {
    pts[i]=c.pts[i];
    pts[i]->addChild(this);
  }
}

const QString ConicB5P::sDescriptiveName()
{
  return i18n("Conic by five points");
}

const QString ConicB5P::sDescription()
{
  return i18n( "A conic constructed through five points" );
}

Object::WantArgsResult ConicB5P::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 5 || size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  return size == 5 ? Complete : NotComplete;
}

QString ConicB5P::sUseText( const Objects&, const Object* )
{
  return i18n("Through point");
}

void ConicB5P::sDrawPrelim( KigPainter& p, const Objects& os )
{
  std::vector<Coordinate> points;

  uint size = os.size();
  assert( size > 0 && size < 6 );
  if ( size < 2 ) return;  // don't drawprelim if too few points
  for ( uint i = 0; i < size; ++i )
  {
    assert( os[i]->toPoint() );
    points.push_back( os[i]->toPoint()->getCoord() );
  };

  p.setPen(QPen (Qt::red, 1));
  p.drawConic(
    ConicPolarEquationData(
      calcConicThroughPoints( points, zerotilt, parabolaifzt, ysymmetry )
      )
    );
  return;
}

ConicB5P::ConicB5P( const Objects& os )
  : Conic(), pequation(), cequation()
{
  assert( os.size() == 5 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->toPoint() );
    (*i)->addChild( this );
    pts[i-os.begin()] = static_cast<Point*>( *i );
  };
}

void ParabolaBTP::calc()
{
  std::vector<Coordinate> points;

  mvalid = true;
  for ( Point** ipt = pts; ipt < pts + 3; ++ipt )
    mvalid &= (*ipt)->valid();
  if ( mvalid )
  {
    std::transform( pts, pts + 3, std::back_inserter( points ),
                    std::mem_fun( &Point::getCoord ) );
    cequation = calcConicThroughPoints( points, zerotilt, parabolaifzt, ysymmetry );
    pequation = ConicPolarEquationData( cequation );
  }
}

const char* ParabolaBTP::sActionName()
{
  return "objects_new_parabolabtp";
}

Objects ParabolaBTP::getParents() const
{
  Objects objs ( pts, pts+3 );
  return objs;
}

ParabolaBTP::ParabolaBTP(const ParabolaBTP& c)
  : Conic( c ), cequation( c.cequation ), pequation( c.pequation )
{
  for ( int i = 0; i != 3; ++i )
  {
    pts[i]=c.pts[i];
    pts[i]->addChild(this);
  }
}

const QString ParabolaBTP::sDescriptiveName()
{
  return i18n( "Vertical parabola by three points" );
}

const QString ParabolaBTP::sDescription()
{
  return i18n( "A vertical parabola constructed through three points" );
}

Object::WantArgsResult ParabolaBTP::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 3 || size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  return size == 3 ? Complete : NotComplete;
}

QString ParabolaBTP::sUseText( const Objects&, const Object* )
{
  return i18n( "Through point" );
}

void ParabolaBTP::sDrawPrelim( KigPainter& p, const Objects& os )
{
  std::vector<Coordinate> points;

  uint size = os.size();
  assert( size > 0 && size < 4 );
  if ( size < 2 ) return;  // don't drawprelim if too few points
  for ( uint i = 0; i < size; ++i )
  {
    assert( os[i]->toPoint() );
    points.push_back( os[i]->toPoint()->getCoord() );
  };

  p.setPen(QPen (Qt::red, 1));
  p.drawConic(
    ConicPolarEquationData(
      calcConicThroughPoints(
        points, zerotilt, parabolaifzt, ysymmetry
        )
      )
    );
  return;
}

ParabolaBTP::ParabolaBTP( const Objects& os )
  : Conic(), cequation(), pequation()
{
  assert( os.size() == 3 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->toPoint() );
    (*i)->addChild( this );
    pts[i-os.begin()] = static_cast<Point*>( *i );
  };
}

EllipseBFFP::~EllipseBFFP()
{
}

EllipseBFFP* EllipseBFFP::copy()
{
  return new EllipseBFFP(*this);
}

const QCString EllipseBFFP::sFullTypeName()
{
  return "EllipseBFFP";
}

HyperbolaBFFP::~HyperbolaBFFP()
{
}

HyperbolaBFFP* HyperbolaBFFP::copy()
{
  return new HyperbolaBFFP(*this);
}

ConicBFFP::~ConicBFFP()
{
}

ConicBFFP::ConicBFFP( const Objects& os )
  : Conic(), mequation()
{
  assert( os.size() == 3 );
  focus1 = os[0]->toPoint();
  focus2 = os[1]->toPoint();
  poc = os[2]->toPoint();
  assert( poc && focus1 && focus2 );
  poc->addChild( this );
  focus1->addChild( this );
  focus2->addChild( this );
}

void ConicBFFP::calcCommon( int type )
{
  assert( type == 1 || type == -1 );
  mvalid = poc->valid() && focus1->valid() && focus2->valid();

  if ( mvalid )
  {
    std::vector<Coordinate> args;
    args.push_back( focus1->getCoord() );
    args.push_back( focus2->getCoord() );
    args.push_back( poc->getCoord() );
    mequation = calcConicBFFP( args, type );
  };
};

Objects ConicBFFP::getParents() const
{
  Objects objs;
  objs.push_back( focus1 );
  objs.push_back( focus2 );
  objs.push_back( poc );
  return objs;
}

ConicBFFP::ConicBFFP( const ConicBFFP& c )
  : Conic( c ), poc( c.poc ), focus1( c.focus1 ),
    focus2 ( c.focus2 ), mequation( c.mequation )
{
  poc->addChild(this);
  focus1->addChild(this);
  focus2->addChild(this);
}

void EllipseBFFP::sDrawPrelim( KigPainter& p, const Objects& args )
{
  sDrawPrelimCommon( p, args, 1 );
}

HyperbolaBFFP::HyperbolaBFFP( const Objects& os )
  : ConicBFFP( os )
{
}

HyperbolaBFFP::HyperbolaBFFP( const HyperbolaBFFP& c )
  : ConicBFFP( c )
{
}

const QString HyperbolaBFFP::sDescriptiveName()
{
  return i18n("Hyperbola by focuses and point");
}

const QString HyperbolaBFFP::sDescription()
{
  return i18n(
    "A hyperbola constructed by its focuses and a point on its border"
    );
}

const char* HyperbolaBFFP::sActionName()
{
  return "objects_new_hyperbolabffp";
}

QString HyperbolaBFFP::sUseText( const Objects& os, const Object* )
{
  switch ( os.size() )
  {
  case 0:
    return i18n( "Hyperbola with this focus" );
  case 1:
    return i18n( "Hyperbola with this focus" );
  case 2:
    return i18n( "Hyperbola through this point" );
  default:
    assert( false );
  };
}

void HyperbolaBFFP::sDrawPrelim( KigPainter& p, const Objects& args )
{
  sDrawPrelimCommon( p, args, -1 );
}

void HyperbolaBFFP::calc()
{
  calcCommon( -1 );
}

int Conic::conicType() const
{
  const ConicPolarEquationData d = polarEquationData();
  double ec = d.ecostheta0;
  double es = d.esintheta0;
  double esquare = ec*ec + es*es;
  const double parabolamiss = 1e-3;  // don't know what a good value could be

  if (esquare < 1.0 - parabolamiss) return 1;
  if (esquare > 1.0 + parabolamiss) return -1;

  return 0;
}

QString Conic::type() const
{
  switch (conicType())
  {
  case 1:
    return I18N_NOOP("ellipse");
  case -1:
    return I18N_NOOP("hyperbola");
  case 0:
    return I18N_NOOP("parabola");
  default:
    assert( false );
  }
}

const ConicCartesianEquationData Conic::cartesianEquationData() const
{
  return ConicCartesianEquationData( polarEquationData() );
}

Coordinate Conic::focus1() const
{
  return polarEquationData().focus1;
}

Coordinate Conic::focus2() const
{
  const ConicPolarEquationData d = polarEquationData();
  double ec = d.ecostheta0;
  double es = d.esintheta0;

  double fact = 2*d.pdimen/(1 - ec*ec - es*es);

  return d.focus1 + fact*Coordinate(ec, es);
}

const uint Conic::numberOfProperties() const
{
  return Curve::numberOfProperties() + 5;
}

const Property Conic::property( uint which, const KigWidget& w ) const
{
  int pnum = 0;

  if ( which < Curve::numberOfProperties() ) return Curve::property( which, w );
  if ( which == Curve::numberOfProperties() + pnum++ )
    return Property( type() );
  else if ( which == Curve::numberOfProperties() + pnum++ )
    return Property( focus1() );
  else if ( which == Curve::numberOfProperties() + pnum++ )
    return Property( focus2() );
  else if ( which == Curve::numberOfProperties() + pnum++ )
    return Property( cartesianEquationString( w ) );
  else if ( which == Curve::numberOfProperties() + pnum++ )
    return Property( polarEquationString( w ) );
  else assert( false );
}

const QCStringList Conic::properties() const
{
  QCStringList l = Curve::properties();
  l << I18N_NOOP( "Type" );
  l << I18N_NOOP( "First focus" );
  l << I18N_NOOP( "Second focus" );
  l << I18N_NOOP( "Cartesian equation" );
  l << I18N_NOOP( "Polar equation" );
  assert( l.size() == Conic::numberOfProperties() );
  return l;
}

QString Conic::cartesianEquationString( const KigWidget& ) const
{
  QString ret = i18n( "%1 x^2 + %2 y^2 + %3 xy + %4 x + %5 y + %6 = 0" );
  ConicCartesianEquationData data = cartesianEquationData();
  ret = ret.arg( data.coeffs[0], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[1], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[2], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[3], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[4], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[5], 0, 'g', 3 );
  return ret;
}

const ConicPolarEquationData ConicBFFP::polarEquationData() const
{
  return mequation;
}

const ConicPolarEquationData ConicB5P::polarEquationData() const
{
  return pequation;
}

const ConicCartesianEquationData ConicB5P::cartesianEquationData() const
{
  return cequation;
}

const ConicCartesianEquationData ParabolaBTP::cartesianEquationData() const
{
  return cequation;
}

const ConicPolarEquationData ParabolaBTP::polarEquationData() const
{
  return pequation;
}

ConicCartesianEquationData::ConicCartesianEquationData()
{
  std::fill( coeffs, coeffs + 6, 0 );
};

QString Conic::polarEquationString( const KigWidget& w ) const
{
  QString ret = i18n( "rho = %1/(1 + %2 cos theta + %3 sin theta)\n    [centered at %4]" );
  const ConicPolarEquationData data = polarEquationData();

  ret = ret.arg( data.pdimen, 0, 'g', 3 );
  ret = ret.arg( -data.ecostheta0, 0, 'g', 3 );
  ret = ret.arg( -data.esintheta0, 0, 'g', 3 );

  ret = ret.arg( w.document().coordinateSystem().fromScreen( data.focus1, w ) );
  return ret;
}


/*
 * equilateral hyperbola by 4 points
 */

void EquilateralHyperbolaB4P::calc()
{
  std::vector<Coordinate> points;

  mvalid = true;
  for ( Point** ipt = pts; ipt < pts + 4; ++ipt )
    mvalid &= (*ipt)->valid();
  if ( mvalid )
  {
    std::transform( pts, pts + 4, std::back_inserter( points ),
                    std::mem_fun( &Point::getCoord ) );
    cequation = calcConicThroughPoints( points, equilateral );
    pequation = ConicPolarEquationData( cequation );
  }
}

const char* EquilateralHyperbolaB4P::sActionName()
{
  return "objects_new_equilateralhyperbolab4p";
}

Objects EquilateralHyperbolaB4P::getParents() const
{
  Objects objs ( pts, pts+4 );
  return objs;
}

EquilateralHyperbolaB4P::EquilateralHyperbolaB4P(const EquilateralHyperbolaB4P& c)
  : Conic( c ), cequation( c.cequation ), pequation( c.pequation )
{
  for ( int i = 0; i != 4; ++i )
  {
    pts[i]=c.pts[i];
    pts[i]->addChild(this);
  }
}

const QString EquilateralHyperbolaB4P::sDescriptiveName()
{
  return i18n( "Equilateral hyperbola by four points" );
}

const QString EquilateralHyperbolaB4P::sDescription()
{
  return i18n( "An equilateral hyperbola constructed through four points" );
}

Object::WantArgsResult EquilateralHyperbolaB4P::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 4 || size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  return size == 4 ? Complete : NotComplete;
}

QString EquilateralHyperbolaB4P::sUseText( const Objects&, const Object* )
{
  return i18n( "Through point" );
}

void EquilateralHyperbolaB4P::sDrawPrelim( KigPainter& p, const Objects& os )
{
  std::vector<Coordinate> points;

  uint size = os.size();
  assert( size > 0 && size < 5 );
  if ( size < 2 ) return;  // don't drawprelim if too few points
  for ( uint i = 0; i < size; ++i )
  {
    assert( os[i]->toPoint() );
    points.push_back( os[i]->toPoint()->getCoord() );
  };

  p.setPen(QPen (Qt::red, 1));
  p.drawConic(
    ConicPolarEquationData(
      calcConicThroughPoints(
        points, equilateral,
        zerotilt, ysymmetry
        )
      )
    );
}

EquilateralHyperbolaB4P::EquilateralHyperbolaB4P( const Objects& os )
  : Conic()
{
  assert( os.size() == 4 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->toPoint() );
    (*i)->addChild( this );
    pts[i-os.begin()] = static_cast<Point*>( *i );
  };
}

const ConicCartesianEquationData EquilateralHyperbolaB4P::cartesianEquationData() const
{
  return cequation;
}

const ConicPolarEquationData EquilateralHyperbolaB4P::polarEquationData() const
{
  return pequation;
}
