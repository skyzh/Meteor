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
}

Wizard::~Wizard() {
    delete ui;
}

#define DEVELOPMENT

void Wizard::on_pushButtonOpen_clicked() {
#ifndef DEVELOPMENT
    QString path = QFileDialog::getExistingDirectory(
            this,
            "Select dataset folder");
#else
    QString path = "/Users/skyzh/Work/Qt/dataset_CS241";
    ui->checkBox->setChecked(true);
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
                             "Real-world mapping not found, "
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
    }
    checkBoxs.clear();
    QList<QString> required_fields = {
            "time", "lineID", "stationID", "status", "userID"
    };
    for (auto &&field : fields) {
        QCheckBox *checkBox = new QCheckBox(field, this);
        checkBox->setChecked(true);
        if (required_fields.indexOf(field) != -1) {
            checkBox->setEnabled(false);
        }
        ui->verticalLayoutField->addWidget(checkBox);
    }
    return true;
}

void Wizard::on_checkBox_stateChanged(int arg1) {
    ConfigManager::instance()->set("PERSIST", ui->checkBox->isChecked());
}

void Wizard::on_lineEditPath_textChanged(const QString &arg1) {
    ConfigManager::instance()->set("DATASET_PATH", ui->lineEditPath->text());
}
