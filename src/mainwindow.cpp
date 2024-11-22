//
// Created by gms on 24-11-18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include "flowlayout.h"
#include <QDir>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("紫薯码字");
    checkAndCreateBackupFolder();
    loadBooks();
    setupUI();
    
    // 添加窗口居中显示的代码
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建一个堆叠布局来管理主区域和设置页面
    QStackedWidget *stackedWidget = new QStackedWidget(this);
    
    setupSideBar();
    setupMainArea();
    setupSettingsPage();
    
    stackedWidget->addWidget(mainArea);
    stackedWidget->addWidget(settingsPage);
    
    mainLayout->addWidget(sideBar);
    mainLayout->addWidget(stackedWidget);
    
    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 5);
    
    setCentralWidget(centralWidget);
    
    // 设置全局样式
    setStyleSheet(R"(
        QLabel {
            font-family: "Microsoft YaHei";
        }
        QPushButton {
            background-color: #6c5ce7;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 6px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #5b4bc4;
        }
        QListWidget {
            border: none;
            background-color: transparent;
            outline: none;
        }
        QListWidget::item {
            padding: 12px 16px;
            margin: 4px 8px;
            border-radius: 8px;
        }
        QListWidget::item:selected {
            background-color: #f0f0ff;
            color: #6c5ce7;
            font-weight: bold;
        }
        QListWidget::item:hover:!selected {
            background-color: #f8f8ff;
        }
    )");
}

void MainWindow::setupSideBar()
{
    sideBar = new QWidget(this);
    sideBar->setFixedWidth(240);
    sideBar->setStyleSheet(R"(
        QWidget {
            background-color: #ffffff;
            border-right: 1px solid #eef2f7;
        }
    )");
    
    QVBoxLayout *sideBarLayout = new QVBoxLayout(sideBar);
    sideBarLayout->setSpacing(24);
    sideBarLayout->setContentsMargins(0, 40, 0, 40);
    
    // 用户信息区域
    QWidget *userInfoWidget = new QWidget(sideBar);
    QVBoxLayout *userInfoLayout = new QVBoxLayout(userInfoWidget);
    userInfoLayout->setSpacing(12);
    
    userAvatar = new QLabel(this);
    userAvatar->setFixedSize(80, 80);
    userAvatar->setStyleSheet(R"(
        background-color: #a29bfe;
        border-radius: 40px;
        border: 3px solid #eef2f7;
    )");
    
    userName = new QLabel("紫薯", this);
    userName->setAlignment(Qt::AlignCenter);
    userName->setStyleSheet("font-size: 18px; font-weight: bold; color: #2d3436; margin-top: 8px;");
    
    userInfoLayout->addWidget(userAvatar, 0, Qt::AlignCenter);
    userInfoLayout->addWidget(userName, 0, Qt::AlignCenter);
    
    // 功能列表
    functionList = new QListWidget(this);
    functionList->setStyleSheet(R"(
        QListWidget::item {
            color: #2d3436;
            font-size: 15px;
            text-align: center;
        }
        QListWidget::item:selected {
            color: #6c5ce7;
            background-color: #f0f0ff;
        }
    )");
    
    // 添加功能列表项
    auto addListItem = [this](const QString &text, const QString &iconPath) {
        QListWidgetItem *item = new QListWidgetItem(text);
        item->setTextAlignment(Qt::AlignCenter);
        item->setSizeHint(QSize(0, 45));
        functionList->addItem(item);
        
        // 如果是"书籍列表"项，则默认选中
        if (text == "书籍列表") {
            functionList->setCurrentItem(item);
        }
    };
    
    addListItem("书籍列表", ":/icons/books.png");
    addListItem("码字统计", ":/icons/stats.png");
    addListItem("插件箱", ":/icons/plugins.png");
    addListItem("排行榜", ":/icons/rank.png");
    
    // 添加列表项点击事件处理
    connect(functionList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item->text() == "书籍列表") {
            switchToMainPage();
            functionList->setCurrentItem(item);  // 保持选中状态
        }
        // 其他功能项的处理可以后续添加
    });
    
    // 添加设置按钮
    QPushButton *settingsBtn = new QPushButton("设置", this);
    settingsBtn->setObjectName("settingsBtn");  // 添加对象名以便应用样式
    settingsBtn->setCheckable(true);  // 使按钮可切换状态
    settingsBtn->setStyleSheet(R"(
        QPushButton#settingsBtn {
            background-color: transparent;
            color: #666666;
            border: none;
            padding: 12px 16px;
            font-size: 15px;
            text-align: center;
            border-radius: 8px;
        }
        QPushButton#settingsBtn:hover {
            background-color: #f8f8ff;
            color: #6c5ce7;
        }
        QPushButton#settingsBtn:checked {
            background-color: #f0f0ff;
            color: #6c5ce7;
            font-weight: bold;
        }
    )");
    
    // 添加点击事件
    connect(settingsBtn, &QPushButton::clicked, this, [this, settingsBtn]() {
        switchToSettingsPage();
        settingsBtn->setChecked(true);  // 设置为选中状态
    });
    
    // 保存设置按钮的指针以便在其他地方使用
    this->settingsBtn = settingsBtn;  // 需要在头文件中添加成员变量
    
    sideBarLayout->addWidget(userInfoWidget);
    sideBarLayout->addSpacing(20);
    sideBarLayout->addWidget(functionList);
    sideBarLayout->addStretch();
    sideBarLayout->addWidget(settingsBtn, 0, Qt::AlignHCenter);
}

