#pragma once
#include <QQuickImageProvider>
#include <QApplication>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QIcon>
#include <QPainter>
#include <QSettings>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QStyle>
#include <QMap>
#include <memory>

// Appearance and application branding are independent: both dock themes use
// host icons, and the reference screenshot is never an icon fallback.
class SystemIcons final : public QQuickImageProvider {
public:
    SystemIcons() : QQuickImageProvider(QQuickImageProvider::Image) {}
    QImage requestImage(const QString &id, QSize *size, const QSize &requested) override {
        const auto parts = id.split('/');
        const QString name = parts.value(0);
        QIcon icon;
#ifdef Q_OS_MACOS
        const QMap<QString, QString> bundles = {
            {"view-app-grid", "/System/Applications/Launchpad.app"},
            {"user-desktop", "/System/Library/CoreServices/Finder.app"},
            {"system-file-manager", "/System/Library/CoreServices/Finder.app"},
            {"firefox", "/Applications/Safari.app"},
            {"multimedia-audio-player", "/System/Applications/Music.app"},
            {"multimedia-video-player", "/System/Applications/QuickTime Player.app"},
            {"system-software-install", "/System/Applications/App Store.app"},
            {"applications-games", "/System/Applications/Games.app"},
            {"applets-screenshooter", "/System/Applications/Utilities/Screenshot.app"},
            {"utilities-terminal", "/System/Applications/Utilities/Terminal.app"},
            {"preferences-system", "/System/Applications/System Settings.app"}
        };
        const auto path = bundles.value(name);
        if (parts.size() == 1 && !path.isEmpty() && QFileInfo::exists(path) && qobject_cast<QApplication *>(qApp))
            icon = QFileIconProvider().icon(QFileInfo(path));
#endif
#ifdef Q_OS_LINUX
        const QMap<QString, QStringList> desktopIds = {
            {"view-app-grid", {"xlaunch", "dde-launcher"}},
            {"system-file-manager", {"dde-file-manager", "org.gnome.Nautilus", "org.kde.dolphin", "thunar"}},
            {"firefox", {"firefox", "org.mozilla.firefox", "chromium"}},
            {"multimedia-audio-player", {"deepin-music", "org.gnome.Music", "rhythmbox"}},
            {"multimedia-video-player", {"deepin-movie", "vlc", "org.gnome.Totem"}},
            {"system-software-install", {"deepin-app-store", "org.gnome.Software", "org.kde.discover"}},
            {"applications-games", {"deepin-game-center", "steam"}},
            {"applets-screenshooter", {"deepin-screen-recorder", "org.kde.spectacle", "org.gnome.Screenshot"}},
            {"utilities-terminal", {"deepin-terminal", "org.gnome.Terminal", "org.kde.konsole", "xfce4-terminal"}},
            {"preferences-system", {"dde-control-center", "org.gnome.Settings", "systemsettings"}}
        };
        for (const auto &desktopId : (parts.size() == 1 ? desktopIds.value(name) : QStringList{})) {
            const auto path = QStandardPaths::locate(QStandardPaths::ApplicationsLocation, desktopId + ".desktop");
            if (path.isEmpty()) continue;
            QSettings desktop(path, QSettings::IniFormat);
            const auto iconName = desktop.value("Desktop Entry/Icon").toString();
            icon = QFileInfo(iconName).isAbsolute() ? QIcon(iconName) : QIcon::fromTheme(iconName);
            break;
        }
#endif
        if (icon.isNull()) icon = QIcon::fromTheme(name);
        if (icon.isNull()) {
            const QMap<QString, QStyle::StandardPixmap> defaults = {
                {"view-app-grid", QStyle::SP_ComputerIcon},
                {"user-desktop", QStyle::SP_DesktopIcon},
                {"system-file-manager", QStyle::SP_DirHomeIcon},
                {"firefox", QStyle::SP_DriveNetIcon},
                {"multimedia-audio-player", QStyle::SP_MediaVolume},
                {"multimedia-video-player", QStyle::SP_MediaPlay},
                {"system-software-install", QStyle::SP_DialogSaveButton},
                {"preferences-system", QStyle::SP_FileDialogDetailedView},
                {"audio-volume-high", QStyle::SP_MediaVolume},
                {"dialog-information", QStyle::SP_MessageBoxInformation},
                {"input-keyboard", QStyle::SP_ComputerIcon},
                {"avatar-default", QStyle::SP_DirHomeIcon}
            };
            // QuickTest uses QGuiApplication; its fallback style is test-local.
            std::unique_ptr<QStyle> fallback;
            QStyle *style = nullptr;
            if (qobject_cast<QApplication *>(qApp)) style = QApplication::style();
            else { fallback.reset(QStyleFactory::create("Fusion")); style = fallback.get(); }
            if (style) icon = style->standardIcon(defaults.value(name, QStyle::SP_FileIcon));
        }
        const QSize dimensions = requested.isValid() ? requested : QSize(52, 52);
        QImage result(dimensions, QImage::Format_ARGB32_Premultiplied);
        result.fill(Qt::transparent);
        QPainter painter(&result);
        icon.paint(&painter, result.rect(), Qt::AlignCenter);
        if (parts.size() > 1) {
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(result.rect(), QColor("#" + parts[1]));
        }
        if (size) *size = dimensions;
        return result;
    }
};
