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

#ifndef KIG_MISC_SPECIAL_CONSTRUCTORS_H
#define KIG_MISC_SPECIAL_CONSTRUCTORS_H

#include "object_constructor.h"

class TwoOrOneIntersectionConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype_std;
  const ArgsParserObjectType* mtype_special;
  ArgsParser margsparser;
public:
  TwoOrOneIntersectionConstructor( const ArgsParserObjectType* t_std,
                                   const ArgsParserObjectType* t_special,
                                   const char* iconfile,
                                   const struct ArgsParser::spec argsspecv[] );
  ~TwoOrOneIntersectionConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
};

class ThreeTwoOneIntersectionConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype_std;
  const ArgsParserObjectType* mtype_special;
  ArgsParser margsparser;
public:
  ThreeTwoOneIntersectionConstructor( const ArgsParserObjectType* t_std,
                                   const ArgsParserObjectType* t_special,
                                   const char* iconfile,
                                   const struct ArgsParser::spec argsspecv[] );
  ~ThreeTwoOneIntersectionConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
};


class PolygonVertexTypeConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
  ArgsParser margsparser;
public:
  PolygonVertexTypeConstructor();
  ~PolygonVertexTypeConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
};

class PolygonSideTypeConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
  ArgsParser margsparser;
public:
  PolygonSideTypeConstructor();
  ~PolygonSideTypeConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
};

/*
 */

class PointSequenceConstructor
  : public ObjectConstructor
{
  const char* mdescname;
  const char* mdesc;
  const char* miconfile;
  const ObjectType* mtype;
public:
  PointSequenceConstructor( const char* descname,
			    const char* desc,
			    const char* iconfile,
			    const ObjectType* type );

  const QString descriptiveName() const;
  const QString description() const;
  const QByteArray iconFileName( const bool canBeNull = false ) const;
  void handleArgs( const std::vector<ObjectCalcer*>& os,
                           KigPart& d,
                           KigWidget& v
    ) const;
  void handlePrelim( KigPainter& p,
                             const std::vector<ObjectCalcer*>& sel,
                             const KigDocument& d,
                             const KigWidget& v
    ) const;
  // TODO: move here actual drawprelm when AbstractPolygon is in place!
  virtual void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const = 0;

  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
};

/*
 * classes PolygonBNPTypeConstructor, OpenPolygonTypeConstructor and 
 *         BezierCurveTypeConstructor
 * all inherit from the PointSequenceConstructor
 */

class PolygonBNPTypeConstructor
  : public PointSequenceConstructor
{
public:
  PolygonBNPTypeConstructor();
  ~PolygonBNPTypeConstructor();

  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const;
  int wantArgs( const std::vector<ObjectCalcer*>& os, const KigDocument& d, const KigWidget& v) const;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
};


class OpenPolygonTypeConstructor
  : public PointSequenceConstructor
{
public:
  OpenPolygonTypeConstructor();
  ~OpenPolygonTypeConstructor();

  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const;
  int wantArgs( const std::vector<ObjectCalcer*>& os, const KigDocument& d, const KigWidget& v) const;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
};

class PolygonBCVConstructor
  : public ObjectConstructor
{
  const ObjectType* mtype;
public:
  PolygonBCVConstructor();
  ~PolygonBCVConstructor();

  const QString descriptiveName() const;
  const QString description() const;
  const QByteArray iconFileName( const bool canBeNull = false ) const;
  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const;
  int wantArgs( const std::vector<ObjectCalcer*>& os,
                              const KigDocument& d,
                              const KigWidget& v
    ) const;
  void handleArgs( const std::vector<ObjectCalcer*>& os,
                           KigPart& d,
                           KigWidget& v
    ) const;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const;
  void handlePrelim( KigPainter& p,
                             const std::vector<ObjectCalcer*>& sel,
                             const KigDocument& d,
                             const KigWidget& v
    ) const;
  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
  int computeNsides( const Coordinate& c, const Coordinate& v, const Coordinate& cntrl, int& winding ) const;
  Coordinate getRotatedCoord( const Coordinate& c1,
               const Coordinate& c2, double alpha ) const;
};

class BezierCurveTypeConstructor
  : public PointSequenceConstructor
{
public:
  BezierCurveTypeConstructor();
  ~BezierCurveTypeConstructor();

  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const;
  int wantArgs( const std::vector<ObjectCalcer*>& os, const KigDocument& d, const KigWidget& v) const;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
};

