/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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

#include "kigpainter.h"

#include "../kig/kig_view.h"
#include "../objects/object.h"
#include "../objects/conic.h"
#include "common.h"
#include "coordinate_system.h"

#include <qpen.h>

#include <cmath>
#include <stack>

KigPainter::KigPainter( const ScreenInfo& si, QPaintDevice* device, bool no )
  : mP ( device ),
    msi( si ),
    mNeedOverlay( no )
{
}

KigPainter::~KigPainter()
{
}

void KigPainter::drawRect( const Rect& r )
{
  Rect rt = r.normalized();
  QRect qr = toScreen(rt);
  qr.normalize();
  mP.drawRect(qr);
  if( mNeedOverlay ) mOverlay.push_back( qr );
}

void KigPainter::drawRect( const QRect& r )
{
  mP.drawRect(r);
  if( mNeedOverlay ) mOverlay.push_back( r );
}

void KigPainter::drawCircle( const Coordinate& center, const double radius )
{
  Coordinate bottomLeft = center - Coordinate(radius, radius);
  Coordinate topRight = center + Coordinate(radius, radius);
  Rect r( bottomLeft, topRight );
  QRect qr = toScreen( r );
  mP.drawEllipse ( qr );
  if( mNeedOverlay ) circleOverlay( center, radius );
}

void KigPainter::drawSegment( const Coordinate& from, const Coordinate& to )
{
  QPoint tF = toScreen(from), tT = toScreen(to);
  mP.drawLine( tF, tT );
  if( mNeedOverlay ) segmentOverlay( from, to );
}

void KigPainter::drawFatPoint( const Coordinate& p, double radius )
{
  setBrushStyle( Qt::SolidPattern );
  Coordinate rad( radius, radius );
  rad /= 2;
  Coordinate tl = p - rad;
  Coordinate br = p + rad;
  Rect r( tl, br );
  QRect qr = toScreen( r );
  mP.drawEllipse( qr );
  if( mNeedOverlay ) mOverlay.push_back( qr );
}

void KigPainter::drawPoint( const Coordinate& p )
{
  mP.drawPoint( toScreen(p) );
  if( mNeedOverlay ) pointOverlay( p );
}

void KigPainter::drawLine( const Coordinate& p1, const Coordinate& p2 )
{
  drawLine( LineData( p1, p2 ) );
}

void KigPainter::drawText( const Rect p, const QString s, int textFlags, int len )
{
  QRect t = toScreen(p);
  int tf = textFlags;
  mP.drawText( t, tf, s, len );
  if( mNeedOverlay ) textOverlay( t, s, tf, len );
}

void KigPainter::textOverlay( const QRect& r, const QString s, int textFlags, int len )
{
  //  kdDebug() << Rect::fromQRect( mP.boundingRect( r, textFlags, s, len ) ) << endl;
  mOverlay.push_back( mP.boundingRect( r, textFlags, s, len ) );
}

const Rect KigPainter::boundingRect( const Rect& r, const QString s,
                                      int f, int l ) const
{
  return fromScreen( mP.boundingRect( toScreen( r ), f, s, l ) );
};

void KigPainter::setColor( const QColor& c )
{
  color = c;
  mP.setPen( QPen( color, width, style ) );
}

void KigPainter::setStyle( const PenStyle c )
{
  style = c;
  mP.setPen( QPen( color, width, style ) );
}

void KigPainter::setWidth( const uint c )
{
  width = c;
  mP.setPen( QPen( color, width, style ) );
}

void KigPainter::setPen( const QPen& p )
{
  color = p.color();
  width = p.width();
  style = p.style();
  mP.setPen(p);
}

void KigPainter::setBrush( const QBrush& b )
{
  brushStyle = b.style();
  brushColor = b.color();
  mP.setBrush( b );
}

void KigPainter::setBrushStyle( const BrushStyle c )
{
  brushStyle = c;
  mP.setBrush( QBrush( brushColor, brushStyle ) );
}

void KigPainter::setBrushColor( const QColor& c )
{
  brushColor = c;
  mP.setBrush( QBrush( brushColor, brushStyle ) );
}

