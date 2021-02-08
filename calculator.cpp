#include "calculator.h"
#include "button.h"
#include <math.h>
#include <QGridLayout>
#include <QRadioButton>
#include<QGroupBox>
#include<QInputDialog>
#include <QMainWindow>
#include <cctype>

Calculator::Calculator(QWidget *parent)
    : QMainWindow(parent)
{

    sumSoFar = 0.0;
    factorSoFar = 0.0;
    waitingForOperand = true;
    setMinimumSize(380, 450);
    setMaximumSize(380, 450);

    usually= new QRadioButton(tr("Обычный"));
    usually->setChecked(true);
    connect(usually, SIGNAL(toggled(bool)), SLOT(SwitchMode()));

    CreateSimpleCalcWidget();
    CreateComplCalcWidget();
    CreateDefaultCalcWidget();
    mainWidget = new QWidget(this);
    SpawnSimpleMode();

    setWindowTitle(tr("Calculator"));
}

Calculator::~Calculator()
{

}

void Calculator::digitClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    int digitValue = clickedButton->text().toInt();
    if (display->text() == "0" && digitValue == 0.0)
        return;

    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    display->setText(display->text() + QString::number(digitValue));
}

void Calculator::unaryOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if(clickedButton==nullptr)
        return;
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("\u221A")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result = sqrt(operand);
    } else if (clickedOperator == tr("x\u00B2")) {
        result = pow(operand, 2.0);
    } else if (clickedOperator == tr("x\263")) {
        result = pow(operand, 3.0);
    } else
        if (clickedOperator == tr("1/x")) {
            if (operand == 0.0) {
                abortOperation();
                return;
            }
            result = 1.0 / operand;
        }
    display->setText(QString::number(result));
    waitingForOperand = true;
}

void Calculator::additiveOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if(clickedButton==nullptr)
        return;
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }

    pendingAdditiveOperator = clickedOperator;
    waitingForOperand = true;
}

void Calculator::multiplicativeOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if(clickedButton==nullptr)
        return;
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
    } else {
        factorSoFar = operand;
    }

    pendingMultiplicativeOperator = clickedOperator;
    waitingForOperand = true;
}

void Calculator::trigonometryOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if(clickedButton==nullptr)
        return;
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("sin")) {
        result=sin(operand);
    }else if(clickedOperator == tr("cos"))
    {
        result=cos(operand);
    }else if(clickedOperator == tr("tag"))
    {
        result=tan(operand);
    }else  if (clickedOperator == tr("sinh")) {
        result=sinh(operand);
    }else if(clickedOperator == tr("cosh"))
    {
        result=cosh(operand);
    }else if(clickedOperator == tr("tagh"))
    {
        result=tanh(operand);
    }
    waitingForOperand = true;
    display->setText(QString::number(result));
}

void Calculator::constantOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if(clickedButton==nullptr)
        return;
    QString clickedOperator = clickedButton->text();
    if (clickedOperator ==tr("exp")) {
        display->setText( QString::number(exp(1)));
    }else
        if (clickedOperator == tr("pi")) {
            display->setText(QString::number(M_PI));
        }
    waitingForOperand = true;
}

int factorial(int n) {
    if (n < 0) {
        return 0;
    }
    return !n ? 1 : n * factorial(n - 1);
}

void Calculator::unusullyClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if(clickedButton==nullptr)
        return;
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("n!")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        if(operand-int(operand)!=0)
        {
            abortOperation();
            return;
        }
        result=factorial(operand);
    }else if (clickedOperator == tr("ln")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }

        result=log(operand);
    }else if (clickedOperator == tr("log")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result=log(operand);

    }


    display->setText(QString::number(result));
    waitingForOperand = true;


}

void Calculator::extraunaryOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if(clickedButton==nullptr)
        return;
    QString clickedOperator = clickedButton->text();

    double operand = display->text().toDouble();

    double result = 0.0;

    if (clickedOperator == tr("x\u00B3")) {
        result = pow(operand, 3.0);
    } else{
        if (clickedOperator == tr("x\u207F n")) {
            sqrtMemore=operand;
            display->clear();
        }
    } if (clickedOperator == tr("x^0.5y")) {
        sqrtpolMemory=operand;
        display->clear();
    }

    display->setText(QString::number(result));
    waitingForOperand = true;
}



