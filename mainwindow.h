#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <queue>
#include <stack>
#include <map>
#include <sstream>
#include <QStandardItemModel>
#include <QDialog>
#include <QList>
#include <QString>
#include <QStandardItem>
#include <QLatin1String>
#include<QDebug>
#include<QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void pre();
    int icp(char a);
    int isp(char a);
    void InfixToPostfix();    //中缀表达式转换后缀表达式
    int cal();
    void dfs(int cur);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QStandardItemModel * model;
    int row;
    int flagT, flagF, lenA;

};

#endif // MAINWINDOW_H