void KigPainter::drawPolygon( const std::vector<Coordinate>& pts, bool winding, int index, int npoints )
{
  Rect sr;
  // i know this isn't really fast, but i blame it all on Qt with its
  // stupid container classes... what's wrong with the STL ?
  QPointArray t( pts.size() );
  int c = 0;
  for( std::vector<Coordinate>::const_iterator i = pts.begin(); i != pts.end(); ++i )
    {
      sr.setContains( *i );
      QPoint tt = toScreen (*i);
      t.putPoints( c++, 1, tt.x(), tt.y() );
    };
  mP.drawPolygon( t, winding, index, npoints );
  mOverlay.push_back( toScreen( sr ) );
}

Rect KigPainter::window()
{
  return msi.shownRect();
}

void KigPainter::circleOverlayRecurse( const Coordinate& centre, double radius,
				       const Rect& cr )
{
  Rect currentRect = cr.normalized();

  if( !currentRect.intersects( window() ) ) return;

  // this code is an adaptation of Marc Bartsch's code, from KGeo
  Coordinate tl = currentRect.topLeft();
  Coordinate br = currentRect.bottomRight();
  Coordinate tr = currentRect.topRight();
  Coordinate bl = currentRect.bottomLeft();
  Coordinate c = currentRect.center();

  // 1.415 should actually be 1.414...
  double fault = currentRect.width()*1.415 + pixelWidth();
  double radiusBig = radius + fault ;
  double radiusSmall = radius - fault ;

  // if the circle doesn't touch this rect, we return
  // too far from the centre
  if (((tl - centre).length() > radiusBig) &&
      ((tr - centre).length() > radiusBig) &&
      ((bl - centre).length() > radiusBig) &&
      ((br - centre).length() > radiusBig))
    return;

  // too near to the centre
  if (((tl - centre).length() < radiusSmall) &&
      ((tr - centre).length() < radiusSmall) &&
      ((bl - centre).length() < radiusSmall) &&
      ((br - centre).length() < radiusSmall))
    return;

  // the rect contains some of the circle
  // -> if it's small enough, we keep it
  if( currentRect.width() < overlayRectSize() ) {
    mOverlay.push_back( toScreen( currentRect) );
  } else {
    // this func works recursive: we subdivide the current rect, and if
    // it is of a good size, we keep it, otherwise we handle it again
    double width = currentRect.width() / 2;
    double height = currentRect.height() / 2;
    Rect r1 ( c, -width, -height);
    r1.normalize();
    circleOverlayRecurse(centre, radius, r1);
    Rect r2 ( c, width, -height);
    r2.normalize();
    circleOverlayRecurse(centre, radius, r2);
    Rect r3 ( c, -width, height);
    r3.normalize();
    circleOverlayRecurse(centre, radius, r3);
    Rect r4 ( c, width, height);
    r4.normalize();
    circleOverlayRecurse(centre, radius, r4);
  };
};

void KigPainter::circleOverlay( const Coordinate& centre, double radius )
{
  double t = radius + pixelWidth();
  Coordinate r( t, t );
  Coordinate bottomLeft = centre - r;
  Coordinate topRight = centre + r;
  Rect rect( bottomLeft, topRight );
  circleOverlayRecurse ( centre , radius, rect );
}

void KigPainter::segmentOverlay( const Coordinate& p1, const Coordinate& p2 )
{
  // this code is based upon what Marc Bartsch wrote for KGeo

  // some stuff we may need:
  Coordinate p3 = p2 - p1;
  Rect border = window();
  double length = p3.length();
  // if length is smaller than one, we risk getting a divide by zero
  length = std::max( length, 1. );
  p3 *= overlayRectSize();
  p3 /= length;

  int counter = 0;

  Rect r(p1, p2);
  r.normalize();

  for (;;) {
    Rect tR( Coordinate( 0, 0 ), overlayRectSize(), overlayRectSize() );
    Coordinate tP = p1+p3*counter;
    tR.setCenter(tP);
    if (!tR.intersects(r))
      {
	//kdDebug()<< "stopped after "<< counter << " passes." << endl;
	break;
      }
    if (tR.intersects(border)) mOverlay.push_back( toScreen( tR ) );
    if (++counter > 100)
      {
	kdError()<< k_funcinfo << "counter got too big :( " << endl;
	break;
      }
    }
}

