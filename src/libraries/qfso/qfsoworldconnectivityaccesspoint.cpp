#include "qfsoworldconnectivityaccesspoint.h"

QFsoWorldConnectivityAccessPoint::QFsoWorldConnectivityAccessPoint()
{
}

QDBusArgument &operator<<(QDBusArgument &argument, const QFsoWorldConnectivityAccessPoint & value)
{
    argument.beginStructure();
    argument << value.description;
    argument << value.apn;
    argument << value.username;
    argument << value.password;
    argument << value.dns1;
    argument << value.dns2;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QFsoWorldConnectivityAccessPoint & value)
{
    argument.beginStructure();
    argument >> value.description;
    argument >> value.apn;
    argument >> value.username;
    argument >> value.password;
    argument >> value.dns1;
    argument >> value.dns2;
    argument.endStructure();
    return argument;
}

