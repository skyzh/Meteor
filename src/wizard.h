#ifndef WIZARD_H
#define WIZARD_H

#include <QDialog>
#include <QCheckBox>
#include <QMap>

namespace Ui {
class Wizard;
}

class Wizard : public QDialog
{
    Q_OBJECT

public:
    explicit Wizard(QWidget *parent = nullptr);
    ~Wizard();

private slots:
    void on_pushButtonOpen_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_lineEditPath_textChanged(const QString &arg1);

private:
    Ui::Wizard *ui;
    QMap<QString, QCheckBox*> checkBoxs;
    QMap<QString, QVariant> filter;
    bool configure(QString path);
};

#endif // WIZARD_H
