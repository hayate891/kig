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


#ifndef CONICSEXTRA_H
#define CONICSEXTRA_H

#include "point.h"
#include "line.h"

// the polar line of a point with respect to a conic

class LinePolar
  : public Line
{
public:
  LinePolar( const Objects& os );
  LinePolar( const LinePolar& l );
  ~LinePolar();
  LinePolar* copy() { return new LinePolar (*this);};

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return ("LinePolar"); };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "polarline"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // arguments
  static void sDrawPrelim ( KigPainter& p, const Objects& os );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );

  Objects getParents() const;

  void calc();

protected:
  Conic* conic;
  Point* pole;
};

// the polar point of a line with respect to a conic

class PointPolar
  : public Point
{
public:
  PointPolar( const Objects& os );
  PointPolar( const PointPolar& l );
  ~PointPolar();
  PointPolar* copy() { return new PointPolar (*this);};

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return ("PointPolar"); };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "polarpoint"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // arguments
  static void sDrawPrelim ( KigPainter& p, const Objects& os );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );

  Objects getParents() const;

  void calc();

protected:
  Conic* conic;
  AbstractLine* polar;
};

#endif
