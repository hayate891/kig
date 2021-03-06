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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "kig.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

#include <KPluginLoader>
#include <KAboutData>
#include <KCrash>

#include "aboutdata.h"
#include <klocalizedstring.h>

static int convertToNative( const QUrl &file, const QByteArray& outfile )
{
  KPluginLoader libraryLoader( "kigpart" );
  QLibrary library( libraryLoader.fileName() );
  int ( *converterfunction )( const QUrl&, const QByteArray& );
  converterfunction = ( int ( * )( const QUrl&, const QByteArray& ) ) library.resolve( "convertToNative" );
  if ( !converterfunction )
  {
    qCritical() << "Error: broken Kig installation: different library and application version !" << endl;
    return -1;
  }
  return (*converterfunction)( file, outfile );
}

int main(int argc, char **argv)
{
  QApplication app( argc, argv );
  KLocalizedString::setApplicationDomain("kig");
  KAboutData about = kigAboutData( "kig", I18N_NOOP( "Kig" ) );
  KCrash::initialize();
  QCommandLineParser parser;
  QCommandLineOption convertToNativeOption( QStringList() << "c" << "convert-to-native", i18n( "Do not show a GUI. Convert the specified file to the native Kig format. Output goes to stdout unless --outfile is specified." ) );
  QCommandLineOption outfileOption( QStringList() << "o" << "outfile", i18n( "File to output the created native file to. '-' means output to stdout. Default is stdout as well." ), "file" );

  QCoreApplication::setApplicationName( "kig" );
  QCoreApplication::setApplicationVersion( i18n( KIGVERSION ) );
  QCoreApplication::setOrganizationDomain( i18n( "kde.org" ) );
  KAboutData::setApplicationData( about );

  about.setupCommandLine( &parser );
  parser.addHelpOption();
  parser.addOption( convertToNativeOption );
  parser.addOption( outfileOption );
  parser.addPositionalArgument( "URL", i18n( "Document to open" ) );
  parser.process( app );
  about.processCommandLine( &parser );

  QStringList urls = parser.positionalArguments();

  if ( parser.isSet( "convert-to-native" ) )
  {
    QString outfile = parser.value( "outfile" );
    if ( outfile.isNull() )
      outfile = "-";
    if ( urls.isEmpty() )
    {
      qCritical() << "Error: --convert-to-native specified without a file to convert." << endl;
      return -1;
    }
    if ( urls.count() > 1 )
    {
      qCritical() << "Error: --convert-to-native specified with more than one file to convert." << endl;
      return -1;
    }
    return convertToNative( QUrl::fromLocalFile( urls[0] ), outfile.toLocal8Bit() );
  }
  else
  {
    if ( parser.isSet( "outfile" ) )
    {
      qCritical() << "Error: --outfile specified without convert-to-native." << endl;
      return -1;
    }

    // see if we are starting with session management
    if ( app.isSessionRestored() )
    {
      RESTORE( Kig )
    }

    Kig *widget = new Kig;
    widget->show();

    if( urls.count() > 0 )
    {
      widget->load( QUrl::fromLocalFile( urls[ 0 ] ) );
    }

    return app.exec();
  }
}
