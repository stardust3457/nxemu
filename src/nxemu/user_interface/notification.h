#pragma once
#include <memory>
#include <nxemu-core/notification.h>

class Notification :
    public INotification
{
public:
    Notification();

    // INotification
    void DisplayError(const char * message, const char * title) const override;
    void BreakPoint(const char * fileName, uint32_t lineNumber) override;
    void AppInitDone(void) override;

    static Notification & GetInstance();
    static void CleanUp();

private:
    Notification(const Notification &) = delete;
    Notification & operator=(const Notification &) = delete;

    static std::unique_ptr<Notification> s_instance;
};