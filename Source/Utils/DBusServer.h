#pragma once
#include <memory>

namespace sdbus
{
class IObject;
class IConnection;
}

class DBusServer {
public:
    static DBusServer *GetInstance();
    void PackageLaunchSignal(const std::string& name) const;
    void PackageStopSignal(const std::string& name) const;

    // Delete copy and move operations
    DBusServer(const DBusServer&) = delete;
    DBusServer& operator=(const DBusServer&) = delete;
    DBusServer(DBusServer&&) = delete;
    DBusServer& operator=(DBusServer&&) = delete;


private:
    std::unique_ptr<sdbus::IConnection> Connector;
    std::unique_ptr<sdbus::IObject> Object;

    DBusServer();
};
