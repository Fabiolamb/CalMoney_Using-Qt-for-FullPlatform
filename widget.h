#ifndef WIDGET_H    // 这两行是包含保护（include guard），防止头文件被多次包含导致重定义错误。
#define WIDGET_H

#include <QWidget>
#include <QDateTime>

// 这些前向声明（forward declarations）告诉编译器这些类的名字和存在，而不需要包括这些类的完整定义。这样可以加快编译速度。
QT_BEGIN_NAMESPACE
class QCalendarWidget;
class QCheckBox;
class QComboBox;
class QDate;
class QDateEdit;
class QGridLayout;
class QGroupBox;
class QLabel;
QT_END_NAMESPACE

class Widget : public QWidget
{
    // 定义一个名为 Widget 的类，并声明它继承自 QWidget。 Q_OBJECT 宏是所有使用Qt元对象系统的类所必需的，它支持信号和槽机制。
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    //~Widget();
private slots:
    void selectedDateChanged();             // 在所选日期更改时调用
    void maximumDateChanged(QDate date);    // 在最大日期更改时调用

private:
    // 用于创建不同的用户界面部分
    void createPreviewGroupBox();
    void createDatesGroupBox();
    void createRightInfoBar();

    QGroupBox *rightGroupBox;
    QGridLayout *rightLayout;

    QGroupBox *previewGroupBox;
    QGridLayout *previewLayout;
    QCalendarWidget *calendar;

    QGroupBox *datesGroupBox;
    QLabel *currentDateLabel;
    QLabel *maximumDateLabel;
    QDateEdit *currentDateEdit;
    QDateEdit *maximumDateEdit;

    QString filename;
};
#endif // WIDGET_H
