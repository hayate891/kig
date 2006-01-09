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

#include "latexexporteroptions.h"
#include "latexexporteroptions.moc"

#include "latexexporteroptionswidget.h"

#include <qcheckbox.h>
#include <qlayout.h>

LatexExporterOptions::LatexExporterOptions( QWidget* parent )
  : QWidget( parent )
{
  expwidget = new Ui_LatexExporterOptionsWidget();
  expwidget->setupUi( this );

  layout()->setMargin( 0 );
}

LatexExporterOptions::~LatexExporterOptions()
{
}

void LatexExporterOptions::setGrid( bool grid )
{
  expwidget->showGridCheckBox->setChecked( grid );
}

bool LatexExporterOptions::showGrid() const
{
  return expwidget->showGridCheckBox->isChecked();
}

void LatexExporterOptions::setAxes( bool axes )
{
  expwidget->showAxesCheckBox->setChecked( axes );
}

bool LatexExporterOptions::showAxes() const
{
  return expwidget->showAxesCheckBox->isChecked();
}

void LatexExporterOptions::setExtraFrame( bool frame )
{
  expwidget->showFrameCheckBox->setChecked( frame );
}

bool LatexExporterOptions::showExtraFrame() const
{
  return expwidget->showFrameCheckBox->isChecked();
}