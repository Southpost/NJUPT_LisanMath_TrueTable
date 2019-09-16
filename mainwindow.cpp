#include "mainwindow.h"
#include "ui_mainwindow.h"
#define ERROR -1
#define TRUE 1

using namespace std;
static string infix;           //中缀
static string postfix;       //后缀
static string alpha;          //所有字母变量
static map<char, int> M;   //将字母变量与0或1对应

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->model = new QStandardItemModel;
    this->ui->tableView->setModel(model);

    this->setWindowTitle("真值表法求范式");
    this->setWindowIcon(QIcon(":/ico/cal.ico"));
    this->ui->lineEdit->setStyleSheet ("border:2px groove gray;border-radius:10px;padding:2px 4px;");
    this->ui->textBrowser->setStyleSheet ("border:2px groove gray;border-radius:10px;padding:2px 4px;");
    this->ui->textBrowser_2->setStyleSheet ("border:2px groove gray;border-radius:10px;padding:2px 4px;");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pre() {  //去除中缀表达式中 单条件的>,和双条件的= 和 >,以及多余的!
    size_t flag = 0;
    size_t len = infix.size();
    infix.insert(0, 1, '(');
    for (size_t i = 0; i < len; i++) {
        int f = 0;
        if (infix[i] == '>' || infix[i] == '=') {
                infix.erase(infix.begin() + i);
                i--;
        }
        else if(infix[i] == '!') {
            flag++;
            f = 1;
        }
        if(f == 0 && flag) {
            if(flag & 1) {
                for(size_t j = i - 1; j >= i - flag + 1; j--) {
                    infix.erase(infix.begin() + j);
                }
            }else {
                for(size_t j = i - 1; j >= i - flag; j--) {
                    infix.erase(infix.begin() + j);
                }
            }
            flag = 0;
        }
    }
}
int MainWindow::icp(char a) {     //栈外优先级
    if (a == '#') return 0;
    if (a == '(') return 12;
    if (a == '!') return 10;
    if (a == '&') return 8;
    if (a == '|') return 6;
    if (a == '-') return 4;
    if (a == '<') return 2;
    if (a == ')') return 1;
    return ERROR;
}
int MainWindow::isp(char a) {      //栈内优先级
    if (a == '#') return 0;
    if (a == '(') return 1;
    if (a == '!') return 11;
    if (a == '&') return 9;
    if (a == '|') return 7;
    if (a == '-') return 5;
    if (a == '<') return 3;
    if (a == ')') return 12;
    return ERROR;
}
void MainWindow::InfixToPostfix()    //中缀表达式转换后缀表达式
{
    size_t j = 0;
    stack<char> s;
    char ch, y;
    s.push('#');
    stringstream ss;
    ss << infix;
    while (ss >> ch && ch != '#') {
        if(!isalpha(ch) && icp(ch) == ERROR){
            QMessageBox::critical(this, "ERROR","输入有误！请按规定填写！","确定");
            return;
        }
        if (isalpha(ch)) {
            postfix[j++] = ch;
            //qDebug() << alpha.find(ch);
            if (alpha.find(ch) == -1) {
                alpha.push_back(ch);
                //qDebug("99");
            }
        }
        else if (ch == ')') {
            for (y = s.top(), s.pop(); y != '('; y = s.top(), s.pop()) {
                postfix[j++] = y;
            }
        }
        else {
            for (y = s.top(), s.pop(); icp(ch) <= isp(y); y = s.top(), s.pop()) {
                postfix[j++] = y;
            }
            s.push(y);          //重新压回
            s.push(ch);
        }
    }
    lenA = alpha.size();
    while (!s.empty()) {
        y = s.top();
        s.pop();
        if (y != '#') {
            postfix[j++] = y;
        }
    }
    postfix[j] = '#';
}
int MainWindow::cal()   //对后缀表达式进行计算
{
    stack<int> s;
    char ch;
    size_t j = 0;
    int t1, t2;
    while (TRUE) {
        ch = postfix[j];
        if (ch == '#' || ch == 0)
            break;
        j++;
        if (isalpha(ch)) {
            s.push(M[ch]);
        }
        else if (ch == '!') {   //非
                t1 = s.top();
                s.pop();
                s.push(!t1);
        }
        else if (ch == '&') {   //与
            t1 = s.top();
            s.pop();
            t2 = s.top();
            s.pop();
            if (t1 == 1 && t2 == 1) {
                s.push(1);
            }
            else {
                s.push(0);
            }
        }
        else if (ch == '|') {    //或
            t1 = s.top();
            s.pop();
            t2 = s.top();
            s.pop();
            if (t1 == 0 && t2 == 0) {
                    s.push(0);
            }
            else {
                s.push(1);
            }
        }
        else if (ch == '-') {   //条件
            t1 = s.top();
            s.pop();
            t2 = s.top();
            s.pop();
            if (t1 == 0 && t2 == 1) {
                    s.push(0);
            }
            else {
                s.push(1);
            }
        }
        else if (ch == '<') {  //双条件
            t1 = s.top();
            s.pop();
            t2 = s.top();
            s.pop();
            if ((t1 == 1 && t2 == 1) || (t1 == 0 && t2 == 0)) {
                    s.push(1);
            }
            else {
                    s.push(0);
            }
        }
    }
    int ans = s.top();
    return ans;
}
void MainWindow::dfs(int cur)   //递归枚举并求取范式
{   
    if (cur == lenA) {
        int ans = cal();
        int i;
        for (i = 0; i < lenA; i++) {
            if (M[alpha[i]]) {
                this->model->setItem(row, i, new QStandardItem("T"));
                this->model->item(row,i)->setTextAlignment(Qt::AlignCenter);
            }
            else {
                this->model->setItem(row, i, new QStandardItem("F"));
                this->model->item(row,i)->setTextAlignment(Qt::AlignCenter);
            }
        }
        /**
        * 主析取范式
        */
        if (ans == 1) {
            this->model->setItem(row, i, new QStandardItem("T"));
            this->model->item(row,i)->setTextAlignment(Qt::AlignCenter);
            if(flagT != 0) {
                this->ui->textBrowser->insertPlainText("∨");
            }else{
                flagT = 1;
            }
            this->ui->textBrowser->insertPlainText("(");
            for (int i = 0; i < lenA; i++) {
                if (i != 0) {
                    this->ui->textBrowser->insertPlainText("∧");
                }
                if (M[alpha[i]] == 1) {
                    this->ui->textBrowser->insertPlainText(QString(alpha[i]));
                }
                else {
                    this->ui->textBrowser->insertPlainText("￢" + QString(alpha[i]));
                }
            }
            this->ui->textBrowser->insertPlainText(")");
        }
        /**
        * 主析取范式
        */
        else {
            this->model->setItem(row, i, new QStandardItem("F"));
            this->model->item(row,i)->setTextAlignment(Qt::AlignCenter);
            if(flagF != 0) {
                this->ui->textBrowser_2->insertPlainText("∧");
            }else{
                flagF = 1;
            }
            this->ui->textBrowser_2->insertPlainText("(");
            for (int i = 0; i < lenA; i++) {
                if (i != 0) {
                    this->ui->textBrowser_2->insertPlainText("∨");
                }
                if (M[alpha[i]] == 0) {
                    this->ui->textBrowser_2->insertPlainText(QString(alpha[i]));
                }
                else {
                    this->ui->textBrowser_2->insertPlainText("￢" + QString(alpha[i]));
                }
            }
            this->ui->textBrowser_2->insertPlainText(")");
        }
        row++;
        return;
    }
    M[alpha[cur]] = 1;
    dfs(cur + 1);
    M[alpha[cur]] = 0;
    dfs(cur + 1);
}
void MainWindow::on_pushButton_clicked()
{
    this->model->clear();
    this->ui->textBrowser->clear();
    this->ui->textBrowser_2->clear();
    flagT = 0;
    flagF = 0;
    M.clear();
    alpha.clear();
    if(this->ui->lineEdit->text() == nullptr) {
        QMessageBox::critical(this, "ERROR","请输入内容！","确定");
        return;
    }
    infix = this->ui->lineEdit->text().toStdString();
    infix.append(")#");
    pre();  //预处理
    InfixToPostfix();   //转后缀
    for (int i = 0; i < lenA; i++) {    //设置表头
        this->model->setHorizontalHeaderItem(i,new QStandardItem(QString(alpha[i])));
    }
    this->model->setHorizontalHeaderItem(lenA,new QStandardItem("表达式真值"));
    this->ui->tableView->setModel(model);
    for (int i = 0; i < lenA; i++) {
        this->ui->tableView->setColumnWidth(i,45);
    }
    this->ui->tableView->setColumnWidth(lenA+ 1,100);
    row = 0;
    dfs(0);  //打印真值表
}
