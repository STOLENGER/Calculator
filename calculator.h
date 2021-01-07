#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QDialog>
#include <QLineEdit>
#include "button.h"
#include <QGridLayout>
#include <QRadioButton>
#include <QMainWindow>




class Calculator : public QMainWindow{
    Q_OBJECT
public:
    Calculator(QWidget *parent = 0);
    ~Calculator();
private slots:
    void digitClicked();
    void unaryOperatorClicked();
    void additiveOperatorClicked();
    void multiplicativeOperatorClicked();
    void trigonometryOperatorClicked();
    void constantOperatorClicked();
    void unusullyClicked();
    void extraunaryOperatorClicked();
    void equalClicked();
    void pointClicked();
    void changeSignClicked();
    void backspaceClicked();
    void clear();
    void clearAll();
    void SwitchMode();
    void  SpawnComplMode();
    void SpawnSimpleMode();
    void CreateSimpleCalcWidget();
    void CreateComplCalcWidget();
    void CreateDefaultCalcWidget();


private:
    QWidget* defaultCalcWidget;
    QWidget* simpleCalcWidget;
    QWidget* complCalcWidget;
    QWidget* mainWidget;
    QRadioButton * usually;
    QGridLayout* defaultCalcLayout;
    QGridLayout* simpleCalcLayout;
    QGridLayout* complCalcLayout;
    QGridLayout* mainLayout;

    Button *createButton(const QString &text, const char *member);
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);


    double sumSoFar;
    double factorSoFar;
    double sqrtpolMemory=1;
    double sqrtMemore=1;
    QString pendingAdditiveOperator;
    QString pendingMultiplicativeOperator;
    bool waitingForOperand;

    QLineEdit *display;

    enum { NumDigitButtons = 10 };
    Button *digitButtons[NumDigitButtons];

};

#endif // CALCULATOR_H
