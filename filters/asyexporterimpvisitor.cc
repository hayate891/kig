// Copyright (C)  2010,2011 Raoul Bourquin

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "asyexporterimpvisitor.h"

#include "../misc/goniometry.h"
#include "../objects/circle_imp.h"
#include "../objects/cubic_imp.h"
#include "../objects/bezier_imp.h"
#include "../objects/curve_imp.h"
#include "../objects/line_imp.h"
#include "../objects/locus_imp.h"
#include "../objects/object_drawer.h"
#include "../objects/object_holder.h"
#include "../objects/object_imp.h"
#include "../objects/other_imp.h"
#include "../objects/point_imp.h"
#include "../objects/polygon_imp.h"
#include "../objects/text_imp.h"


void AsyExporterImpVisitor::newLine()
{
  mstream << "\n";
}


QString AsyExporterImpVisitor::emitColor( const QColor& c )
{
  QString ret("");
  ret = "rgb(" + QString::number(c.red()) + "," + QString::number(c.green()) + "," + QString::number(c.blue()) + ")";
  return ret;
}


QString AsyExporterImpVisitor::emitStyle( Qt::PenStyle style )
{
  QString linetype("");
  if ( style == Qt::SolidLine ) {
    linetype = "solid";
  } else if ( style == Qt::DashLine ) {
    linetype = "dashed";
  } else if ( style == Qt::DotLine ) {
    linetype = "dotted";
  } else if ( style == Qt::DashDotLine ) {
    linetype = "dashdotted";
  } else if ( style == Qt::DashDotDotLine ) {
    linetype = "longdashdotted";
  }
  return linetype;
}


QString AsyExporterImpVisitor::emitCoord( const Coordinate& c )
{
  QString ret("");
  ret = "(" + QString::number(c.x) + "," + QString::number(c.y) + ")";
  return ret;
}


void AsyExporterImpVisitor::emitLine( const Coordinate& a, const Coordinate& b,
                                      const int width, const Qt::PenStyle s,
                                      bool vector )
{
  mstream << "path line = "
          << emitCoord( a )
          << "--"
          << emitCoord( b )
          << ";";
  newLine();

  if ( vector == true )
  {
    mstream << "draw(line, "
	    << emitColor( mcurobj->drawer()->color() )
	    << "+"
	    << emitStyle( s )
	    << ", Arrow );";
  }
  else
  {
    mstream << "draw(line, "
	    << emitColor( mcurobj->drawer()->color() )
	    << "+"
	    << emitStyle( s )
	    << " );";
  }
  newLine();
}


double AsyExporterImpVisitor::dimRealToCoord( int dim )
{
  QRect qr( 0, 0, dim, dim );
  Rect r = mw.screenInfo().fromScreen( qr );
  return fabs( r.width() );
}


void AsyExporterImpVisitor::visit( ObjectHolder* obj )
{
  if ( ! obj->drawer()->shown() )
    return;
  mcurobj = obj;
  obj->imp()->visit( this );
}


void AsyExporterImpVisitor::plotGenericCurve( const CurveImp* imp )
{
  std::vector< std::vector< Coordinate > > coordlist;
  coordlist.push_back( std::vector< Coordinate >() );
  uint curid = 0;

  Coordinate c;
  Coordinate prev = Coordinate::invalidCoord();
  for ( double i = 0.0; i <= 1.0; i += 0.0001 )
  {
    c = imp->getPoint( i, mw.document() );
    if ( !c.valid() )
    {
      if ( coordlist[curid].size() > 0 )
      {
        coordlist.push_back( std::vector< Coordinate >() );
        ++curid;
        prev = Coordinate::invalidCoord();
      }
      continue;
    }
    if ( ! ( ( fabs( c.x ) <= 10000 ) && ( fabs( c.y ) <= 10000 ) ) )
      continue;
    // if there's too much distance between this coordinate and the previous
    // one, then it's another piece of curve not joined with the rest
    if ( prev.valid() && ( c.distance( prev ) > 50.0 ) )
    {
      coordlist.push_back( std::vector< Coordinate >() );
      ++curid;
    }
    coordlist[curid].push_back( c );
    prev = c;
  }
  // special case for ellipse
  if ( const ConicImp* conic = dynamic_cast< const ConicImp* >( imp ) )
  {
    // if ellipse, close its path
    // THSI IS WRONG, think of ellipse arcs!!
    if ( conic->conicType() == 1 && coordlist.size() == 1 && coordlist[0].size() > 1 )
    {
      coordlist[0].push_back( coordlist[0][0] );
    }
  }
  for ( uint i = 0; i < coordlist.size(); ++i )
  {
    uint s = coordlist[i].size();
    // there's no point in draw curves empty or with only one point
    if ( s <= 1 )
      continue;

    uint linelength = 13;
    QString tmp;
    mstream << "path curve = ";
    for ( uint j = 0; j < s; ++j )
    {
      tmp = emitCoord( coordlist[i][j] );
      // Avoid too long lines in the output file
      if(linelength + tmp.length() > maxlinelength)
      {
	linelength = tmp.length();
	newLine();
      }
      else
      {
	linelength += tmp.length();
      }
      mstream << tmp;
      if ( j < s-1 )
      {
	linelength += 2;
	mstream << "--";
      }
      else
      {
	mstream << ";";
	newLine();
	linelength = 0;
      }
    }
    mstream << "draw(curve, "
	    << emitColor( mcurobj->drawer()->color() )
	    << "+"
	    << emitStyle( mcurobj->drawer()->style() )
	    << " );";
    newLine();
  }
}