double KigPainter::overlayRectSize()
{
  return 20 * pixelWidth();
}

void KigPainter::pointOverlay( const Coordinate& p1 )
{
  Rect r( p1, 3*pixelWidth(), 3*pixelWidth());
  r.setCenter( p1 );
  mOverlay.push_back( toScreen( r) );
}

double KigPainter::pixelWidth()
{
  return msi.pixelWidth();
}

void KigPainter::setWholeWinOverlay()
{
  mOverlay.clear();
  mOverlay.push_back( mP.viewport() );
  // don't accept any more overlay's...
  mNeedOverlay = false;
}

QPoint KigPainter::toScreen( const Coordinate p ) const
{
  return msi.toScreen( p );
}

void KigPainter::drawGrid( const CoordinateSystem& c )
{
  c.drawGrid( *this );
  setWholeWinOverlay();
}

void KigPainter::drawObject( const Object* o, bool ss )
{
  o->drawWrap( *this, ss );
}

void KigPainter::drawObjects( const Objects& os )
{
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    drawObject( *i );
  };
}

void KigPainter::drawFilledRect( const QRect& r )
{
  QPen pen( Qt::black, 1, Qt::DotLine );
  setPen( pen );
  setBrush( QBrush( Qt::cyan, Dense6Pattern ) );
  drawRect( r.normalize() );
}

void KigPainter::drawTextStd( const QPoint& p, const QString& s )
{
  if ( ! s ) return;
  // tf = text formatting flags
  int tf = AlignLeft | AlignTop | DontClip | WordBreak;
  // we need the rect where we're going to paint text
  setPen(QPen(Qt::blue, 1, SolidLine));
  setBrush(Qt::NoBrush);
  drawText( Rect(
              msi.fromScreen(p), window().bottomRight()
              ).normalized(), s, tf );

}

QRect KigPainter::toScreen( const Rect r ) const
{
  return msi.toScreen( r );
}

void KigPainter::drawSimpleText( const Coordinate& c, const QString s )
{
  int tf = AlignLeft | AlignTop | DontClip | WordBreak;
  drawText( c, s, tf);
}

void KigPainter::drawText( const Coordinate p, const QString s,
                           int textFlags, int len )
{
  drawText( Rect( p, mP.window().right(), mP.window().top() ),
            s, textFlags, len );
}
const Rect KigPainter::simpleBoundingRect( const Coordinate& c, const QString s )
{
  int tf = AlignLeft | AlignTop | DontClip | WordBreak;
  return boundingRect( c, s, tf );
}

const Rect KigPainter::boundingRect( const Coordinate& c, const QString s,
                                      int f, int l ) const
{
  return boundingRect( Rect( c, mP.window().right(), mP.window().top() ),
                       s, f, l );
}

Coordinate KigPainter::fromScreen( const QPoint& p ) const
{
  return msi.fromScreen( p );
}

Rect KigPainter::fromScreen( const QRect& r ) const
{
  return msi.fromScreen( r );
}

void KigPainter::drawRay( const Coordinate& a, const Coordinate& b )
{
  Coordinate tb = b;
  calcRayBorderPoints( a, tb, window() );
  drawSegment( a, tb );
}

void KigPainter::drawArc( const Rect& surroundingRect, int startAngle, int angle )
{
  mP.drawArc( toScreen( surroundingRect ), startAngle, angle );
  Rect r = surroundingRect;
  r *= 1.2;
  r.setCenter( surroundingRect.center() );
//  if ( mNeedOverlay ) mOverlay.push_back( toScreen( r ) );
  setWholeWinOverlay();
}

void KigPainter::drawFatPoint( const Coordinate& p )
{
    drawFatPoint( p, 5 * pixelWidth() );
}

