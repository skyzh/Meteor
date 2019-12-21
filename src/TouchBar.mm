#include "mainwindow.h"

#ifdef DISABLE_TOUCHBAR
void bindTouchbar(MainWindow &window) {}
void setTouchbarSliderFlowValue(int value) {}
#else
#include <QtWidgets>
#include <QMetaObject>
#import <AppKit/AppKit.h>

static QPointer<MainWindow> mainWindow;

@interface TouchBarProvider : NSResponder <NSTouchBarDelegate, NSApplicationDelegate, NSWindowDelegate>

@property(strong) NSCustomTouchBarItem *touchBarItem1;
@property(strong) NSCustomTouchBarItem *touchBarItem2;
@property(strong) NSCustomTouchBarItem *touchBarItem3;
@property(strong) NSCustomTouchBarItem *touchBarItem4;
@property(strong) NSButton *touchBarButton1;
@property(strong) NSButton *touchBarButton2;
@property(strong) NSButton *touchBarButton3;
@property(strong) NSSlider *touchBarSliderFlow;

@property(strong) NSObject *qtDelegate;

@end

static NSTouchBarItemIdentifier ButtonTabQuery = @"com.alexchi.ButtonTabQuery";
static NSTouchBarItemIdentifier ButtonTabRoutePlanning = @"com.alexchi.ButtonTabRoutePlanning";
static NSTouchBarItemIdentifier ButtonTabFlow = @"com.alexchi.ButtonTabFlow";
static NSTouchBarItemIdentifier SliderFlow = @"com.alexchi.SliderFlow";

@implementation TouchBarProvider

- (NSTouchBar *)makeTouchBar {
    // Create the touch bar with this instance as its delegate
    NSTouchBar *bar = [[NSTouchBar alloc] init];
    bar.delegate = self;

    bar.defaultItemIdentifiers = @[
            ButtonTabQuery,
            ButtonTabRoutePlanning,
            ButtonTabFlow,
            SliderFlow];

    return bar;
}

- (NSTouchBarItem *)touchBar:(NSTouchBar *)touchBar makeItemForIdentifier:(NSTouchBarItemIdentifier)identifier {
    Q_UNUSED(touchBar);

    if ([identifier isEqualToString:ButtonTabQuery]) {
        QString title = "Traffic";
        self.touchBarItem1 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
        self.touchBarButton1 = [[NSButton buttonWithTitle:title.toNSString() target:self
                                                   action:@selector(buttonTabQueryClicked)] autorelease];
        self.touchBarItem1.view = self.touchBarButton1;
        return self.touchBarItem1;
    } else if ([identifier isEqualToString:ButtonTabRoutePlanning]) {
        QString title = "Route";
        self.touchBarItem2 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
        self.touchBarButton2 = [[NSButton buttonWithTitle:title.toNSString() target:self
                                                   action:@selector(buttonTabRoutePlanningClicked)] autorelease];
        self.touchBarItem2.view = self.touchBarButton2;
        return self.touchBarItem2;
    } else if ([identifier isEqualToString:ButtonTabFlow]) {
        QString title = "Flow";
        self.touchBarItem3 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
        self.touchBarButton3 = [[NSButton buttonWithTitle:title.toNSString() target:self
                                                   action:@selector(buttonTabFlowClicked)] autorelease];
        self.touchBarItem3.view = self.touchBarButton3;
        return self.touchBarItem3;
    } else if ([identifier isEqualToString:SliderFlow]) {
        self.touchBarItem4 = [[[NSCustomTouchBarItem alloc] initWithIdentifier:identifier] autorelease];
        self.touchBarSliderFlow = [[[NSSlider alloc] init] autorelease];
        self.touchBarSliderFlow.minValue = 0;
        self.touchBarSliderFlow.maxValue = 86400;
        self.touchBarSliderFlow.target = self;
        self.touchBarSliderFlow.action = @selector(sliderFlowChanged);
        self.touchBarItem4.view = self.touchBarSliderFlow;
        return self.touchBarItem4;
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

- (void)buttonTabFlowClicked {
    QMetaObject::invokeMethod(mainWindow, &MainWindow::tb_buttonTabFlow_clicked);
}

- (void)sliderFlowChanged {
    QMetaObject::invokeMethod(mainWindow, "tb_sliderFlow_changed",
                              Qt::QueuedConnection, Q_ARG(int, self.touchBarSliderFlow.integerValue));
}

- (void)setSliderValue:(int)value {
    self.touchBarSliderFlow.integerValue = value;
}
@end

TouchBarProvider *touchBarProvider;

void setTouchbarSliderFlowValue(int value) {
    if (touchBarProvider)
        [touchBarProvider setSliderValue:value];
}

void bindTouchbar(MainWindow &window) {
    mainWindow = &window;
    {
        NSView *view = reinterpret_cast<NSView *>(window.winId());
        touchBarProvider = [[TouchBarProvider alloc] init];
        [touchBarProvider installAsDelegateForWindow:view.window];
    }
}
#endif