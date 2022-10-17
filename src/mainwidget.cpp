#include "mainwidget.h"
#include "ui_form.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::Form)
{
    ui->setupUi(this);
    render_widget = new RenderWidget(this);
    render_widget->setObjectName("MainWidget");
    render_widget->setGeometry(QRect(0, 0, 700, 700));
}

MainWidget::~MainWidget()
{
    delete ui;
}
