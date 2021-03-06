// Copyright (C)  2003  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#ifndef KIG_OBJECTS_POLYGON_TYPE_H
#define KIG_OBJECTS_POLYGON_TYPE_H

#include "base_type.h"

/**
 * Triangle by its vertices
 */
class TriangleB3PType
  : public ArgsParserObjectType
{
  TriangleB3PType();
  ~TriangleB3PType();
public:
  static const TriangleB3PType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;
};

/**
 * Polygon by its vertices
 */
class PolygonBNPType
  : public ObjectType
{
  PolygonBNPType();
  ~PolygonBNPType();
public:
  static const PolygonBNPType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const;
  Args sortArgs( const Args& args ) const;

  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;
};

/**
 * Open Polygon (Polyline, Polygonal Line)
 */
class OpenPolygonType
  : public ObjectType
{
  OpenPolygonType();
  ~OpenPolygonType();
public:
  static const OpenPolygonType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const;
  Args sortArgs( const Args& args ) const;

  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;
};

/**
 * Polygon by center and vertex
 */
class PolygonBCVType
  : public ObjectType
{
  PolygonBCVType();
  ~PolygonBCVType();
public:
  static const PolygonBCVType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const;
  Args sortArgs( const Args& args ) const;
  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;
};

int polygonlineintersection( const std::vector<Coordinate>& ppoints, 
      const Coordinate a, const Coordinate b,
      double& t1, double& t2,
      std::vector<Coordinate>::const_iterator& intersectionside );

int polygonlineintersection( const std::vector<Coordinate>& ppoints, 
      const Coordinate a, const Coordinate b,
      bool boundleft, bool boundright, bool inside, 
      bool openpolygon, double& t1, double& t2,
      std::vector<Coordinate>::const_iterator& intersectionside );

class PolygonLineIntersectionType
  : public ArgsParserObjectType
{
  PolygonLineIntersectionType();
protected:
  PolygonLineIntersectionType( const char fulltypename[],
                        const struct ArgsParser::spec argsspec[],
                        int n );
  ~PolygonLineIntersectionType();
public:
  static const PolygonLineIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class OPolygonalLineIntersectionType
  : public PolygonLineIntersectionType
{
  OPolygonalLineIntersectionType();
  ~OPolygonalLineIntersectionType();
public:
  static const OPolygonalLineIntersectionType* instance();
  const ObjectImpType* resultId() const;
};

class CPolygonalLineIntersectionType
  : public PolygonLineIntersectionType
{
  CPolygonalLineIntersectionType();
  ~CPolygonalLineIntersectionType();
public:
  static const CPolygonalLineIntersectionType* instance();
  const ObjectImpType* resultId() const;
};

class PolygonPolygonIntersectionType
  : public ArgsParserObjectType
{
  PolygonPolygonIntersectionType();
  ~PolygonPolygonIntersectionType();
public:
  static const PolygonPolygonIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class PolygonVertexType
  : public ArgsParserObjectType
{
  PolygonVertexType();
  ~PolygonVertexType();
public:
  static const PolygonVertexType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class PolygonSideType
  : public ArgsParserObjectType
{
  PolygonSideType();
  ~PolygonSideType();
public:
  static const PolygonSideType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConvexHullType
  : public ArgsParserObjectType
{
  ConvexHullType();
  ~ConvexHullType();
public:
  static const ConvexHullType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