class RationalBezierCurveTypeConstructor
   : public ObjectConstructor
{
public:
  RationalBezierCurveTypeConstructor();
  ~RationalBezierCurveTypeConstructor();

  const QString descriptiveName() const;
  const QString description() const;
  const QByteArray iconFileName( const bool canBeNull = false ) const;
 
  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const;

  int wantArgs( const std::vector<ObjectCalcer*>& os, const KigDocument& d, const KigWidget& v) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void handleArgs( const std::vector<ObjectCalcer*>& os,
                   KigPart& d,
                   KigWidget& v ) const;
 
 QString useText( const ObjectCalcer& o, 
                   const std::vector<ObjectCalcer*>& sel,
                   const KigDocument& d, 
                   const KigWidget& v ) const;
  QString selectStatement( const std::vector<ObjectCalcer*>& sel, 
                           const KigDocument& d,
                           const KigWidget& w ) const;


  void handlePrelim( KigPainter& p,
                     const std::vector<ObjectCalcer*>& sel,
                     const KigDocument& d,
                     const KigWidget& v ) const;
  void drawprelim( const ObjectDrawer& drawer, 
                   KigPainter& p, 
                   const std::vector<ObjectCalcer*>& parents, 
                   const KigDocument& d ) const;

 void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
};

class MeasureTransportConstructor
  : public ObjectConstructor
{
  const ObjectType* mtype;
public:
  MeasureTransportConstructor();
  ~MeasureTransportConstructor();

  const QString descriptiveName() const;
  const QString description() const;
  const QByteArray iconFileName( const bool canBeNull = false ) const;
  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os,
                              const uint& ) const;
  int wantArgs( const std::vector<ObjectCalcer*>& os,
                              const KigDocument& d,
                              const KigWidget& v
    ) const;
  void handleArgs( const std::vector<ObjectCalcer*>& os,
                           KigPart& d,
                           KigWidget& v
    ) const;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const;
  void handlePrelim( KigPainter& p,
                             const std::vector<ObjectCalcer*>& sel,
                             const KigDocument& d,
                             const KigWidget& v
    ) const;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
};

class ConicRadicalConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
  const ArgsParser mparser;
public:
  ConicRadicalConstructor();
  ~ConicRadicalConstructor();
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const;
  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );

  bool isTransform() const;
};

class LocusConstructor
  : public StandardConstructorBase
{
  ArgsParser margsparser;
public:
  LocusConstructor();
  ~LocusConstructor();
  /**
   * we override the wantArgs() function, since we need to see
   * something about the objects that an ArgsParser can't know about,
   * namely, whether the first point is a constrained point...
   */
  int wantArgs(
    const std::vector<ObjectCalcer*>& os, const KigDocument& d,
    const KigWidget& v
    ) const;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );

  bool isTransform() const;
};

class GenericAffinityConstructor
  : public MergeObjectConstructor
{
public:
  GenericAffinityConstructor();
  ~GenericAffinityConstructor();
  virtual bool isAlreadySelectedOK(const std::vector< ObjectCalcer* >& os, const uint& ) const;
};

class GenericProjectivityConstructor
  : public MergeObjectConstructor
{
public:
  GenericProjectivityConstructor();
  ~GenericProjectivityConstructor();
  virtual bool isAlreadySelectedOK(const std::vector< ObjectCalcer* >& os, const uint& ) const;
};

class InversionConstructor
  : public MergeObjectConstructor
{
public:
  InversionConstructor();
  ~InversionConstructor();
};

class GenericIntersectionConstructor
  : public MergeObjectConstructor
{
public:
  GenericIntersectionConstructor();
  ~GenericIntersectionConstructor();

  bool isIntersection() const;

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const;
};

class MidPointOfTwoPointsConstructor
  : public StandardConstructorBase
{
  ArgsParser mparser;
public:
  MidPointOfTwoPointsConstructor();
  ~MidPointOfTwoPointsConstructor();
  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d,
                 KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
};

class TestConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
public:
  TestConstructor( const ArgsParserObjectType* type, const char* descname,
    const char* desc, const char* iconfile );
  ~TestConstructor();
  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d,
                 KigWidget& w ) const;
  int wantArgs( const std::vector<ObjectCalcer*>& os,
                      const KigDocument& d, const KigWidget& v ) const;
  void plug( KigPart* doc, KigGUIAction* kact );
  bool isTransform() const;
  bool isTest() const;

  BaseConstructMode* constructMode( KigPart& doc );
};

class TangentConstructor
  : public MergeObjectConstructor
{
public:
  TangentConstructor();
  ~TangentConstructor();

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const;
//  QString selectStatement(
//    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
//    const KigWidget& w ) const;
};

class CocConstructor
  : public MergeObjectConstructor
{
public:
  CocConstructor();
  ~CocConstructor();

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const;
//  QString selectStatement(
//    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
//    const KigWidget& w ) const;
};

bool relativePrimes( int n, int p );

std::vector<ObjectCalcer*> 
removeDuplicatedPoints( std::vector<ObjectCalcer*> points );
bool coincidentPoints( const ObjectImp* p1, const ObjectImp* p2 );

#endif
