// text_imp.cc
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

#include "text_imp.h"

#include "bogus_imp.h"
#include "../misc/kigpainter.h"

TextImp::TextImp( const QString& text, const Coordinate& loc, bool frame )
  : mtext( text), mloc( loc ), mframe( frame )
{
}

TextImp* TextImp::copy() const
{
  return new TextImp( mtext, mloc );
}

TextImp::~TextImp()
{
}

ObjectImp* TextImp::transform( const Transformation& ) const
{
  return new InvalidImp;
}

void TextImp::draw( KigPainter& p ) const
{
  mboundrect = p.simpleBoundingRect( mloc, mtext );
  p.drawTextFrame( mboundrect, mtext, mframe );
}

bool TextImp::contains( const Coordinate& p, int, const ScreenInfo& ) const
{
  return mboundrect.contains( p );
}

bool TextImp::inRect( const Rect& r, int, const ScreenInfo& ) const
{
  return mboundrect.intersects( r );
}

bool TextImp::valid() const
{
  return true;
}

const uint TextImp::numberOfProperties() const
{
  return Parent::numberOfProperties();
}

const QCStringList TextImp::propertiesInternalNames() const
{
  return Parent::propertiesInternalNames();
}

const QCStringList TextImp::properties() const
{
  return Parent::properties();
}

ObjectImp* TextImp::property( uint which, const KigDocument& w ) const
{
  return Parent::property( which, w );
}

bool TextImp::inherits( int id ) const
{
  return id == ID_TextImp ? true : Parent::inherits( id );
}

int TextImp::id() const
{
  return ID_TextImp;
}

const char* TextImp::baseName() const
{
  return "label";
}

