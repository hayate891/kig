/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>
 Copyright (C) 2003  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_MISC_KIGTRANSFORM_H
#define KIG_MISC_KIGTRANSFORM_H

#include "coordinate.h"

class LineData;

/**
 * Class representing a transformation.  More specifically, this class
 * represents a generic affine 2-dimensional transformation. Various
 * common transformations can be used.
 */
class Transformation
{
  double mdata[3][3];
  bool mIsHomothety;
  Transformation();
public:
  ~Transformation();
  Transformation( double data[3][3], bool ishomothety );

  /**
   * Apply.  Apply this transformation to the Coordinate c.
   */
  const Coordinate apply( const Coordinate& c ) const;

  bool isHomothetic() const;
  double getAffineDeterminant() const;
  double getRotationAngle() const;
  const Coordinate apply2by2only( const Coordinate& c ) const;
  // a homothetic transformation maintains the ratio's of lengths.
  // This means that every length is multiplied by a fixed number when
  // it is projected...  This function does that calculation for
  // you..
  double apply( double length ) const;
  double data( int r, int c ) const;
  const Transformation inverse( bool& valid ) const;

  // don't do anything..
  static const Transformation identity();
  // scale over a point..
  static const Transformation scaling( double factor, const Coordinate& center = Coordinate() );
  // scale over a line..
  static const Transformation scaling( double factor, const LineData& l );
  // translate..
  static const Transformation translation( const Coordinate& c );
  // rotate..
  static const Transformation rotation( double angle, const Coordinate& center = Coordinate() );
  // reflect over a point..  this equals scaling( -1, c )
  static const Transformation pointReflection( const Coordinate& c );
  // reflect over a line.. this equals scaling( -1, l );
  static const Transformation lineReflection( const LineData& l );
  // cast a shadow, given a light source and a line indicating a plane
  static const Transformation castShadow( const Coordinate& ls,
                                              const LineData& d );
  // this is a test example of a projective non-affine transformation
  static const Transformation projectiveRotation( double alpha,
                                              const Coordinate& d,
                                              const Coordinate& t );

  friend const Transformation operator*( const Transformation&, const Transformation& );
};

const Transformation operator*( const Transformation&, const Transformation& );
bool operator==( const Transformation& lhs, const Transformation& rhs );

class Object;
class Objects;

// enum tWantArgsResult { tComplete, tNotComplete, tNotGood };

// Transformation getProjectiveTransformation(
//   int transformationsnum, Object *mtransformations[],
//   bool& valid );

// tWantArgsResult WantTransformation ( Objects::const_iterator& i,
//          const Objects& os );

// QString getTransformMessage ( const Objects& os, const Object *o );

// bool isHomoteticTransformation ( double transformation[3][3] );

#endif // KIG_MISC_KIGTRANSFORM_H