void AsyExporterImpVisitor::visit( const LineImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  calcBorderPoints( a, b, msr );

  int width = 1;
  emitLine( a, b, width, mcurobj->drawer()->style() );
}


void AsyExporterImpVisitor::visit( const PointImp* imp )
{
  mstream << "pair point = "
          << emitCoord( imp->coordinate() )
          << ";";
  newLine();
  mstream << "dot(point, "
	  << emitColor( mcurobj->drawer()->color() )
	  << ");";
  newLine();
}


void AsyExporterImpVisitor::visit( const TextImp* imp )
{
  // FIXME: support multiline texts...
  mstream << "pair anchor = "
          << emitCoord( imp->coordinate() )
          << ";";
  newLine();
  mstream << "Label l = Label(\""
	  << imp->text()
	  << "\", "
	  << emitColor( mcurobj-> drawer()->color() )
	  << ");";
  newLine();
  if ( imp->hasFrame() )
  {
    mstream << "draw(l, box, anchor, textboxmargin);";
  }
  else
  {
    mstream << "draw(l, anchor);";
  }
  newLine();
}


void AsyExporterImpVisitor::visit( const AngleImp* imp )
{
  const Coordinate center = imp->point();
  // TODO: How to choose radius size?
  const double radius = 0.5;
  double startangle = imp->startAngle();
  double endangle = startangle + imp->angle();

  startangle = Goniometry::convert( startangle, Goniometry::Rad, Goniometry::Deg );
  endangle = Goniometry::convert( endangle, Goniometry::Rad, Goniometry::Deg );
  
  // TODO: Allow arrow tips?
  
  mstream << "path a = Arc("
	  << emitCoord(center)
	  << ", "
	  << radius
	  << ", "
	  << startangle
	  << ", "
	  << endangle
	  << " );";
  newLine();
  mstream << "draw(a, "
	  << emitColor( mcurobj->drawer()->color() )
	  << "+"
	  << emitStyle( mcurobj->drawer()->style() )
	  << " );";
  newLine();
}


void AsyExporterImpVisitor::visit( const VectorImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;

  int width = 1;
  emitLine( a, b, width, mcurobj->drawer()->style(), true );
}


void AsyExporterImpVisitor::visit( const LocusImp* imp )
{
  plotGenericCurve( imp );
}


void AsyExporterImpVisitor::visit( const CircleImp* imp )
{
  mstream << "pair center = "
          << emitCoord( imp->center() )
          << ";";
  newLine();
  mstream << "real radius = " << imp->radius()
          << ";";
  newLine();
  mstream << "path circle = Circle(center, radius);";
  newLine();
  mstream << "draw(circle, "
	  << emitColor( mcurobj->drawer()->color() )
	  << "+"
	  << emitStyle( mcurobj->drawer()->style() )
	  << " );";
  newLine();
}


void AsyExporterImpVisitor::visit( const ConicImp* imp )
{
  plotGenericCurve( imp );
}


void AsyExporterImpVisitor::visit( const CubicImp* )
{
  // FIXME: cubic are not drawn correctly with plotGenericCurve
  //  plotGenericCurve( imp );
}


void AsyExporterImpVisitor::visit( const SegmentImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;

  int width = 1;
  emitLine( a, b, width, mcurobj->drawer()->style() );
}


void AsyExporterImpVisitor::visit( const RayImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  calcRayBorderPoints( a, b, msr );

  int width = 1;
  emitLine( a, b, width, mcurobj->drawer()->style() );
}


void AsyExporterImpVisitor::visit( const ArcImp* imp )
{
  const Coordinate center = imp->center();
  const double radius = imp->radius();
  double startangle = imp->startAngle();
  double endangle = startangle + imp->angle();

  startangle = Goniometry::convert( startangle, Goniometry::Rad, Goniometry::Deg );
  endangle = Goniometry::convert( endangle, Goniometry::Rad, Goniometry::Deg );

  mstream << "path arc = Arc("
	  << emitCoord(center)
	  << ", "
	  << radius
	  << ", "
	  << startangle
	  << ", "
	  << endangle
	  << " );";
  newLine();
  mstream << "draw(arc, "
	  << emitColor( mcurobj->drawer()->color() )
	  << "+"
	  << emitStyle( mcurobj->drawer()->style() )
	  << " );";
  newLine();
}


