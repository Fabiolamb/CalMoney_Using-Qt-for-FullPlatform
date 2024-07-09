# Cal-money_GUI_Std-path

## 部件版本

- Qt 6.6.3
- C++ 17
- Android
  - JDK Version: 17.1.0
  - SDK Version: 13.0
  - NDK Version: 25.1.8

## 代码

- ### QMake

  ```cmake
  QT       += core gui
  
  greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
  
  CONFIG += c++17
  # 可以通过定义QT_DISABLE_DEPRECATED_BEFORE来使代码中使用的所有在Qt 6.0.0之前被弃用的API导致编译失败。取消注释这行代码可以实现这个功能。
  # You can make your code fail to compile if it uses deprecated APIs.
  # In order to do so, uncomment the following line.
  #DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
  
  SOURCES += \
      main.cpp \
      widget.cpp
  
  HEADERS += \
      widget.h
  # 这部分代码定义了默认的部署规则。对于QNX操作系统，目标路径是/tmp/$${TARGET}/bin；对于非Android的Unix系统，目标路径是/opt/$${TARGET}/bin。如果目标路径不为空，则将目标添加到安装规则中。
  # Default rules for deployment.
  qnx: target.path = /tmp/$${TARGET}/bin
  else: unix:!android: target.path = /opt/$${TARGET}/bin
  !isEmpty(target.path): INSTALLS += target
  
  #这部分代码定义了针对Android平台的特定设置。如果目标架构包含arm64-v8a，则设置ANDROID_PACKAGE_SOURCE_DIR为当前目录下的android文件夹
  contains(ANDROID_TARGET_ARCH,arm64-v8a) {
      ANDROID_PACKAGE_SOURCE_DIR = \
          $$PWD/android
  }
  ```

- ### Widget.h

  ```c++
  #ifndef WIDGET_H    // 包含保护（include guard），防止头文件被多次包含导致重定义错误。
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
  ```

