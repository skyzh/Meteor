#include "wizard.h"
#include "ui_wizard.h"
#include "ConfigManager.h"

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QtAlgorithms>
#include <QDebug>

Wizard::Wizard(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Wizard) {
    ui->setupUi(this);
    ConfigManager::instance()->set("PERSIST", false);
    ConfigManager::instance()->set("FILTER", filter);
}

Wizard::~Wizard() {
    delete ui;
}

void Wizard::on_pushButtonOpen_clicked() {
#ifndef DEVELOPMENT
    QString path = QFileDialog::getExistingDirectory(
            this,
            "Select dataset folder");
#else
    QString path = "/Users/skyzh/Work/Qt/dataset_CS241";
    // ui->checkBox->setChecked(true);
#endif
    ui->buttonBox->setEnabled(configure(path));
}

bool Wizard::configure(QString path) {
    if (path == "") return false;
    ui->lineEditPath->setText(path);
    QDir directory(path);
    if (!directory.exists("dataset")) {
        QMessageBox::warning(this, "Warning", "Dataset folder not found!");
        return false;
    }
    if (!directory.exists("adjacency_adjacency/Metro_roadMap.csv")) {
        QMessageBox::warning(this, "Warning", "Metro roadmap not found!");
        return false;
    }
    if (!directory.exists("adjacency_adjacency/station_line.csv")) {
        QMessageBox::warning(this, "Warning",
                             "Real-world mapping (station_line.csv) not found, "
                             "please follow the instructions provided in README.md.");
        return false;
    }
    QDir dataset(directory.absoluteFilePath("dataset"));
    auto entries = dataset.entryList(QStringList() << "*.csv" << "*.CSV", QDir::Files);
    if (entries.empty()) {
        QMessageBox::warning(this, "Warning",
                             "No dataset CSV found.");
        return false;
    }
    QFile file(dataset.absoluteFilePath(entries[0]));
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Warning",
                             "Failed to open file.");
        return false;
    }
    QTextStream in(&file);
    auto header = in.readLine().trimmed();
    auto fields = header.split(",");
    for (auto &&key : checkBoxs.keys()) {

        ui->verticalLayoutField->removeWidget(checkBoxs[key]);
        delete checkBoxs[key];
    }
    checkBoxs.clear();
    filter.clear();
    QList<QString> required_fields = {
            "time", "lineID", "stationID", "status", "userID"
    };
    for (auto &&field : fields) {
        auto checkBox = new QCheckBox(field, this);
        checkBox->setChecked(true);
        if (required_fields.indexOf(field) != -1) {
            checkBox->setEnabled(false);
        }
        filter[field] = true;
        ui->verticalLayoutField->addWidget(checkBox);
        checkBoxs[field] = checkBox;
        connect(checkBox, &QCheckBox::stateChanged, [=](int) {
            filter[field] = checkBox->isChecked();
            ConfigManager::instance()->set("FILTER", filter);
        });
    }
    ConfigManager::instance()->set("FILTER", filter);
    return true;
}

void Wizard::on_checkBox_stateChanged(int arg1) {
    ConfigManager::instance()->set("PERSIST", ui->checkBox->isChecked());
    ui->labelHint->setText(ui->checkBox->isChecked()
                           ? "A new file called \"alex_chi_persistence_dataset.db\" will be created in that folder. You may remove it later."
                           : "You'll lose all cached data after restarting the program.");
}

void Wizard::on_lineEditPath_textChanged(const QString &arg1) {
    ConfigManager::instance()->set("DATASET_PATH", ui->lineEditPath->text());
}
