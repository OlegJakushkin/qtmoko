/*
 * This file was generated by qofonodbusxml2cpp version 0.7
 * Command line was: qofonodbusxml2cpp -p qofonomodem ofono.xml org.ofono.Modem
 *
 * qofonodbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "qofonomodem.h"

/*
 * Implementation of interface class OrgOfonoModemInterface
 */

OrgOfonoModemInterface::OrgOfonoModemInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QOFonoDbusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

OrgOfonoModemInterface::~OrgOfonoModemInterface()
{
}

