#include "xdog_gui.h"

xdog_gui::xdog_gui(QWidget *parent)
	: QMainWindow(parent)
{
	// initialize data
	param = new XDOG_param();

	mainWidget = new QWidget(this);

	// initialize menu
	createActions();
	createMenu();

	// initialize ui control
	// initialize push button
	processButton = new QPushButton(tr("Process"));
	connect(processButton, SIGNAL(clicked()), this, SLOT(processClicked()));

	// label + slider + edit
	sigmaLabel = new QLabel(tr("sigma"));
	sigmaSlider = new QSlider(Qt::Vertical);
	sigmaSlider->setMinimum(0*slider_factor);
	sigmaSlider->setMaximum(10*slider_factor);
	sigmaSlider->setSingleStep(2);
	sigmaSlider->setValue(param->getSigma()*slider_factor);
	sigmaEdit = new QLineEdit(QString::number((1.0)*sigmaSlider->value()/slider_factor));
	connect(sigmaSlider, SIGNAL(valueChanged(int)), this, SLOT(slideSigma(int)));
	connect(sigmaEdit, SIGNAL(textChanged(const QString&)), this, SLOT(editSigma(const QString&)));

	kernelszLabel = new QLabel(tr("kernel size"));
	kernelszSlider = new QSlider(Qt::Vertical);
	kernelszSlider->setMinimum(3);
	kernelszSlider->setMaximum(31);
	kernelszSlider->setSingleStep(2);
	kernelszSlider->setValue(param->getKernelSz());
	kernelszEdit = new QLineEdit(QString::number(kernelszSlider->value()));
	connect(kernelszSlider, SIGNAL(valueChanged(int)), this, SLOT(slideKernelSz(int)));
	connect(kernelszEdit, SIGNAL(textChanged(const QString&)), this, SLOT(editKernelSz(const QString&)));

	tauLabel = new QLabel(tr("tau"));
	tauSlider = new QSlider(Qt::Vertical);
	tauSlider->setMinimum(0*slider_factor);
	tauSlider->setMaximum(2*slider_factor);
	tauSlider->setSingleStep(2);
	tauSlider->setValue(param->getTau()*slider_factor);
	tauEdit = new QLineEdit(QString::number((1.0)*tauSlider->value()/slider_factor));
	connect(tauSlider, SIGNAL(valueChanged(int)), this, SLOT(slideTau(int)));
	connect(tauEdit, SIGNAL(textChanged(const QString&)), this, SLOT(editTau(const QString&)));

	epsilonLabel = new QLabel(tr("epsilon"));
	epsilonSlider = new QSlider(Qt::Vertical);
	epsilonSlider->setMinimum(-1*slider_factor);
	epsilonSlider->setMaximum(1*slider_factor);
	epsilonSlider->setSingleStep(2);
	epsilonSlider->setValue(param->getEpsilon()*slider_factor);
	epsilonEdit = new QLineEdit(QString::number((1.0)*epsilonSlider->value()/slider_factor));
	connect(epsilonSlider, SIGNAL(valueChanged(int)), this, SLOT(slideEpsilon(int)));
	connect(epsilonEdit, SIGNAL(textChanged(const QString&)), this, SLOT(editEpsilon(const QString&)));

	phiLabel = new QLabel(tr("phi"));
	phiSlider = new QSlider(Qt::Vertical);
	phiSlider->setMinimum(1);
	phiSlider->setMaximum(1000);
	phiSlider->setSingleStep(10);
	phiSlider->setValue(param->getPhi());
	phiEdit = new QLineEdit(QString::number(phiSlider->value()));
	connect(phiSlider, SIGNAL(valueChanged(int)), this, SLOT(slidePhi(int)));
	connect(phiEdit, SIGNAL(textChanged(const QString&)), this, SLOT(editPhi(const QString&)));

	// initialize src & res display
	srcLabel0 = new QLabel(tr("source img 0"));
	srcLabel1 = new QLabel(tr("source img 1"));
	srcLabel2 = new QLabel(tr("source img 2"));
	resLabel = new QLabel(tr("result img"));

	// set layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	QHBoxLayout* displayLayout = new QHBoxLayout();
	QVBoxLayout* dispSrcLayout = new QVBoxLayout();
	QHBoxLayout* dispSrcAddLayout = new QHBoxLayout();
	QHBoxLayout* controlLayout = new QHBoxLayout();
	
	QVBoxLayout* sigmaLayout = new QVBoxLayout();
	QVBoxLayout* kernelszLayout = new QVBoxLayout();
	QVBoxLayout* tauLayout = new QVBoxLayout();
	QVBoxLayout* epsilonLayout = new QVBoxLayout();
	QVBoxLayout* phiLayout = new QVBoxLayout();

	dispSrcAddLayout->addWidget(srcLabel1);
	dispSrcAddLayout->addWidget(srcLabel2);
	dispSrcLayout->addWidget(srcLabel0);
	dispSrcLayout->addLayout(dispSrcAddLayout);
	displayLayout->addLayout(dispSrcLayout);
	displayLayout->addWidget(resLabel);

	sigmaLayout->addWidget(sigmaLabel);
	sigmaLayout->addWidget(sigmaSlider);
	sigmaLayout->addWidget(sigmaEdit);
	kernelszLayout->addWidget(kernelszLabel);
	kernelszLayout->addWidget(kernelszSlider);
	kernelszLayout->addWidget(kernelszEdit);
	tauLayout->addWidget(tauLabel);
	tauLayout->addWidget(tauSlider);
	tauLayout->addWidget(tauEdit);
	epsilonLayout->addWidget(epsilonLabel);
	epsilonLayout->addWidget(epsilonSlider);
	epsilonLayout->addWidget(epsilonEdit);
	phiLayout->addWidget(phiLabel);
	phiLayout->addWidget(phiSlider);
	phiLayout->addWidget(phiEdit);

	controlLayout->addWidget(processButton);
	controlLayout->addLayout(sigmaLayout);
	controlLayout->addLayout(kernelszLayout);
	controlLayout->addLayout(tauLayout);
	controlLayout->addLayout(epsilonLayout);
	controlLayout->addLayout(phiLayout);

	mainLayout->addLayout(displayLayout);
	mainLayout->addLayout(controlLayout);
	mainWidget->setLayout(mainLayout);

	this->setCentralWidget(mainWidget);
	this->setWindowTitle("XDoG demo");

	//ui.setupUi(this);
}

