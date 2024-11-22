//
// Created by gms on 24-11-18.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollArea>
#include <QMouseEvent>
#include <QDir>
#include <QStandardPaths>
#include <QScreen>
#include <QGuiApplication>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QPoint m_dragPosition;
    
    // 左侧边栏组件
    QWidget *sideBar;
    QLabel *userAvatar;
    QLabel *userName;
    QListWidget *functionList;
    
    // 主界面组件
    QWidget *mainArea;
    QPushButton *syncLibraryBtn;
    QPushButton *newBookBtn;
    QGridLayout *booksGrid;
    
    // 设置页面相关声明
    QWidget *settingsPage;  // 设置页面
    void setupSettingsPage();  // 设置页面初始化
    void switchToSettingsPage();  // 切换到设置页面
    void switchToMainPage();      // 切换回主页面
    
    void setupUI();
    void setupSideBar();
    void setupMainArea();
    QFrame* createBookCard(const QString &title, const QString &wordCount);
    bool checkAndCreateBackupFolder();
    QString backupFolderPath;
    void loadBooks();  // 加载书籍列表
    QList<QString> bookList;  // 存储书籍列表
    QPushButton *settingsBtn;  // 添加设置按钮成员变量
};

#endif // MAINWINDOW_H
