import QtQuick
import QtTest
import "../qml"

Item {
    width: 1204; height: 120
    property var testApps: [
        {key: "launcher", name: "XLaunch", icon: "view-app-grid", launchId: ""},
        {key: "desktop", name: "显示桌面", icon: "user-desktop", launchId: ""},
        {key: "files", name: "文件管理器", icon: "system-file-manager", launchId: ""},
        {key: "browser", name: "浏览器", icon: "firefox", launchId: ""},
        {key: "music", name: "音乐", icon: "multimedia-audio-player", launchId: ""},
        {key: "video", name: "视频", icon: "multimedia-video-player", launchId: ""},
        {key: "store", name: "应用商店", icon: "system-software-install", launchId: ""},
        {key: "games", name: "游戏中心", icon: "applications-games", launchId: ""},
        {key: "screenshot", name: "截图", icon: "applets-screenshooter", launchId: ""},
        {key: "terminal", name: "终端", icon: "utilities-terminal", launchId: ""},
        {key: "settings", name: "系统设置", icon: "preferences-system", launchId: ""}
    ]
    DockBar { id: dock; width: 1204; height: 78; fixedClock: true; apps: testApps }
    TestCase {
    name: "DockInteractions"
    when: windowShown
    SignalSpy { id: activation; target: dock; signalName: "appActivated" }
    function init() {
        dock.width = 1204; dock.theme = "classic"; dock.apps = testApps; dock.focusedIndex = 0; activation.clear()
        wait(20)
    }
    function test_pointer_launch() {
        mouseClick(dock, 200, 40)
        compare(activation.count, 1)
        compare(activation.signalArguments[0][0], "files")
    }
    function test_keyboard_launch() {
        dock.forceActiveFocus()
        keyClick(Qt.Key_Right)
        keyClick(Qt.Key_Return)
        compare(activation.count, 1)
        compare(activation.signalArguments[0][0], "desktop")
    }
    function test_narrow_layout() {
        dock.width = 480
        compare(dock.visibleCount, 3)
        verify(dock.visibleCount * dock.slotWidth + dock.trayWidth <= dock.width)
        dock.width = 450
        compare(dock.visibleCount, 2)
    }
    function test_system_theme() {
        dock.theme = "system"
        verify(dock.systemTheme)
        compare(dock.visibleCount, 11)
    }
    function test_fisheye_magnification() {
        dock.pointerActive = true
        dock.pointerX = 14 + 3 * 72 + 26
        verify(dock.iconMagnification(3) > 2)
        compare(dock.iconMagnification(0), 1)
        verify(dock.iconOffset(2) < 0)
        verify(dock.iconOffset(4) > 0)
        dock.pointerActive = false
        compare(dock.iconMagnification(3), 1)
    }
    function test_dynamic_app_model() {
        dock.apps = [{key: "custom-1", name: "自定义应用", icon: "application-x-executable", launchId: "org.example.app"}]
        compare(dock.visibleCount, 1)
        dock.activate(0)
        compare(activation.count, 1)
        compare(activation.signalArguments[0][0], "custom-1")
        compare(activation.signalArguments[0][2], "org.example.app")
    }
    function test_overflow_last_item() {
        dock.width = 480
        wait(20)
        var menu = findChild(dock, "overflowMenu")
        verify(menu !== null)
        compare(menu.count, 8)
        menu.open()
        tryCompare(menu, "opened", true)
        // QuickTest input targets its main window, not a separate native popup.
        // Verify menu-to-application routing here; native interaction is checked separately.
        verify(menu.height > dock.height)
        menu.itemAt(menu.count - 1).triggered()
        tryCompare(activation, "count", 1)
        compare(activation.signalArguments[0][0], "settings")
        menu.close()
    }
    function test_invalid_activation() {
        dock.activate(-1)
        dock.activate(11)
        compare(activation.count, 0)
    }
}
}