- ### Widget.cpp

  ```c++
  #include "widget.h"
  
  #include <QCalendarWidget>
  #include <QCheckBox>
  #include <QComboBox>
  #include <QDateEdit>
  #include <QGridLayout>
  #include <QGroupBox>
  #include <QLabel>
  #include <QLocale>
  #include <QTextCharFormat>
  #include <QPlainTextEdit>
  #include <QPushButton>
  #include <QFile>
  #include <QFileDialog>
  #include <QTextStream>
  #include <QSpinBox>
  #include <QSettings>
  #include <QStringList>
  #include <QTime>
  
  #include <QTextEdit>
  #include <QVBoxLayout>
  #include <QFileDialog>
  #include <QMessageBox>
  
  #include <QStandardPaths>
  
  //#include <QDialog>
  
  Widget::Widget(QWidget *parent)
      : QWidget(parent)
  {
      createPreviewGroupBox();
      createDatesGroupBox();
      createRightInfoBar();
  
      QGridLayout *layout = new QGridLayout;
  
      // 0, 0 是网格布局中的行和列坐标。具体来说：0 是行号，表示这一项被放置在网格的第一行。0 是列号，表示这一项被放置在网格的第一列。
      layout->addWidget(previewGroupBox, 0, 0);
      layout->addWidget(datesGroupBox, 1, 0);
      layout->addWidget(rightGroupBox, 2, 0);
      //layout->addLayout(layout, 0, 0);
      layout->setSizeConstraint(QLayout::SetFixedSize);
      setLayout(layout);
  
      // 设置预览布局的最小行高和列宽，以及窗口标题
      previewLayout->setRowMinimumHeight(0, calendar->sizeHint().height());
      previewLayout->setColumnMinimumWidth(0, calendar->sizeHint().width());
  
      setWindowTitle(tr("Calendar Widget"));
  }
  
  // 槽函数，响应日期选择更改，将当前选定日期设置为 currentDateEdit 的日期
  inline void Widget::selectedDateChanged()
  {
      currentDateEdit->setDate(calendar->selectedDate());
  }
  
  // 槽函数，响应最大日期更改，将日历的最大日期设置为新日期。
  inline void Widget::maximumDateChanged(QDate date)
  {
      calendar->setMaximumDate(date);
  }
  
  // 创建预览组框，包含一个日历部件，并设置日历的最小和最大日期，以及网格可见性。将日历部件添加到预览布局，并将布局设置为预览组框的布局。
  inline void Widget::createPreviewGroupBox()
  {
      previewGroupBox = new QGroupBox(tr("Calendar"));
  
      calendar = new QCalendarWidget;
      calendar->setMinimumDate(QDate(1900, 1, 1));
      calendar->setMaximumDate(QDate(3000, 1, 1));
      calendar->setGridVisible(true);
  
      previewLayout = new QGridLayout;
      previewLayout->addWidget(calendar, 0, 0, Qt::AlignCenter);
      previewGroupBox->setLayout(previewLayout);
  }
  
  inline void Widget::createDatesGroupBox()
  {
      // 创建日期组框，包含两个日期编辑器（currentDateEdit 和 maximumDateEdit）及其标签（currentDateLabel 和 maximumDateLabel），并设置它们的显示格式、日期范围和初始日期。
      datesGroupBox = new QGroupBox(tr("Dates"));
  
      currentDateEdit = new QDateEdit;
      currentDateEdit->setDisplayFormat("MMM d yyyy");
      currentDateEdit->setDate(calendar->selectedDate());
      currentDateEdit->setDateRange(calendar->minimumDate(),
                                    calendar->maximumDate());
  
      currentDateLabel = new QLabel(tr("&Current Date:"));
      currentDateLabel->setBuddy(currentDateEdit);
  
      maximumDateEdit = new QDateEdit;
      maximumDateEdit->setDisplayFormat("MMM d yyyy");
      maximumDateEdit->setDateRange(calendar->minimumDate(),
                                    calendar->maximumDate());
      maximumDateEdit->setDate(calendar->maximumDate());
  
      maximumDateLabel = new QLabel(tr("Ma&ximum Date:"));
      maximumDateLabel->setBuddy(maximumDateEdit);
  
      // 连接信号和槽，将日期编辑器的日期更改信号连接到日历和 Widget 的相应槽函数。创建日期组框的网格布局，将组件添加到布局中，并设置布局。
      connect(currentDateEdit, &QDateEdit::dateChanged, calendar, &QCalendarWidget::setSelectedDate);
      connect(calendar, &QCalendarWidget::selectionChanged, this, &Widget::selectedDateChanged);
  
      connect(maximumDateEdit, &QDateEdit::dateChanged, this, &Widget::maximumDateChanged);
  
      QGridLayout *dateBoxLayout = new QGridLayout;
      dateBoxLayout->addWidget(currentDateLabel, 1, 0);
      dateBoxLayout->addWidget(currentDateEdit, 1, 1);
  
      datesGroupBox->setLayout(dateBoxLayout);
  }
  
  void Widget::createRightInfoBar()
  {
      // 创建右侧信息栏组框，包含一个标签，用于显示当前选定的日期。连接日历的选择更改信号到一个槽函数，该槽函数更新标签文本。
      rightGroupBox = new QGroupBox;
  
      QLabel *label = new QLabel(calendar->selectedDate().toString(Qt::ISODate));
      label->setAlignment(Qt::AlignCenter);
      connect(calendar, &QCalendarWidget::selectionChanged, this, [=]() {
          label->setText("Selected Date : " + currentDateEdit->date().toString(Qt::ISODate));
      });
  
      // 创建两个用于设置时间的 QSpinBox（小时和分钟），并连接它们的值更改信号到调试输出槽函数。
      QSpinBox *hourSpinBox = new QSpinBox;
      hourSpinBox->setRange(0, 23); // 小时的范围是 0 到 23
      hourSpinBox->setValue(QTime::currentTime().hour()); // 设置初始小时为当前小时
      connect(hourSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
          //Q_UNUSED(value); // 避免未使用参数的警告
          qDebug() << "First Hour SpinBox value changed : " << value;
      });
  
      // 创建分钟输入框
      QSpinBox *minuteSpinBox = new QSpinBox;
      minuteSpinBox->setRange(0, 59); // 分钟的范围是 0 到 59
      minuteSpinBox->setValue(QTime::currentTime().minute()); // 设置初始分钟为当前分钟
      connect(minuteSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
          //Q_UNUSED(value); // 避免未使用参数的警告
          qDebug() << "First Minute SpinBox value changed : " << value;
      });
  
      QSpinBox *hourSpinBox2 = new QSpinBox;
      hourSpinBox2->setRange(0, 23); // 小时的范围是 0 到 23
      hourSpinBox2->setValue(QTime::currentTime().hour()); // 设置初始小时为当前小时
      connect(hourSpinBox2, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
          //Q_UNUSED(value); // 避免未使用参数的警告
          qDebug() << "Second Hour SpinBox value changed : " << value;
      });
  
      // 创建分钟输入框
      QSpinBox *minuteSpinBox2 = new QSpinBox;
      minuteSpinBox2->setRange(0, 59); // 分钟的范围是 0 到 59
      minuteSpinBox2->setValue(QTime::currentTime().minute()); // 设置初始分钟为当前分钟
      connect(minuteSpinBox2, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
          //Q_UNUSED(value); // 避免未使用参数的警告
          qDebug() << "Second Minute SpinBox value changed : " << value;
      });
  
      // 创建日志输入框
      QPlainTextEdit *logTextEdit = new QPlainTextEdit;
      logTextEdit->setPlaceholderText(tr("write here..."));
      logTextEdit->setMaximumHeight(125);
  
      // new method
      // 在构造函数或合适的位置初始化保存路径
      QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
      QDir().mkpath(appDataPath);  // 确保目录存在
  
  
      QPushButton *saveButton = new QPushButton(tr("&Save"));
      saveButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
  
      connect(saveButton, &QPushButton::clicked, this, [=]() {
          QString logText = logTextEdit->toPlainText();
  
          QSettings settings("MyCompany", "MyApp");
          QString savedDirectory = settings.value("FolderPath").toString();
          QString selectedDirectory;
  
          qDebug() << !savedDirectory.isEmpty() << " && " << QFile((savedDirectory + "/log.txt")).exists();
          qDebug() << savedDirectory + "/log.txt";
  
          if (!savedDirectory.isEmpty() && QFile((savedDirectory + "/log.txt")).exists()) {
              selectedDirectory = savedDirectory;
              qDebug() << "yes! " << savedDirectory << ' ' << savedDirectory + "/log.txt";
          } else {
              selectedDirectory = appDataPath;
              qDebug() << "Using app data path: " << appDataPath;
          }
  
          if (!selectedDirectory.isEmpty()) {
              filename = selectedDirectory + "/log.txt";
              qDebug() << "Selected folder: " << selectedDirectory;
  
              settings.setValue("FolderPath", selectedDirectory);
          } else {
              filename = appDataPath + "/log.txt";
              qDebug() << "No folder selected, using default: " << filename;
          }
  
          settings.setValue("Filename", filename);
          QFile file(filename);
          if (!file.exists()) {
              qDebug() << "File does not exist, creating new file: " << filename;
              if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                  qDebug() << "Unable to create new file: " << file.errorString();
                  return;
              }
              file.close();
          }
  
          if (!file.open(QIODevice::Append | QIODevice::Text)) {
              qDebug() << "Unable to open file for writing: " << file.errorString();
              return;
          }
  
          QTextStream out(&file);
          out << currentDateEdit->date().toString(Qt::ISODate) << ' '
              << hourSpinBox->value() << '-' << minuteSpinBox->value() << ' '
              << hourSpinBox2->value() << '-' << minuteSpinBox2->value() << ' '
              << " : " << '\n'
              << logText << "\n\n";
          file.close();
          qDebug() << "Log saved to file: " << filename;
  
          qDebug() << "Saving log: " << logText;
          logTextEdit->clear();
      });
  
      QPushButton *calbutton = new QPushButton(tr("%1Calculate!"));
      calbutton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_1));
      connect(calbutton, &QPushButton::clicked, this, [=]() {
          qDebug() << filename;
          if (filename.isEmpty()) {
              QSettings settings("MyCompany", "MyApp");
              filename = settings.value("Filename").toString();
              qDebug() << "Using filename: " << filename;
          }
          QFile file(filename);
          if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
              qDebug() << "Unable to open file for reading: " << file.errorString();
              return;
          }
  
          QTextStream in(&file);
          QString fileContent = in.readAll();
          file.close();
  
          qDebug() << "File content: " << fileContent;
  
          QStringList sl = fileContent.split("\n\n", Qt::SkipEmptyParts);
  
          double sum = 0;
  
          for (const QString &_ssl : sl) {
              QStringList parts = _ssl.split(" ", Qt::SkipEmptyParts);
  
              if (parts.size() <= 4)  continue;
  
              QString string_date = parts.value(0);
              QDate date = QDate::fromString(string_date, "yyyy-MM-dd");
  
              QString beginTime = parts.value(1);
              QStringList bbengintime = beginTime.split("-");
              QTime time1(bbengintime[0].toInt(), bbengintime[1].toInt());
  
              QString endTime = parts.value(2);
              QStringList eendintime = endTime.split("-");
              QTime time2(eendintime[0].toInt(), eendintime[1].toInt());
  
              double timediff = time1.secsTo(time2) / 60 / 60.;
  
              if (date.dayOfWeek() == Qt::Sunday || date.dayOfWeek() == Qt::Saturday) {
                  sum = sum + timediff * 20.;
              } else {
                  sum = sum + timediff * 17.;
              }
  
              qDebug() << "Date:" << string_date << "QDate:" << date.dayOfWeek() << ", Begin Time:" << beginTime << ", End Time:" << endTime;
              qDebug() << "Timediff:" << timediff;
              qDebug() << "Money:" << sum;
          }
  
          QFile filesum(filename);
          if (!filesum.open(QIODevice::Append | QIODevice::Text)) {
              qDebug() << "Unable to open file for writing: " << file.errorString();
              return;
          }
  
          QTextStream out2(&filesum);
          out2 << "sum = " << sum << "\n\n";
          filesum.close();
      });
  
      QPushButton *cleanButton = new QPushButton(tr("Clean"));
  
      connect(cleanButton, &QPushButton::clicked, this, [=]() {
          qDebug() << "Clearing file contents";
          if (filename.isEmpty()) {
              QSettings settings("MyCompany", "MyApp");
              filename = settings.value("Filename").toString();
              qDebug() << "Using filename: " << filename;
          }
          QFile file(filename);
          if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
              qDebug() << "Unable to open file for clearing: " << file.errorString();
              return;
          }
          file.close();
          qDebug() << "File contents cleared: " << filename;
      });
  
      QPushButton *viewButton = new QPushButton(tr("View File"));
      QLabel *fileContentLabel = new QLabel(this);
      fileContentLabel->setWordWrap(true);  // 允许自动换行
      fileContentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);  // 允许文本选择
      connect(viewButton, &QPushButton::clicked, this, [=]() {
          qDebug() << "Viewing file contents";
          if (filename.isEmpty()) {
              QSettings settings("MyCompany", "MyApp");
              filename = settings.value("Filename").toString();
              qDebug() << "Using filename: " << filename;
          }
          QFile file(filename);
          if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
              qDebug() << "Unable to open file for reading: " << file.errorString();
              return;
          }
  
          QTextStream in(&file);
          QString fileContent = in.readAll();
          file.close();
  
          QPushButton *close = new QPushButton(tr("Close"));
  
          // 注意这里花了比较长的时间，用Widget是可以的，Dialog不行
          // 创建新窗口来显示文件内容
          //QDialog *viewDialog = new QDialog(this);
          QWidget *viewDialog = new QWidget();
          viewDialog->setWindowTitle(tr("File Contents"));
          QVBoxLayout *layout = new QVBoxLayout(viewDialog);
          QTextEdit *textEdit = new QTextEdit(viewDialog);
          textEdit->setPlainText(fileContent);
          textEdit->setReadOnly(true);
          layout->addWidget(textEdit);
          layout->addWidget(close);
          viewDialog->setLayout(layout);
          viewDialog->resize(400, 300);
          viewDialog->show();
  
          connect(close, &QPushButton::clicked, this, [=]() {
              viewDialog->close();
          });
      });
  
      // 将标签和日志输入框添加到右侧信息栏布局中
      rightLayout = new QGridLayout;
      rightLayout->addWidget(label);
  
      QGridLayout *spinboxLayout = new QGridLayout;
      QGroupBox *spinboxGroupbox = new QGroupBox(tr("Time Record"));
      spinboxLayout->addWidget(hourSpinBox, 0, 0);
      spinboxLayout->addWidget(minuteSpinBox, 0, 1);
      spinboxLayout->addWidget(hourSpinBox2, 1, 0);
      spinboxLayout->addWidget(minuteSpinBox2, 1, 1);
      spinboxGroupbox->setLayout(spinboxLayout);
      rightLayout->addWidget(spinboxGroupbox);
      rightLayout->addWidget(logTextEdit);
      rightLayout->addWidget(saveButton);
      rightLayout->addWidget(calbutton);
      rightLayout->addWidget(cleanButton);
      rightLayout->addWidget(viewButton);
  
      rightGroupBox->setLayout(rightLayout);
  }
  ```

