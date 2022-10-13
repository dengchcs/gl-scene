#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "renderwidget.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Form; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    RenderWidget *render_widget;
    Ui::Form *ui;
};

#endif // MAINWIDGET_H
