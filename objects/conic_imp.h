// conic_imp.h
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

#ifndef KIG_OBJECTS_CONIC_IMP_H
#define KIG_OBJECTS_CONIC_IMP_H

#include "curve_imp.h"

#include "../misc/conic-common.h"

class ConicImp
  : public CurveImp
{
protected:
  ConicImp();
  ~ConicImp();
public:
  typedef CurveImp Parent;
  static const ObjectImpType* stype();

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  bool valid() const;

  const uint numberOfProperties() const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  const char* iconForProperty( uint which ) const;
  ObjectImp* property( uint which, const KigDocument& w ) const;

  double getParam( const Coordinate& point, const KigDocument& ) const;
  const Coordinate getPoint( double param, bool& valid, const KigDocument& ) const;

  // information about ourselves..  These are all virtual, because a
  // trivial subclass like CircleImp can override these with trivial
  // versions..
  virtual int conicType() const;
  virtual QString conicTypeString() const;
  virtual QString cartesianEquationString( const KigDocument& w ) const;
  virtual QString polarEquationString( const KigDocument& w ) const;
  virtual const ConicCartesianData cartesianData() const;
  virtual const ConicPolarData polarData() const = 0;
  virtual Coordinate focus1() const;
  virtual Coordinate focus2() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool equals( const ObjectImp& rhs ) const;
};

class ConicImpCart
  : public ConicImp
{
  ConicCartesianData mcartdata;
  ConicPolarData mpolardata;
public:
  ConicImpCart( const ConicCartesianData& data );
  ~ConicImpCart();
  ConicImpCart* copy() const;

  const ConicCartesianData cartesianData() const;
  const ConicPolarData polarData() const;
};

class ConicImpPolar
  : public ConicImp
{
  ConicPolarData mdata;
public:
  ConicImpPolar( const ConicPolarData& data );
  ~ConicImpPolar();
  ConicImpPolar* copy() const;

  const ConicPolarData polarData() const;
};

#endif