- ### main.cpp

  ```c++
  #include "widget.h"
  
  #include <QApplication>
  
  int main(int argc, char *argv[])
  {
      QApplication a(argc, argv);
      Widget w;
      w.show();
      return a.exec();
  }
  ```



## Qt数据持久化方法

- ###### 描述：一个Qt应用程序，它可以在电脑端执行，也可以在安卓端执行，作用是记录工作日期和时间，这些数据使用Qt的什么方法持久化存放最合适

  在Qt应用程序中，如果你需要记录工作日期和时间，并且希望数据能够在电脑端和安卓端都能持久化存放，以下方法是比较合适的：

  1. **SQLite数据库**：

     - Qt提供了对SQLite的内置支持，使用`QSqlDatabase`和相关类可以很方便地操作SQLite数据库。
     - SQLite是一个轻量级的嵌入式数据库，适用于大多数存储需求。

     ```C++
     #include <QtSql/QSqlDatabase>
     #include <QtSql/QSqlQuery>
     #include <QtSql/QSqlError>
     #include <QDebug>
     
     // 打开数据库连接
     QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
     db.setDatabaseName("work_times.db");
     
     if (!db.open()) {
         qDebug() << "Error: connection with database failed";
     } else {
         qDebug() << "Database: connection ok";
     }
     
     // 创建表格
     QSqlQuery query;
     query.exec("CREATE TABLE IF NOT EXISTS work_times (id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT, time TEXT)");
     
     // 插入数据
     query.prepare("INSERT INTO work_times (date, time) VALUES (:date, :time)");
     query.bindValue(":date", "2023-07-08");
     query.bindValue(":time", "08:00");
     query.exec();
     ```

  2. **JSON文件**：

     - 使用Qt的`QJsonDocument`、`QJsonObject`和`QJsonArray`类可以方便地将数据存储为JSON格式，并将其写入文件或从文件读取。
     - JSON文件适用于结构化但不是特别复杂的数据存储需求。

     ```c++
     #include <QJsonDocument>
     #include <QJsonObject>
     #include <QJsonArray>
     #include <QFile>
     #include <QIODevice>
     
     QJsonObject record;
     record["date"] = "2023-07-08";
     record["time"] = "08:00";
     
     QJsonArray recordsArray;
     recordsArray.append(record);
     
     QJsonObject mainObj;
     mainObj["work_times"] = recordsArray;
     
     QJsonDocument doc(mainObj);
     
     QFile file("work_times.json");
     if (!file.open(QIODevice::WriteOnly)) {
         qWarning("Couldn't open save file.");
         return;
     }
     
     file.write(doc.toJson());
     ```

  3. **QSettings**：

     - 如果数据量较小，且不需要复杂的查询操作，可以使用`QSettings`，它提供了一个简单的方法来存储和检索应用程序设置和少量数据。
     - `QSettings`在不同平台上自动选择合适的存储位置（如Windows注册表、INI文件、Android SharedPreferences等）。

     ```c++
     #include <QSettings>
     
     QSettings settings("MyCompany", "MyApp");
     
     // 写入数据
     settings.beginGroup("WorkTimes");
     settings.setValue("2023-07-08", "08:00");
     settings.endGroup();
     
     // 读取数据
     settings.beginGroup("WorkTimes");
     QString time = settings.value("2023-07-08").toString();
     settings.endGroup();
     ```

  以上三种方法各有优劣，选择哪种方式取决于你的具体需求：

  - **SQLite**适合需要复杂查询和大数据量存储的情况。
  - **JSON文件**适合需要可读性和便于手工编辑的小型数据存储。
  - **QSettings**适合存储少量配置信息和简单的数据。

  根据你的需求和应用程序的复杂度，选择合适的方法进行持久化存储。