inline Coordinate conicGetCoord( double theta, double ecostheta0,
                                 double esintheta0, double pdimen,
                                 const Coordinate& focus1 )
{
  // TODO: eliminate some cos() and sin() calls with a table...
  double costheta = cos( theta );
  double sintheta = sin( theta );
  double ecosthetamtheta = costheta*ecostheta0 + sintheta*esintheta0;
  double rho = pdimen / (1.0 - ecosthetamtheta);
  return focus1 + rho * Coordinate( costheta, sintheta );
};

void KigPainter::drawConic( const ConicPolarEquationData& data )
{
  // @author Maurizio Paolini wrote an original version of this, ( the
  // math in conicGetCoord() is still his ), I ( Dominique ) adapted
  // it afterwards to use a distribution mechanism as explained
  // below. )

  // this function works more or less like Locus::calcForWidget():
  // we calc some initial points, draw them, and then keep checking if
  // the point with the param in between two other points.  If the
  // point with that param is not too close to one of them, and if it
  // is in the current window(), then we draw it, and put it on the
  // stack for further processing...

  Coordinate focus1 = data.focus1;
  double pdimen = data.pdimen;
  double ecostheta0 = data.ecostheta0;
  double esintheta0 = data.esintheta0;

  // we manage our own overlay
  bool tNeedOverlay = mNeedOverlay;
  mNeedOverlay = false;
  Rect overlay;

  // this stack contains pairs of Coordinates that we still need to
  // process:
  typedef std::pair<double,Coordinate> coordparampair;
  typedef std::pair<coordparampair, coordparampair> workitem;
  std::stack<workitem> workstack;

  // first work on some 20 initial points and push them onto the
  // stack.. :
  coordparampair prev =
    coordparampair( 0, conicGetCoord( 0, ecostheta0, esintheta0,
                                      pdimen, focus1 ) );

  for ( double i = 1./20; i <= 2*M_PI; i += 1./20 )
  {
    Coordinate c = conicGetCoord( i, ecostheta0, esintheta0,
                                  pdimen, focus1 );
    drawPoint( c );
    workstack.push( workitem( coordparampair( i, c ), prev ) );
    prev = coordparampair( i, c );
  };

  // maxlength is the square of the maximum size that we allow
  // between two points..
  double maxlength = 1.5 * pixelWidth();
  maxlength *= maxlength;

  // sanity check...
  int count = 20;              // the number of points we've already
                               // visited...
  static const int maxnumberofpoints = 1000;

  const Rect& sr = window();

  // we don't use recursion, but a stack based approach for efficiency
  // concerns...
  while ( ! workstack.empty() && count < maxnumberofpoints )
  {
    workitem curitem = workstack.top();
    workstack.pop();
    for ( ; count < maxnumberofpoints; ++count )
    {
      // we take the middle parameter of the two previous points...
      double p = ( curitem.first.first + curitem.second.first ) / 2;
      Coordinate n = conicGetCoord( p, ecostheta0, esintheta0,
                                    pdimen, focus1 );
      bool addn = sr.contains( n );
      bool followfirst = addn &&
                         (n-curitem.first.second).squareLength() > maxlength;
      bool followlast = addn &&
                        (n-curitem.second.second).squareLength() > maxlength;
      if ( addn )
      {
        drawPoint( n );
        overlay.setContains( n );
      };
      if ( followfirst ) workstack.push( workitem( curitem.first,
                                                   coordparampair( p, n ) ) );
      // we don't push here, but simply set curitem to a new item, and
      // restart the inner for loop, to save some stack pushes...
      if ( followlast ) curitem = workitem( coordparampair( p, n ),
                                            curitem.second );
      else break;
    };
  };

  mOverlay.push_back( toScreen( overlay ) );
  mNeedOverlay = tNeedOverlay;
}

void KigPainter::drawLine( const LineData& d )
{
  LineData l = calcBorderPoints( d, window() );
  drawSegment( l );
}

void KigPainter::drawSegment( const LineData& d )
{
  drawSegment( d.a, d.b );
}