void MainWindow::setupMainArea()
{
    mainArea = new QWidget(this);
    mainArea->setStyleSheet("background-color: #f8f9fc;");
    
    QVBoxLayout *mainAreaLayout = new QVBoxLayout(mainArea);
    mainAreaLayout->setSpacing(20);
    mainAreaLayout->setContentsMargins(30, 30, 0, 30);
    
    // 顶部按钮区
    QWidget *topBar = new QWidget(this);
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(0, 0, 0, 0);
    
    syncLibraryBtn = new QPushButton("同步书稿", this);
    newBookBtn = new QPushButton("新建书稿", this);
    
    syncLibraryBtn->setIcon(QIcon(":/icons/sync.png"));
    newBookBtn->setIcon(QIcon(":/icons/new.png"));
    
    // 修改按钮样式
    QString btnStyle = R"(
        QPushButton {
            padding: 8px 20px;
            font-size: 14px;
            font-weight: 500;
            min-width: 100px;
        }
        QPushButton#syncLibraryBtn {
            background-color: #ffffff;
            color: #6c5ce7;
            border: 2px solid #6c5ce7;
        }
        QPushButton#syncLibraryBtn:hover {
            background-color: #f0f0ff;
        }
        QPushButton#newBookBtn {
            background-color: #6c5ce7;
            color: white;
            border: none;
        }
        QPushButton#newBookBtn:hover {
            background-color: #5b4bc4;
        }
    )";
    
    syncLibraryBtn->setObjectName("syncLibraryBtn");  // 设置对象名以便应用样式
    newBookBtn->setObjectName("newBookBtn");          // 设置对象名以便应用样式
    
    syncLibraryBtn->setStyleSheet(btnStyle);
    newBookBtn->setStyleSheet(btnStyle);
    
    topBarLayout->addWidget(syncLibraryBtn);
    topBarLayout->addSpacing(12);
    topBarLayout->addWidget(newBookBtn);
    topBarLayout->addStretch();
    
    // 书籍容器
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // 修改滚动区域样式，移除右边距
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            border: none;
            background: transparent;
            width: 8px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #d0d0d0;
            border-radius: 4px;
            min-height: 60px;
        }
        QScrollBar::handle:vertical:hover {
            background: #a8a8a8;
        }
        QScrollBar::add-line:vertical {
            height: 0px;
            background: none;
            border: none;
        }
        QScrollBar::sub-line:vertical {
            height: 0px;
            background: none;
            border: none;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
            border: none;
        }
    )");
    
    QWidget *booksWidget = new QWidget(scrollArea);
    booksWidget->setStyleSheet("QWidget { background: transparent; }");
    
    // 使用 FlowLayout，移除右边距
    FlowLayout *flowLayout = new FlowLayout(booksWidget, 0, 16, 16);
    flowLayout->setContentsMargins(0, 0, 0, 0);  // 移除所有边距
    
    scrollArea->setWidget(booksWidget);
    
    mainAreaLayout->addWidget(topBar);
    mainAreaLayout->addWidget(scrollArea);
    
    // 显示书籍列表
    if (bookList.isEmpty()) {
        // 如果没有书籍，显示空状态
        QLabel *emptyLabel = new QLabel("还没有书籍，点击\"新建书稿\"开始创作", booksWidget);
        emptyLabel->setStyleSheet(R"(
            QLabel {
                color: #666666;
                font-size: 16px;
                padding: 40px;
            }
        )");
        emptyLabel->setAlignment(Qt::AlignCenter);
        flowLayout->addWidget(emptyLabel);
    } else {
        // 显示所有书籍
        for (const QString &bookName : bookList) {
            QFrame *card = createBookCard(bookName, "目前字数0");  // 这里可以添加获取实际字数的逻辑
            flowLayout->addWidget(card);
        }
    }
}

