#include "TouchBar.h"
#include "mainwindow.h"
#include "wizard.h"
#include "ConfigManager.h"
#include "Meteor.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ConfigManager::instance();

    QSurfaceFormat fmt;
    fmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(fmt);

    Wizard wizard;
    MainWindow window;
    bindTouchbar(window);

    wizard.open();

    app.connect(&wizard, &Wizard::rejected, [&wizard] {
        QMessageBox::information(&wizard, "Info", "No file selected");
    });

    app.connect(&wizard, &Wizard::accepted, [&window] {
        window.show();
        QMetaObject::invokeMethod(&window, "meteor_wizard_complete");
    });

    return app.exec();
}
