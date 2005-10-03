// Copyright (C)  2005  Pino Toscano       <toscano.pino@tiscali.it>

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

#include "svgexporteroptions.h"
#include "svgexporteroptions.moc"

#include "svgexporteroptionswidget.h"

#include <qcheckbox.h>
#include <qlayout.h>

SVGExporterOptions::SVGExporterOptions( QWidget* parent )
  : QWidget( parent )
{
  expwidget = new Ui_SVGExporterOptionsWidget();
  expwidget->setupUi( this );

  layout()->setMargin( 0 );
}

SVGExporterOptions::~SVGExporterOptions()
{
}

void SVGExporterOptions::setGrid( bool grid )
{
  expwidget->showGridCheckBox->setChecked( grid );
}

bool SVGExporterOptions::showGrid() const
{
  return expwidget->showGridCheckBox->isOn();
}

void SVGExporterOptions::setAxes( bool axes )
{
  expwidget->showAxesCheckBox->setChecked( axes );
}

bool SVGExporterOptions::showAxes() const
{
  return expwidget->showAxesCheckBox->isOn();
}