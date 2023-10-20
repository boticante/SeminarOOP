#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QFileDialog>
#include <QTextStream>
#include <QMenuBar>
#include <QFile>
#include <QIODevice>
#include <QString>
#include <QColor>
#include <QPoint>
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QRubberBand>
#include <QtGui>
#include <QApplication>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QTextDocument>
#include <QListView>
#include <QPixmap>

class MyMainWindow : public QMainWindow
{
public:
    MyMainWindow();
    virtual ~MyMainWindow();
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QLabel* MyLabel;
    QMenu* FileMenu;
    QAction* FileSaveAs;
    QAction* FileOpen;
    QRect DashRect, FullRect;
    QPoint StartPoint, EndPoint;

    void fileSaveAsMenu();
    void fileOpenMenu();
    void getCoordinates(QPoint leftTop, QPoint rightBottom);
};

MyMainWindow::MyMainWindow()
{
    MyLabel = new QLabel(this);
    MyLabel->setText("Hello World!");

    FileSaveAs = new QAction(tr("&Save As..."), this);
    FileSaveAs->setShortcut(tr("CTRL+S"));
    connect(FileSaveAs, &QAction::triggered, this, &MyMainWindow::fileSaveAsMenu);

    FileOpen = new QAction(tr("&Open..."), this);
    FileOpen->setShortcut(tr("CTRL+O"));
    connect(FileOpen, &QAction::triggered, this, &MyMainWindow::fileOpenMenu);

    FileMenu = menuBar()->addMenu(tr("&File"));
    FileMenu->addAction(FileSaveAs);
    FileMenu->addAction(FileOpen);

    FileMenu = menuBar()->addMenu(tr("&Edit"));
    FileMenu = menuBar()->addMenu(tr("&Mode"));

    QString str("color: black;"
               "background-color: white;"
               "selection-color: black;"
               "selection-background-color: lightblue;");
    menuBar()->setStyleSheet(str);
}

MyMainWindow::~MyMainWindow()
{
    delete(MyLabel);
    delete(FileSaveAs);
    delete(FileOpen);
}

void MyMainWindow::mousePressEvent(QMouseEvent* event)
{
    StartPoint = event->pos();
}

void MyMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    DashRect = QRect(StartPoint, event->pos()).normalized();
    update();
}

void MyMainWindow::mouseReleaseEvent(QMouseEvent*)
{
    if (!DashRect.isNull())
        FullRect = DashRect;

    DashRect = QRect();
    update();
}

void MyMainWindow::paintEvent(QPaintEvent*event)
{
    QSize size = this->size();
    QPainter painter(this);
    QMainWindow::paintEvent(event);
    painter.drawLine(0, size.height(), size.width(), menuBar()->height());

    if(!DashRect.isNull()) {
        painter.setPen(Qt::DashLine);
        painter.drawRect(DashRect);
    }

    if(!FullRect.isNull()) {
        painter.setPen(Qt::black);
        painter.drawRect(FullRect);
        MyLabel->move(FullRect.left() + 10, FullRect.top() );
    }
}

void MyMainWindow::fileSaveAsMenu()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Save As...", "", "FESB File (*.fsb)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this,"Unable to Open File", file.errorString());
            return;
        }

        QTextStream out(&file);
        out << "fesb file" << Qt::endl;
        out << MyLabel->text() << Qt::endl;
        out << FullRect.left() << Qt::endl;
        out << FullRect.top() << Qt::endl;
        out << FullRect.width() << Qt::endl;
        out << FullRect.height() << Qt::endl;
        out << pos().x() << Qt::endl;
        out << pos().y() << Qt::endl;
        out << size().width() << Qt::endl;
        out << size().height() << Qt::endl;
    }
}

void MyMainWindow::fileOpenMenu()
{

    QString fileName = QFileDialog::getOpenFileName(this,"Open Geometry...", "", "FESB File (*.fsb)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, "Unable to Open File", file.errorString());
            return;
        }

        QTextStream in(&file);
        QString str; str = in.readLine();

        if(str=="fesb file") {
            str = in.readLine();
            MyLabel->setText(str);
            int left, top, x, y, w, h;
            in >> left >> top >> w >> h;
            FullRect = QRect(left, top, w, h);
            in >> x >> y >> w >> h;
            this->setGeometry(x, y, w, h);
            update();
        }
    }
}

void MyMainWindow::keyPressEvent(QKeyEvent *event)
{
    QSize size = this->size();

    switch( event->key() ) {

        case Qt::Key_Left:
            FullRect.moveLeft(FullRect.left() - 1);

            if(FullRect.left() == 0) {
                FullRect.moveLeft(FullRect.left() + 1);
            }

            update();
            break;

        case Qt::Key_Right:
            FullRect.moveLeft(FullRect.left() + 1);

            if(FullRect.right() == size.width()) {
                FullRect.moveLeft(FullRect.left() - 1);
            }

            update();
            break;

        case Qt::Key_Up:
            FullRect.moveTop(FullRect.top() - 1);

            if(FullRect.top() == 22) {
                FullRect.moveTop(FullRect.top() + 1);
            }

            update();
            break;

        case Qt::Key_Down:
            FullRect.moveTop(FullRect.top() + 1);

            if(FullRect.bottom() == size.height()) {
                FullRect.moveTop(FullRect.top() - 1);
            }

            update();
            break;
    }
}

int main(int argc, char **argv)
{
    QApplication app (argc, argv);
    MyMainWindow mainWindow;
    mainWindow.resize(500,300);
    mainWindow.show();

    return app.exec();
}