xdog_gui::~xdog_gui()
{

}


// callback of push buttons
void xdog_gui::processClicked()
{
	// compute DOG based on flow
	//GetFDoG(src_im, etf, 1.0, 3.0, 0.99); // tao = 0.99
	//int w = src_mat.cols, h = src_mat.rows;
	//for int i=0; i<h; i++){
	//	for(int j=0; j<w; j++){
	//		dog_mat.at<uchar>(i, j) = src_im.p[i][j];
	//	}
	//}
	//GrayThresholding(src_im, 0.7); // thres = 0.7

	// compute DOG
	compute_dog(src_mat, dog_mat, param);
	// thresholding
	threshold_dog(dog_mat, res_mat, param);
	// convert double ->[0, 255]
	double th_min, th_max;
	cv::minMaxIdx(res_mat, &th_min, &th_max);
	cv::Mat res_8u = ((res_mat - th_min)/(th_max - th_min))*MAX_PIX_VAL;
	res_8u.convertTo(res_8u, CV_8U);

	cv::Mat res_rgb;
	cv::cvtColor(res_8u, res_rgb, CV_GRAY2RGB);

	resQimg = QImage((const unsigned char*)(res_rgb.data),
		res_rgb.cols, res_rgb.rows,
		res_rgb.cols*res_rgb.channels(),
		QImage::Format_RGB888);
	resLabel->setPixmap(QPixmap::fromImage(resQimg));
	resLabel->resize(resLabel->pixmap()->size());
}

// callback of slide bars
void xdog_gui::slideSigma(int v)
{
	int val = sigmaSlider->value();
	QString str =  QString::number((1.0)*val/slider_factor);
	sigmaEdit->setText(str);
	param->setSigma((1.0)*val/slider_factor);
}

void xdog_gui::slideKernelSz(int v)
{
	int val = kernelszSlider->value();
	if(val%2 == 0)
		val += 1;
	QString str = QString::number(val);
	kernelszEdit->setText(str);
	param->setKernelSz(val);
}

void xdog_gui::slideTau(int v)
{
	int val = tauSlider->value();
	QString str =  QString::number((1.0)*val/slider_factor);
	tauEdit->setText(str);
	param->setTau((1.0)*val/slider_factor);
}

void xdog_gui::slideEpsilon(int v)
{
	int val = epsilonSlider->value();
	QString str =  QString::number((1.0)*val/slider_factor);
	epsilonEdit->setText(str);
	param->setEpsilon((1.0)*val/slider_factor);
}

void xdog_gui::slidePhi(int v)
{
	int val = phiSlider->value();
	QString str =  QString::number(val);
	phiEdit->setText(str);
	param->setPhi(val);
}

// TODO 
// callback of line edits
void xdog_gui::editSigma(const QString& t)
{
}

void xdog_gui::editKernelSz(const QString& t)
{
}

void xdog_gui::editTau(const QString& t)
{
}

void xdog_gui::editEpsilon(const QString& t)
{
}

void xdog_gui::editPhi(const QString& t)
{
}

