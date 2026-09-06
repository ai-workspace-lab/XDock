#include "PlatformAdapter.h"
#include <QWindow>
#import <AppKit/AppKit.h>

void PlatformAdapter::configureDockWindow(QWindow *window)
{
    if (!window) return;
    // QWindow::winId() is the Cocoa content view on macOS. Configure its
    // owning panel so activating another application cannot hide the Dock.
    auto *view = reinterpret_cast<NSView *>(window->winId());
    NSWindow *nativeWindow = [view window];
    if (!nativeWindow) return;
    [nativeWindow setLevel:NSStatusWindowLevel];
    [nativeWindow setHidesOnDeactivate:NO];
    [nativeWindow setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces
                                        | NSWindowCollectionBehaviorStationary
                                        | NSWindowCollectionBehaviorFullScreenAuxiliary];
    [nativeWindow setReleasedWhenClosed:NO];
}
