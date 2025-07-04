#include "DBusServer.h"

#include "sdbus-c++/IConnection.h"
#include "sdbus-c++/IObject.h"
#include "sdbus-c++/VTableItems.h"

constexpr char interfaceName[] = "dev.warden.PackageEvents";
constexpr char serviceName[] = "dev.warden.WardenObserver";
constexpr char objectPath[] = "/dev/warden/PackageEvents";

DBusServer::DBusServer()
{
    Connector = sdbus::createSystemBusConnection();
    Connector->requestName(sdbus::ServiceName{serviceName});
    Object = sdbus::createObject(*Connector, sdbus::ObjectPath{objectPath});

    Object->addVTable(
        sdbus::registerSignal("packageLaunch").withParameters<std::string>(),
        sdbus::registerSignal("packageStop").withParameters<std::string>()
        ).forInterface(interfaceName);
}

DBusServer *DBusServer::GetInstance()
{
    static DBusServer dbusServer;
    return &dbusServer;
}

void DBusServer::PackageLaunchSignal(const std::string &name) const
{
    Object->emitSignal("packageLaunch").onInterface(interfaceName).withArguments(name);
}

void DBusServer::PackageStopSignal(const std::string &name) const
{
    Object->emitSignal("packageStop").onInterface(interfaceName).withArguments(name);
}