// menu
void xdog_gui::createActions()
{
	fileSrc = new QAction(tr("Load Src"), this);
	connect(fileSrc, SIGNAL(triggered()), this, SLOT(loadSrc()));
	fileTexture = new QAction(tr("Load Texture"), this);
	connect(fileTexture, SIGNAL(triggered()), this, SLOT(loadTexture()));

	effAbstraction = new QAction(tr("Abstraction"), this);
	connect(effAbstraction, SIGNAL(triggered()), this, SLOT(setAbstraction()));
	effIndication = new QAction(tr("Indication"), this);
	connect(effIndication, SIGNAL(triggered()), this, SLOT(setIndication()));
	effSpeedLine = new QAction(tr("Speed-lines"), this);
	connect(effSpeedLine, SIGNAL(triggered()), this, SLOT(setSpeedLine()));
	effGhosting = new QAction(tr("Ghosting"), this);
	connect(effGhosting, SIGNAL(triggered()), this, SLOT(setGhosting()));
	effNegativeEdge = new QAction(tr("Negative Edges"), this);
	connect(effNegativeEdge, SIGNAL(triggered()), this, SLOT(setNegativeEdge()));
	
	styPencilShading = new QAction(tr("Pencil-shading"), this);
	connect(styPencilShading, SIGNAL(triggered()), this, SLOT(setPencilShading()));
	styCharcoal = new QAction(tr("Charcoal"), this);
	connect(styCharcoal, SIGNAL(triggered()), this, SLOT(setCharcoal()));
	styPastel = new QAction(tr("Pastel"), this);
	connect(styPastel, SIGNAL(triggered()), this, SLOT(setPastel()));
	styPastelColor = new QAction(tr("Colored Pastel"), this);
	connect(styPastelColor, SIGNAL(triggered()), this, SLOT(setPastelColor()));
	styThresholding = new QAction(tr("Thresholding"), this);
	connect(styThresholding, SIGNAL(triggered()), this, SLOT(setThresholding()));
	styHatching = new QAction(tr("Hatching"), this);
	connect(styHatching, SIGNAL(triggered()), this, SLOT(setHatching()));
}

void xdog_gui::createMenu()
{
	fileMenu = menuBar()->addMenu(tr("File"));
	fileMenu->addAction(fileSrc);
	fileMenu->addAction(fileTexture);

	effectMenu = menuBar()->addMenu(tr("Effect"));
	effectMenu->addAction(effAbstraction);
	effectMenu->addAction(effIndication);
	effectMenu->addAction(effSpeedLine);
	effectMenu->addAction(effGhosting);
	effectMenu->addAction(effNegativeEdge);

	styleMenu = menuBar()->addMenu(tr("Style"));
	styleMenu->addAction(styPencilShading);
	styleMenu->addAction(styCharcoal);
	styleMenu->addAction(styPastel);
	styleMenu->addAction(styPastelColor);
	styleMenu->addAction(styThresholding);
	styleMenu->addAction(styHatching);
}

void xdog_gui::updateParam()
{
}

// menu : file operations
void xdog_gui::loadSrc()
{
	src_file = QFileDialog::getOpenFileName(this, tr("Open Source Image"),
		"./image/", tr("Image Files (*.png *.jpg *.bmp)"));
	src_mat = cv::imread(src_file.toStdString());

	cv::Mat rgb_mat;

	if(src_mat.channels() == 3){
		cv::cvtColor(src_mat, rgb_mat, CV_BGR2RGB);
		srcQimg = QImage((const unsigned char*)(rgb_mat.data),
			rgb_mat.cols, rgb_mat.rows,
			rgb_mat.cols*rgb_mat.channels(),
			QImage::Format_RGB888);
	}
	else{
		srcQimg = QImage((const unsigned char*)(src_mat.data),
			src_mat.cols, src_mat.rows,
			src_mat.cols*src_mat.channels(),
			QImage::Format_RGB888);
	}

	srcLabel0->setPixmap(QPixmap::fromImage(srcQimg));
	srcLabel0->resize(srcLabel0->pixmap()->size());

	cv::cvtColor(src_mat, src_mat, CV_BGR2GRAY);
	src_mat.convertTo(src_mat, CV_32F);
	src_mat /= MAX_PIX_VAL;
	dog_mat = cv::Mat::zeros(src_mat.size(), src_mat.type());
	res_mat = cv::Mat::zeros(src_mat.size(), src_mat.type());

	// FDOG
	//int w = src_mat.cols, h = src_mat.rows;
	//src_im = imatrix(h, w);
	//for(int i=0; i<h; i++){
	//	for(int j=0; j<w; j++){
	//		src_im.p[i][j] = src_mat.at<uchar>(i,j);
	//	}
	//}
	// create flow
	//etf.init(src_im.getRow(), src_im.getCol());
	//etf.set(src_im);
	//etf.Smooth(4, 2);
}

void xdog_gui::loadTexture()
{
}

// menu : predefined params
void xdog_gui::setAbstraction()
{
}

void xdog_gui::setIndication()
{
}

void xdog_gui::setSpeedLine()
{
}

void xdog_gui::setGhosting()
{
}

void xdog_gui::setNegativeEdge()
{
}

void xdog_gui::setPencilShading()
{
}

void xdog_gui::setCharcoal()
{
}

void xdog_gui::setPastel()
{
}

void xdog_gui::setPastelColor()
{
}

void xdog_gui::setThresholding()
{
}

void xdog_gui::setHatching()
{
}