void Calculator::equalClicked()
{
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }
    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        pendingAdditiveOperator.clear();
    } else {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));
    if(sqrtMemore!=1)
    {
        sumSoFar=pow(operand,sqrtMemore);
        sqrtMemore=1;
    }
    if(sqrtpolMemory!=1)
    {
        sumSoFar=pow(operand, sqrtpolMemory/2);
        sqrtpolMemory=1;
    }
    display->setText(QString::number(sumSoFar));
    sumSoFar = 0.0;
    waitingForOperand = true;
}

void Calculator::pointClicked()
{
    if (waitingForOperand)
        display->setText("0");
    if (!display->text().contains("."))
        display->setText(display->text() + tr("."));
    waitingForOperand = false;
}

void Calculator::changeSignClicked()
{
    QString text = display->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    display->setText(text);
}

void Calculator::backspaceClicked()
{
    if (waitingForOperand)
        return;

    QString text = display->text();
    text.chop(1);
    if (text.isEmpty()) {
        text = "0";
        waitingForOperand = true;
    }
    display->setText(text);
}

void Calculator::clear()
{
    if (waitingForOperand)
        return;

    display->setText("0");
    waitingForOperand = true;
}

void Calculator::clearAll()
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    pendingAdditiveOperator.clear();
    pendingMultiplicativeOperator.clear();
    display->setText("0");
    waitingForOperand = true;
}

void Calculator::SwitchMode()
{
    if (!usually->isChecked()) {
        delete mainLayout;
        setMinimumSize(685, 450);
        setMaximumSize(685, 450);
        SpawnComplMode();
    } else {
        delete mainLayout;
        setMinimumSize(380, 450);
        setMaximumSize(380, 450);
        SpawnSimpleMode();
    }
}

void Calculator::SpawnComplMode()
{
    mainLayout = new QGridLayout;
    mainLayout->setSpacing(0);
    mainLayout->addWidget(defaultCalcWidget, 0, 0, 1, 9);
    mainLayout->addWidget(complCalcWidget,   1, 0, 2, 4);
    mainLayout->addWidget(simpleCalcWidget,  1, 4, 2, 5);
    complCalcWidget->show();

    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);
    centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
}

void Calculator::SpawnSimpleMode()
{
    mainLayout = new QGridLayout;
    mainLayout->setSpacing(0);
    complCalcWidget->hide();
    mainLayout->addWidget(defaultCalcWidget, 0, 0, 1, 3);
    mainLayout->addWidget(simpleCalcWidget,  1, 0, 2, 3);

    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
    centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
}

void Calculator::CreateSimpleCalcWidget()
{

    for (int i = 0; i < NumDigitButtons; ++i) {
        digitButtons[i] = createButton(QString::number(i), SLOT(digitClicked()));
    }

    Button *pointButton = createButton(tr("."), SLOT(pointClicked()));

    Button *divisionButton = createButton(tr("\u00F7"), SLOT(multiplicativeOperatorClicked()));//:
    Button *timesButton = createButton(tr("\u00D7"), SLOT(multiplicativeOperatorClicked()));//*
    Button *minusButton = createButton(tr("-"), SLOT(additiveOperatorClicked()));
    Button *plusButton = createButton(tr("+"), SLOT(additiveOperatorClicked()));

    Button *squareRootButton = createButton(tr("\u221A"), SLOT(unaryOperatorClicked()));
    Button *reciprocalButton = createButton(tr("1/x"), SLOT(unaryOperatorClicked()));
    Button *equalButton = createButton(tr("="), SLOT(equalClicked()));

    simpleCalcLayout = new QGridLayout(this);
    simpleCalcLayout->setSpacing(0);
    simpleCalcLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 1; i < NumDigitButtons; ++i) {
        int row = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;
        simpleCalcLayout->addWidget(digitButtons[i], row, column);
    }

    simpleCalcLayout->addWidget(digitButtons[0], 5, 1,1,2);
    simpleCalcLayout->addWidget(pointButton, 5, 3);

    simpleCalcLayout->addWidget(divisionButton, 2, 4);
    simpleCalcLayout->addWidget(timesButton, 3, 4);
    simpleCalcLayout->addWidget(minusButton, 4, 4);
    simpleCalcLayout->addWidget(plusButton, 5, 4);

    simpleCalcLayout->addWidget(squareRootButton, 2, 5);
    simpleCalcLayout->addWidget(reciprocalButton, 3, 5);
    simpleCalcLayout->addWidget(equalButton, 4, 5,2,1);

    simpleCalcWidget = new QWidget(this);
    simpleCalcWidget->setLayout(simpleCalcLayout);

}

