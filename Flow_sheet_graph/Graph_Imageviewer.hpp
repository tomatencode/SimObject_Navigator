class Graph_ImageViewer : public QWidget {
public:
  Graph_ImageViewer(QWidget* parent = nullptr) : QWidget(parent) {
    QLabel* imageLabel = new QLabel(this);
    imageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //imageLabel->setScaledContents(true);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(scrollArea);

    setWindowTitle(tr("Image Viewer"));
  }

  void setImage(const QImage& image) {
    QLabel* imageLabel = findChild<QLabel*>();
    imageLabel->setPixmap(QPixmap::fromImage(image));
  }
};