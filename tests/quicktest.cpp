#include <QtQuickTest/quicktest.h>
#include <QQmlEngine>
#include "../src/SystemIcons.h"
class Setup : public QObject {
    Q_OBJECT
public slots:
    void qmlEngineAvailable(QQmlEngine *engine) {
        engine->addImageProvider("system", new SystemIcons);
    }
};
QUICK_TEST_MAIN_WITH_SETUP(xdock, Setup)
#include "quicktest.moc"
