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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "object_drawer.h"

#include "object_imp.h"
#include "../misc/kigpainter.h"

#include <qpen.h>
#include <qnamespace.h>
#include <cassert>

#include <QDebug>

void ObjectDrawer::draw( const ObjectImp& imp, KigPainter& p, bool sel ) const
{
  bool nv = p.getNightVision( );
  if ( mshown || nv )
  {
    p.setBrushStyle( Qt::NoBrush );
    p.setBrushColor( sel ? Qt::red : ( mshown?mcolor:Qt::gray ) );
    p.setPen( QPen ( sel ? Qt::red : ( mshown?mcolor:Qt::gray ),  1) );
    p.setWidth( mwidth );
    p.setStyle( mstyle );
    p.setPointStyle( mpointstyle );
    p.setFont( mfont );
    p.setSelected( sel );
    imp.draw( p );
  }
}

bool ObjectDrawer::contains( const ObjectImp& imp, const Coordinate& pt, const KigWidget& w, bool nv ) const
{
  bool shownornv = mshown || nv;
  return shownornv && imp.contains( pt, mwidth, w );
}

bool ObjectDrawer::shown( ) const
{
  return mshown;
}

QColor ObjectDrawer::color() const
{
  return mcolor;
}

ObjectDrawer* ObjectDrawer::getCopyShown( bool s ) const
{
  ObjectDrawer* ret = new ObjectDrawer;
  ret->mcolor = mcolor;
  ret->mshown = s;
  ret->mwidth = mwidth;
  ret->mstyle = mstyle;
  ret->mpointstyle = mpointstyle;
  ret->mfont = mfont;
  return ret;
}

ObjectDrawer* ObjectDrawer::getCopyColor( const QColor& c ) const
{
  ObjectDrawer* ret = new ObjectDrawer;
  ret->mcolor = c;
  ret->mshown = mshown;
  ret->mwidth = mwidth;
  ret->mstyle = mstyle;
  ret->mpointstyle = mpointstyle;
  ret->mfont = mfont;
  return ret;
}

ObjectDrawer* ObjectDrawer::getCopyWidth( int w ) const
{
  ObjectDrawer* ret = new ObjectDrawer;
  ret->mcolor = mcolor;
  ret->mshown = mshown;
  ret->mwidth = w;
  ret->mstyle = mstyle;
  ret->mpointstyle = mpointstyle;
  ret->mfont = mfont;
  return ret;
}

ObjectDrawer* ObjectDrawer::getCopyStyle( Qt::PenStyle s ) const
{
  ObjectDrawer* ret = new ObjectDrawer;
  ret->mcolor = mcolor;
  ret->mshown = mshown;
  ret->mwidth = mwidth;
  ret->mstyle = s;
  ret->mpointstyle = mpointstyle;
  ret->mfont = mfont;
  return ret;
}

ObjectDrawer* ObjectDrawer::getCopyPointStyle( Kig::PointStyle p ) const
{
  ObjectDrawer* ret = new ObjectDrawer;
  ret->mcolor = mcolor;
  ret->mshown = mshown;
  ret->mwidth = mwidth;
  ret->mstyle = mstyle;
  ret->mpointstyle = p;
  ret->mfont = mfont;
  return ret;
}

ObjectDrawer* ObjectDrawer::getCopyFont( const QFont& f ) const
{
  ObjectDrawer* ret = new ObjectDrawer;
  ret->mcolor = mcolor;
  ret->mshown = mshown;
  ret->mwidth = mwidth;
  ret->mstyle = mstyle;
  ret->mpointstyle = mpointstyle;
  ret->mfont = f;
  return ret;
}

int ObjectDrawer::width() const
{
  return mwidth;
}

Qt::PenStyle ObjectDrawer::style() const
{
  return mstyle;
}

Kig::PointStyle ObjectDrawer::pointStyle() const
{
  return mpointstyle;
}

QFont ObjectDrawer::font() const
{
  return mfont;
}

ObjectDrawer::ObjectDrawer( const QColor& color, int width, bool shown, Qt::PenStyle style, Kig::PointStyle pointStyle, const QFont& f )
  : mcolor( color ), mshown( shown ), mwidth( width ), mstyle( style ), mpointstyle( pointStyle ), mfont( f )
{
}

ObjectDrawer::ObjectDrawer()
  : mcolor( Qt::blue ), mshown( true ), mwidth( -1 ), mstyle( Qt::SolidLine ), mpointstyle( Kig::Round ), mfont( QFont() )
{
}

bool ObjectDrawer::inRect( const ObjectImp& imp, const Rect& r, const KigWidget& w ) const
{
  return mshown && imp.inRect( r, mwidth, w );
}

Qt::PenStyle ObjectDrawer::styleFromString( const QString& style )
{
  if ( style == "SolidLine" )
    return Qt::SolidLine;
  else if ( style == "DashLine" )
    return Qt::DashLine;
  else if ( style == "DotLine" )
    return Qt::DotLine;
  else if ( style == "DashDotLine" )
    return Qt::DashDotLine;
  else if ( style == "DashDotDotLine" )
    return Qt::DashDotDotLine;
  else return Qt::SolidLine;
}

QString ObjectDrawer::styleToString() const
{
  if ( mstyle == Qt::SolidLine )
    return "SolidLine";
  else if ( mstyle == Qt::DashLine )
    return "DashLine";
  else if ( mstyle == Qt::DotLine )
    return "DotLine";
  else if ( mstyle == Qt::DashDotLine )
    return "DashDotLine";
  else if ( mstyle == Qt::DashDotDotLine )
    return "DashDotDotLine";
  return "SolidLine";
}