QFrame* MainWindow::createBookCard(const QString &title, const QString &wordCount)
{
    QFrame *card = new QFrame(this);
    card->setFixedSize(180, 220);
    card->setStyleSheet(R"(
        QFrame {
            background-color: #ffffff;
            border-radius: 10px;
            border: 1px solid #eef2f7;
            margin: 1px;  /* 为阴影效果预留空间 */
        }
        QFrame:hover {
            border: 2px solid #6c5ce7;
            background-color: #ffffff;
            margin: 0px;  /* 通过改变边距来拟上浮效果 */
        }
    )");
    
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(12);
    cardLayout->setContentsMargins(16, 20, 16, 20);
    
    // 书籍图标
    QLabel *iconLabel = new QLabel(card);
    iconLabel->setFixedSize(70, 70);
    iconLabel->setStyleSheet(QString(R"(
        background-color: #a29bfe;
        border-radius: 35px;
        color: white;
        font-size: 28px;
        font-weight: bold;
    )"));
    // 修复字符转换问题
    QString firstChar = title.isEmpty() ? QString("书") : QString(title.at(0));
    iconLabel->setText(firstChar);
    iconLabel->setAlignment(Qt::AlignCenter);
    
    // 标题
    QLabel *titleLabel = new QLabel(title, card);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 16px; color: #2d3436; font-weight: bold; margin-top: 6px;");
    
    // 字数统计
    QLabel *countLabel = new QLabel(wordCount, card);
    countLabel->setAlignment(Qt::AlignCenter);
    countLabel->setStyleSheet("color: #636e72; font-size: 13px; margin-top: 2px;");
    
    cardLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(countLabel);
    cardLayout->addStretch();
    
    return card;
}

// 添加以下函数到 MainWindow 类中，以实现窗口拖动功能
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 获取标题栏高度（通常是系统标题栏的高度）
    const int titleBarHeight = 30;  // 可以根据实际需要调整这个值
    
    // 只有在标题栏区域的点击才允许拖动
    if (event->button() == Qt::LeftButton && event->position().y() <= titleBarHeight) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // 只有在标题栏区域开始的拖动才处理移动事件
    const int titleBarHeight = 30;  // 保持与 mousePressEvent 中的值一致
    
    if (event->buttons() & Qt::LeftButton && event->position().y() <= titleBarHeight) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

bool MainWindow::checkAndCreateBackupFolder()
{
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    backupFolderPath = documentsPath + "/码字草稿备份";
    
    QDir dir(backupFolderPath);
    if (!dir.exists()) {
        if (dir.mkpath(backupFolderPath)) {
            qDebug() << "成功创建备份文件夹：" << backupFolderPath;
            return true;
        } else {
            qDebug() << "创建备份文件夹失败：" << backupFolderPath;
            return false;
        }
    }
    qDebug() << "备份文件夹已存在：" << backupFolderPath;
    return true;
}

void MainWindow::loadBooks()
{
    QDir dir(backupFolderPath);
    if (!dir.exists()) {
        qDebug() << "备份文件夹不存在：" << backupFolderPath;
        return;
    }

    // 设置过滤器只显示文件夹
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList bookDirs = dir.entryInfoList();

    // 清空现有书籍列表
    bookList.clear();

    // 获取所有子文件夹名称（即名称）
    for (const QFileInfo &fileInfo : bookDirs) {
        bookList.append(fileInfo.fileName());
    }

    qDebug() << "找到" << bookList.size() << "本书";
}

// 添加设置页面相关实现
void MainWindow::setupSettingsPage()
{
    settingsPage = new QWidget(this);
    settingsPage->setStyleSheet("background-color: #f8f9fc;");
    settingsPage->hide();  // 初始隐藏设置页面
    
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsPage);
    settingsLayout->setSpacing(20);
    settingsLayout->setContentsMargins(30, 30, 30, 30);
    
    // 顶部返回按钮
    QPushButton *backBtn = new QPushButton("返回", this);
    backBtn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: #666666;
            border: none;
            padding: 8px 16px;
            font-size: 14px;
            text-align: left;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #f0f0ff;
            color: #6c5ce7;
        }
    )");
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::switchToMainPage);
    
    // 设置标题
    QLabel *titleLabel = new QLabel("设置", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2d3436; margin: 20px 0;");
    
    // 添加设置项（示例）
    QWidget *settingsContainer = new QWidget(this);
    QVBoxLayout *settingsItemsLayout = new QVBoxLayout(settingsContainer);
    settingsItemsLayout->setSpacing(16);
    
    // 添加一些示例设置项
    auto addSettingItem = [this, settingsItemsLayout](const QString &title, const QString &description) {
        QWidget *item = new QWidget(this);
        QVBoxLayout *itemLayout = new QVBoxLayout(item);
        
        QLabel *titleLabel = new QLabel(title, this);
        titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2d3436;");
        
        QLabel *descLabel = new QLabel(description, this);
        descLabel->setStyleSheet("font-size: 14px; color: #666666;");
        
        itemLayout->addWidget(titleLabel);
        itemLayout->addWidget(descLabel);
        
        settingsItemsLayout->addWidget(item);
    };
    
    addSettingItem("自动保存", "设置自动保存间隔时间");
    addSettingItem("字体设置", "设置编辑器字体和大小");
    addSettingItem("主题设置", "选择编辑器主题");
    addSettingItem("备份设置", "配置自动备份选项");
    
    settingsLayout->addWidget(backBtn);
    settingsLayout->addWidget(titleLabel);
    settingsLayout->addWidget(settingsContainer);
    settingsLayout->addStretch();
}

void MainWindow::switchToSettingsPage()
{
    QStackedWidget *stack = qobject_cast<QStackedWidget*>(mainArea->parentWidget());
    if (stack) {
        stack->setCurrentWidget(settingsPage);
        // 取消所有选中状态
        functionList->clearSelection();
    }
}

void MainWindow::switchToMainPage()
{
    QStackedWidget *stack = qobject_cast<QStackedWidget*>(mainArea->parentWidget());
    if (stack) {
        stack->setCurrentWidget(mainArea);
        // 选中书籍列表项
        for(int i = 0; i < functionList->count(); i++) {
            if(functionList->item(i)->text() == "书籍列表") {
                functionList->setCurrentItem(functionList->item(i));
                break;
            }
        }
        // 取消设置按钮的选中状态
        settingsBtn->setChecked(false);
    }
}