void Calculator::CreateComplCalcWidget()
{
    Button *squareButton = createButton(tr("x\u00B2"), SLOT(unaryOperatorClicked()));//^2
    Button *cubeButton = createButton(tr("x\u00B3"), SLOT(extraunaryOperatorClicked()));//^3
    Button *degreeButton = createButton(tr("x\u207F"), SLOT(extraunaryOperatorClicked()));//x^y
    Button *sqrdegreeButton = createButton(tr("x^0.5y"),SLOT(extraunaryOperatorClicked()));
    Button *expButton = createButton(tr("exp"),  SLOT(constantOperatorClicked()));
    Button *piButton = createButton(tr("pi"),  SLOT(constantOperatorClicked()));
    Button *lnButton = createButton(tr("ln"),  SLOT(unusullyClicked()));
    Button *logButton = createButton(tr("log"),  SLOT(digitClicked()));
    Button *cubesqlButton = createButton(tr("\u00B3\u221A"),  SLOT(multiplicativeOperatorClicked()));
    Button *sinButton = createButton(tr("sin"),  SLOT(trigonometryOperatorClicked()));
    Button *cosButton = createButton(tr("cos"),  SLOT(trigonometryOperatorClicked()));
    Button *tanButton = createButton(tr("tan"),  SLOT(trigonometryOperatorClicked()));
    Button *sinhButton = createButton(tr("sinh"),  SLOT(trigonometryOperatorClicked()));
    Button *coshButton = createButton(tr("cosh"),  SLOT(trigonometryOperatorClicked()));
    Button *tanhButton = createButton(tr("tanh"),  SLOT(trigonometryOperatorClicked()));
    Button *factButton = createButton(tr("n!"),  SLOT(unusullyClicked()));


    complCalcLayout = new QGridLayout(this);
    complCalcLayout->setSpacing(0);
    complCalcLayout->setContentsMargins(0, 0, 0, 0);

    complCalcLayout->addWidget(sinhButton, 2, 0);
    complCalcLayout->addWidget(coshButton, 3, 0);
    complCalcLayout->addWidget(tanhButton, 4, 0);
    complCalcLayout->addWidget(factButton, 5, 0);
    complCalcLayout->addWidget(sinButton, 2, 1);
    complCalcLayout->addWidget(cosButton, 3, 1);
    complCalcLayout->addWidget(tanButton, 4, 1);
    complCalcLayout->addWidget(piButton, 5, 1);
    complCalcLayout->addWidget(expButton, 2, 2);
    complCalcLayout->addWidget(lnButton, 3, 2);
    complCalcLayout->addWidget(logButton, 4, 2);
    complCalcLayout->addWidget(cubesqlButton, 5, 2);
    complCalcLayout->addWidget(squareButton, 2, 3);
    complCalcLayout->addWidget(cubeButton, 3, 3);
    complCalcLayout->addWidget(degreeButton, 4, 3);
    complCalcLayout->addWidget(sqrdegreeButton, 5, 3);

    complCalcWidget = new QWidget(this);
    complCalcWidget->setLayout(complCalcLayout);
}

void Calculator::CreateDefaultCalcWidget()
{
    Button *clearButton = createButton(tr("C"), SLOT(clearAll()));
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);


    QFont font = display->font();
    font.setPointSize(font.pointSize() + 10);
    display->setFont(font);

    QRadioButton * ingener= new QRadioButton(tr("Инженерный"));

    defaultCalcLayout = new QGridLayout(this);
    defaultCalcLayout->setSpacing(0);
    defaultCalcLayout->setContentsMargins(0, 0, 0, 0);

    defaultCalcLayout->addWidget(display,  0, 0, 2, 5);
    defaultCalcLayout->addWidget(clearButton,   2, 4, 2, 1);
    defaultCalcLayout->addWidget(ingener,3, 0, 1, 3);
    defaultCalcLayout->addWidget(usually,2, 0, 1, 3);


    defaultCalcWidget = new QWidget(this);
    defaultCalcWidget->setLayout(defaultCalcLayout);


}


Button *Calculator::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

void Calculator::abortOperation()
{
    clearAll();
    display->setText(tr("####"));
}

bool Calculator::calculate(double rightOperand, const QString &pendingOperator)
{
    if (pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if (pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    } else if (pendingOperator == tr("\u00D7")) {
        factorSoFar *= rightOperand;
    } else if (pendingOperator == tr("\u00F7")) {
        if (rightOperand == 0.0)
            return false;
        factorSoFar /= rightOperand;
    }
    return true;
}