void AsyExporterImpVisitor::visit( const FilledPolygonImp* imp )
{
  uint linelength = 15;
  QString tmp;
  mstream << "path polygon = ";
  std::vector<Coordinate> pts = imp->points();
  for ( uint i = 0; i < pts.size(); i++ )
  {
    tmp = emitCoord( pts[i] );
    tmp.append("--");
    if ( linelength + tmp.length() > maxlinelength )
    {
      newLine();
      linelength = tmp.length();
    }
    else
    {
      linelength += tmp.length();
    }
    mstream << tmp;
  }
  mstream << "cycle;";
  newLine();
  mstream << "fill(polygon, "
	  << emitColor( mcurobj->drawer()->color() )
	  << "+opacity(0.5) );";
  newLine();
  mstream << "draw(polygon, "
	  << emitColor( mcurobj->drawer()->color() )
	  << "+"
	  << emitStyle( mcurobj->drawer()->style() )
	  << " );";
  newLine();
}


void AsyExporterImpVisitor::visit(const ClosedPolygonalImp* imp)
{
  uint linelength = 15;
  QString tmp;
  mstream << "path polygon = ";
  std::vector<Coordinate> pts = imp->points();
  for ( uint i = 0; i < pts.size(); i++ )
  {
    tmp = emitCoord( pts[i] );
    tmp.append("--");
    if ( linelength + tmp.length() > maxlinelength )
    {
      newLine();
      linelength = tmp.length();
    }
    else
    {
      linelength += tmp.length();
    }
    mstream << tmp;
  } 
  mstream << "cycle;";
  newLine();
  mstream << "draw(polygon, "
	  << emitColor( mcurobj->drawer()->color() )
	  << "+"
	  << emitStyle( mcurobj->drawer()->style() )
	  << " );";
  newLine();
}


void AsyExporterImpVisitor::visit(const OpenPolygonalImp* imp)
{
  uint linelength = 15;
  QString tmp;
  mstream << "path polygon = ";
  std::vector<Coordinate> pts = imp->points();
  for ( uint i = 0; i < pts.size(); i++ )
  {
    tmp = emitCoord( pts[i] );
    if ( linelength + tmp.length() > maxlinelength )
    {
      newLine();
      linelength = tmp.length();
    }
    else
    {
      linelength += tmp.length();
    }
    mstream << tmp;
    if ( i < pts.size()-1 ) 
    {
      linelength += 2;
      mstream << "--";
    }
    else
    {
      linelength += 1;
      mstream << ";";
    }
  }
  newLine();
  mstream << "draw(polygon, "
	  << emitColor( mcurobj->drawer()->color() )
	  << "+"
	  << emitStyle( mcurobj->drawer()->style() )
	  << " );";
  newLine();
}


void AsyExporterImpVisitor::visit ( const BezierImp* imp )
{
  std::vector<Coordinate> pts = imp->points();
  switch ( pts.size() )
  {
  case 3:
    // Formula for cubic control points
    // CP1 = QP0 + 2/3 *(QP1-QP0)
    // CP2 = CP1 + 1/3 *(QP2-QP0)
    mstream << "pair cp1 = " << emitCoord(pts.at(0)) << " +2/3*(" << emitCoord(pts.at(1)) << "-" << emitCoord(pts.at(0)) << ");";
    newLine();
    mstream << "pair cp2 = cp1 +1/3*(" << emitCoord(pts.at(2)) << "-" << emitCoord(pts.at(0)) << ");";
    newLine();
    mstream << "path bezier = ";
    mstream << emitCoord( pts.at(0) );
    mstream << " .. controls cp1 and cp2 .. ";
    mstream << emitCoord( pts.at(2) );
    mstream << ";";
    newLine();
    mstream << "draw(bezier, "
            << emitColor( mcurobj->drawer()->color() )
            << "+"
            << emitStyle( mcurobj->drawer()->style() )
            << " );";
    newLine();
    break;
  case 4:
    mstream << "path bezier = ";
    mstream << emitCoord( pts.at(0) );
    mstream << " .. controls ";
    mstream << emitCoord( pts.at(1) );
    mstream << " and ";
    mstream << emitCoord( pts.at(2) );
    mstream << " .. ";
    mstream << emitCoord( pts.at(3) );
    mstream << ";";
    newLine();
    mstream << "draw(bezier, "
            << emitColor( mcurobj->drawer()->color() )
            << "+"
            << emitStyle( mcurobj->drawer()->style() )
            << " );";
    newLine();
    break;
  default:
    plotGenericCurve ( imp );
    break;
  }
}


void AsyExporterImpVisitor::visit ( const RationalBezierImp* imp )
{
  plotGenericCurve ( imp );
}