- ###### **描述：这些方法在安卓上各自需要申请什么权限**

  在Android平台上，不同的存储方法需要申请不同的权限。以下是针对SQLite数据库、JSON文件和QSettings的权限需求：

  ### SQLite数据库

  SQLite数据库在应用的私有存储区域中使用，不需要特殊权限，因为每个应用都有自己专属的数据库存储空间。这些存储空间在应用卸载时会被自动清理。

  ### JSON文件

  如果JSON文件存储在应用的私有存储区域（例如`/data/data/<package_name>/files`），则不需要特殊权限。但是，如果你希望将JSON文件存储在公共存储（例如`/sdcard`）上，则需要申请存储权限。

  #### 所需权限

  ```xml
  <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
  <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
  ```

  ### QSettings

  `QSettings`在Android上使用SharedPreferences进行存储，不需要特殊权限，因为SharedPreferences存储在应用的私有存储区域。

  ### 权限配置示例

  如果你的应用使用的是私有存储区域（推荐），则通常不需要在`AndroidManifest.xml`中申请权限。不过，为了全面性，如果涉及公共存储区域，可以参考以下权限配置：

  ```xml
  <manifest xmlns:android="http://schemas.android.com/apk/res/android"
      package="com.example.myapp">
  
      <application
          android:allowBackup="true"
          android:icon="@mipmap/ic_launcher"
          android:label="@string/app_name"
          android:roundIcon="@mipmap/ic_launcher_round"
          android:supportsRtl="true"
          android:theme="@style/AppTheme">
          <activity android:name=".MainActivity">
              <intent-filter>
                  <action android:name="android.intent.action.MAIN" />
                  <category android:name="android.intent.category.LAUNCHER" />
              </intent-filter>
          </activity>
      </application>
  
      <!-- 如果需要写入公共存储，请添加以下权限 -->
      <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
      <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
  
  </manifest>
  ```

  ### 在运行时请求权限

  从Android 6.0（API级别23）开始，需要在运行时请求权限：

  ```java
  // 在Activity中请求权限
  if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
          != PackageManager.PERMISSION_GRANTED) {
      // 权限未被授予
      ActivityCompat.requestPermissions(this,
              new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
              MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE);
  }
  
  // 处理权限请求结果
  @Override
  public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
      switch (requestCode) {
          case MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE: {
              if (grantResults.length > 0
                      && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                  // 权限授予
              } else {
                  // 权限拒绝
              }
              return;
          }
      }
  }
  ```

  总结：

  - **SQLite数据库**和**QSettings**（使用SharedPreferences）不需要特殊权限。
  - **JSON文件**存储在应用的私有存储区域不需要权限，但如果存储在公共存储区域，需要`WRITE_EXTERNAL_STORAGE`和`READ_EXTERNAL_STORAGE`权限。