#pragma once
#include <QObject>
#include <QSettings>
#include <QVariantList>
class DockBackend : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool preview READ preview CONSTANT)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QString dockEdge READ dockEdge WRITE setDockEdge NOTIFY dockEdgeChanged)
    Q_PROPERTY(QVariantList apps READ apps NOTIFY appsChanged)
    Q_PROPERTY(QString platformStatus READ platformStatus NOTIFY platformStatusChanged)
public:
    explicit DockBackend(bool preview, QObject *parent = nullptr);
    bool preview() const { return m_preview; }
    QString theme() const { return m_theme; }
    QString dockEdge() const { return m_dockEdge; }
    QVariantList apps() const { return m_apps; }
    QString platformStatus() const { return m_platformStatus; }
    void setTheme(const QString &theme);
    void setDockEdge(const QString &edge);
    void setPlatformStatus(const QString &status);
    Q_INVOKABLE void launch(const QString &key, const QString &name, const QString &launchId = {});
    Q_INVOKABLE bool addPinnedApp(const QString &name, const QString &launchId, const QString &iconName);
    Q_INVOKABLE bool removePinnedApp(int index);
    Q_INVOKABLE void resetPinnedApps();
signals:
    void themeChanged();
    void dockEdgeChanged();
    void appsChanged();
    void platformStatusChanged();
    void notice(const QString &message);
private:
    bool m_preview;
    QString m_theme;
    QString m_dockEdge;
    QVariantList m_apps;
    QString m_platformStatus;
    QSettings m_settings;
    void saveApps();
};
