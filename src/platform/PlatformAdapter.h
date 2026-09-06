#pragma once
#include <QString>
class QWindow;

// Platform integration is deliberately kept out of the visual components.
namespace PlatformAdapter {
void initialize(bool layerShell);
QString attach(QWindow *window, int reservedHeight, bool layerShell);
void configureDockWindow(QWindow *window);
QString launch(const QString &application, const QString &launchId = {});
}
