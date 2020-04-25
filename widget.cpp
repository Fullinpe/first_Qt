#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
        : QWidget(parent), ui(new Ui::Widget) {

    ui->setupUi(this);
    connect(ui->mbutton, &QPushButton::clicked, this, [=]() {
                static int x = 0;
                x++;
                ui->progressBar->setValue(x);
            }
    );
}

Widget::~Widget() {
    delete ui;
}

void Widget::mslot() {
    static int x = 0;
    x++;
    ui->progressBar->setValue(x);
    ui->mb2->setHidden((x % 2) == 1);
}

