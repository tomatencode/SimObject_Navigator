#ifndef DROPLABEL_HPP
#define DROPLABEL_HPP
#include <QApplication>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

class DropLabel : public QLabel {
public:
    DropLabel(QWidget *parent = nullptr) : QLabel(parent) {
        setAcceptDrops(true);
        setAlignment(Qt::AlignCenter);
        setMinimumSize(100, 50);
        setText("Drop SimObject here");
        setStyleSheet("border: 3px solid blue;");
    }
    std::string filename;
    class MainWindow;

    MainWindow * mw = nullptr;
protected:
    void dragEnterEvent(QDragEnterEvent *event) override
    {
        if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        if (event->mimeData()->hasUrls()) {
            QList<QUrl> urls = event->mimeData()->urls();
            if (urls.size() > 0) {
                setText(urls.first().toString());
                filename = urls.first().toString().toStdString();
                assert(mw);
            }
        }
    }
};
#endif // DROPLABEL_HPP
