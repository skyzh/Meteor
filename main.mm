#include "mainwindow.h"

#include <QtWidgets>
#include <QMetaObject>
#import <AppKit/AppKit.h>

static QPointer <MainWindow> mainWindow;

@interface TouchBarProvider : NSResponder <NSTouchBarDelegate, NSApplicationDelegate, NSWindowDelegate>

@property(strong) NSCustomTouchBarItem *touchBarItem1;
@property(strong) NSCustomTouchBarItem *touchBarItem2;
@property(strong) NSButton *touchBarButton1;
@property(strong) NSButton *touchBarButton2;

@property(strong) NSObject *qtDelegate;

@end

static NSTouchBarItemIdentifier ButtonTabQuery = @"com.alexchi.ButtonTabQuery";
static NSTouchBarItemIdentifier ButtonTabRoutePlanning = @"com.alexchi.ButtonTabRoutePlanning";

@implementation TouchBarProvider

- (NSTouchBar *)makeTouchBar {
    // Create the touch bar with this instance as its delegate
    NSTouchBar *bar = [[NSTouchBar alloc] init];
    bar.delegate = self;

    bar.defaultItemIdentifiers = @[ButtonTabQuery, ButtonTabRoutePlanning];

    return bar;
}

- (NSTouchBarItem *)touchBar:(NSTouchBar *)touchBar makeItemForIdentifier:(NSTouchBarItemIdentifier)identifier {
    Q_UNUSED(touchBar);

    if ([identifier isEqualToString:ButtonTabQuery]) {
        QString title = "Passenger Traffic";
        self.touchBarItem1 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
        self.touchBarButton1 = [[NSButton buttonWithTitle:title.toNSString() target:self
                                                   action:@selector(buttonTabQueryClicked)] autorelease];
        self.touchBarItem1.view = self.touchBarButton1;
        return self.touchBarItem1;
    } else if ([identifier isEqualToString:ButtonTabRoutePlanning]) {
        QString title = "Route Planning";
        self.touchBarItem2 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
        self.touchBarButton2 = [[NSButton buttonWithTitle:title.toNSString() target:self
                                                   action:@selector(buttonTabRoutePlanningClicked)] autorelease];
        self.touchBarItem2.view = self.touchBarButton2;
        return self.touchBarItem2;
    }
    return nil;
}

- (void)installAsDelegateForWindow:(NSWindow *)window {
    _qtDelegate = window.delegate;
    window.delegate = self;
}

- (void)installAsDelegateForApplication:(NSApplication *)application {
    _qtDelegate = application.delegate;
    application.delegate = self;
}

- (BOOL)respondsToSelector:(SEL)aSelector {
    return [_qtDelegate respondsToSelector:aSelector] || [super respondsToSelector:aSelector];
}

- (void)forwardInvocation:(NSInvocation *)anInvocation {
    [anInvocation invokeWithTarget:_qtDelegate];
}

- (void)buttonTabQueryClicked {
        QMetaObject::invokeMethod(mainWindow, &MainWindow::tb_buttonTabQuery_clicked);
}

- (void)buttonTabRoutePlanningClicked {
        QMetaObject::invokeMethod(mainWindow, &MainWindow::tb_buttonTabRoutePlanning_clicked);
}

@end

QPointer<MainWindow> run_application(int argc, char *argv[]);

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    auto window = run_application(argc, argv);
    mainWindow = window;

    {
        NSView *view = reinterpret_cast<NSView *>(window->winId());
        TouchBarProvider *touchBarProvider = [[TouchBarProvider alloc] init];
        [touchBarProvider installAsDelegateForWindow:view.window];
    }

    return app.exec();
}
