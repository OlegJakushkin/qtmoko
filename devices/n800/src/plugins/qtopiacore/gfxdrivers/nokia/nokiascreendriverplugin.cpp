/****************************************************************************
**
** Copyright (C) 2007-2008 TROLLTECH ASA. All rights reserved.
**
** This file is part of the Opensource Edition of the Qtopia Toolkit.
**
** This software is licensed under the terms of the GNU General Public
** License (GPL) version 2.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "nokiascreendriverplugin.h"
#include "nokiascreen.h"

#include <qtopiaglobal.h>
#include <qtopialog.h>

NokiaScreenDriverPlugin::NokiaScreenDriverPlugin( QObject *parent )
: QScreenDriverPlugin( parent )
{
}

NokiaScreenDriverPlugin::~NokiaScreenDriverPlugin()
{
}

QScreen* NokiaScreenDriverPlugin::create(const QString& key, int displayId)
{
    if (key.toLower() == "nokia")
        return new NokiaScreen(displayId);

    return 0;
}

QStringList NokiaScreenDriverPlugin::keys() const
{
    return QStringList() << "nokia";
}

QTOPIA_EXPORT_QT_PLUGIN(NokiaScreenDriverPlugin)

