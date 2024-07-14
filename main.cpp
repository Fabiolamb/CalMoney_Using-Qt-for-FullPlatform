#include "widget.h"

#include <QApplication>

// #ifdef Q_OS_ANDROID
// // https://www.qt.io/blog/qt-6.2-for-android
// #include <QJniObject>
// #include <QCoreApplication>
// #include <QCoreApplication>
// #include <QJniObject>
// #include <QGuiApplication>

// void setAndroidKeyboardResize()
// {   // QNativeInterface::QAndroidApplication::runOnAndroidMainThread
//     QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=]() {
//         QJniObject activity = QNativeInterface::QAndroidApplication::context();
//         if (activity.isValid()) {
//             activity.callObjectMethod("getWindow", "()Landroid/view/Window;").callMethod<void>("setSoftInputMode", "(I)V", 0x10);  // 0x10 is the value for adjustResize
//         }
//     });
// }
// #endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

// #ifdef Q_OS_ANDROID
//     setAndroidKeyboardResize();
// #endif

    Widget w;
    w.show();
    return a.exec();
}
