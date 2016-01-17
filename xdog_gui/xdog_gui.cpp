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

	kernelszLabel = new QLabel(tr("kernel size"));
	kernelszSlider = new QSlider(Qt::Vertical);
	kernelszSlider->setMinimum(3);
	kernelszSlider->setMaximum(31);
	kernelszSlider->setSingleStep(2);
	kernelszSlider->setValue(param->getKernelSz());
	kernelszEdit = new QLineEdit(QString::number(kernelszSlider->value()));
	connect(kernelszSlider, SIGNAL(valueChanged(int)), this, SLOT(slideKernelSz(int)));

	tauLabel = new QLabel(tr("tau"));
	tauSlider = new QSlider(Qt::Vertical);
	tauSlider->setMinimum(0*slider_factor);
	tauSlider->setMaximum(2*slider_factor);
	tauSlider->setSingleStep(2);
	tauSlider->setValue(param->getTau()*slider_factor);
	tauEdit = new QLineEdit(QString::number((1.0)*tauSlider->value()/slider_factor));
	connect(tauSlider, SIGNAL(valueChanged(int)), this, SLOT(slideTau(int)));
	
	epsilonLabel = new QLabel(tr("epsilon"));
	epsilonSlider = new QSlider(Qt::Vertical);
	epsilonSlider->setMinimum(-5*slider_factor);
	epsilonSlider->setMaximum(5*slider_factor);
	epsilonSlider->setSingleStep(2);
	epsilonSlider->setValue(param->getEpsilon()*slider_factor);
	epsilonEdit = new QLineEdit(QString::number((1.0)*epsilonSlider->value()/slider_factor));
	connect(epsilonSlider, SIGNAL(valueChanged(int)), this, SLOT(slideEpsilon(int)));
	
	phiLabel = new QLabel(tr("phi"));
	phiSlider = new QSlider(Qt::Vertical);
	phiSlider->setMinimum(1);
	phiSlider->setMaximum(1000);
	phiSlider->setSingleStep(10);
	phiSlider->setValue(param->getPhi());
	phiEdit = new QLineEdit(QString::number(phiSlider->value()));
	connect(phiSlider, SIGNAL(valueChanged(int)), this, SLOT(slidePhi(int)));
	
	// initialize src & res display
	srcLabel = new QLabel(tr("source img"));
	resLabel = new QLabel(tr("result img"));

	// set layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	QHBoxLayout* displayLayout = new QHBoxLayout();
	QHBoxLayout* controlLayout = new QHBoxLayout();
	
	QVBoxLayout* sigmaLayout = new QVBoxLayout();
	QVBoxLayout* kernelszLayout = new QVBoxLayout();
	QVBoxLayout* tauLayout = new QVBoxLayout();
	QVBoxLayout* epsilonLayout = new QVBoxLayout();
	QVBoxLayout* phiLayout = new QVBoxLayout();

	displayLayout->addWidget(srcLabel);
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
	if(curr_sel == THRESHOLD){
		// compute DOG based on flow
		double curr_sigma = param->getSigma();
		double curr_tau = param->getTau();
		GetFDoG(fdog_im, src_im, etf, curr_sigma, 3*curr_sigma, curr_tau); 
		int w = src_mat.cols, h = src_mat.rows;
		for(int i=0; i<h; i++){
			for(int j=0; j<w; j++){
				dog_mat.at<float>(i, j) = fdog_im.get(i, j);
			}
		}
	}
	else{
		// pure XDOG
		compute_dog(src_mat/MAX_PIX_VAL, dog_mat, param);
	}

	// thresholding
	threshold_dog(dog_mat, res_mat, param);
	double th_min, th_max;
	cv::minMaxIdx(res_mat, &th_min, &th_max);
	res_mat = (res_mat - th_min)/(th_max - th_min);

	cv::Mat res_rgb;
	if(curr_sel == COLOR_PASTEL){
		res_rgb = rgb_mat.clone();
		int nr = src_mat.rows;
		int nc = src_mat.cols;
		for(int r=0; r<nr; r++){
			for(int c=0; c<nc; c++){
				res_rgb.at<cv::Vec3b>(r, c)[0] *= res_mat.at<float>(r, c);
				res_rgb.at<cv::Vec3b>(r, c)[1] *= res_mat.at<float>(r, c);
				res_rgb.at<cv::Vec3b>(r, c)[2] *= res_mat.at<float>(r, c);
			}
		}
	}
	else{
		cv::Mat res_8u = res_mat*MAX_PIX_VAL;
		res_8u.convertTo(res_8u, CV_8U);
		cv::cvtColor(res_8u, res_rgb, CV_GRAY2RGB);
	}

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

// menu
void xdog_gui::createActions()
{
	fileSrc = new QAction(tr("Load Src"), this);
	connect(fileSrc, SIGNAL(triggered()), this, SLOT(loadSrc()));
	
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
}

void xdog_gui::createMenu()
{
	fileMenu = menuBar()->addMenu(tr("File"));
	fileMenu->addAction(fileSrc);

	styleMenu = menuBar()->addMenu(tr("Style"));
	styleMenu->addAction(styPencilShading);
	styleMenu->addAction(styCharcoal);
	styleMenu->addAction(styPastel);
	styleMenu->addAction(styPastelColor);
	styleMenu->addAction(styThresholding);
}

void xdog_gui::updateParam()
{
	double curr_sigma = param->getSigma();
	sigmaSlider->setValue(curr_sigma*slider_factor);
	sigmaEdit->setText(QString::number(curr_sigma));

	int curr_kernel = param->getKernelSz();
	kernelszSlider->setValue(curr_kernel);
	kernelszEdit->setText(QString::number(curr_kernel));

	double curr_tau = param->getTau();
	tauSlider->setValue(curr_tau*slider_factor);
	tauEdit->setText(QString::number(curr_tau));

	double curr_epsilon = param->getEpsilon();
	epsilonSlider->setValue(curr_epsilon*slider_factor);
	epsilonEdit->setText(QString::number(curr_epsilon));

	int curr_phi = param->getPhi();
	phiSlider->setValue(curr_phi);
	phiEdit->setText(QString::number(curr_phi));
}

// menu : file operations
void xdog_gui::loadSrc()
{
	src_file = QFileDialog::getOpenFileName(this, tr("Open Source Image"),
		"./image/", tr("Image Files (*.png *.jpg *.bmp)"));
	src_mat = cv::imread(src_file.toStdString());

	// display src
	cv::cvtColor(src_mat, rgb_mat, CV_BGR2RGB);
	srcQimg = QImage((const unsigned char*)(rgb_mat.data),
		rgb_mat.cols, rgb_mat.rows,
		rgb_mat.cols*rgb_mat.channels(),
		QImage::Format_RGB888);
	srcLabel->setPixmap(QPixmap::fromImage(srcQimg));
	srcLabel->resize(srcLabel->pixmap()->size());

	cv::cvtColor(src_mat, src_mat, CV_BGR2GRAY);
	// create flow
	int w = src_mat.cols, h = src_mat.rows;
	src_im.init(h, w);
	for(int i=0; i<h; i++){
		for(int j=0; j<w; j++){
			src_im.p[i][j] = src_mat.at<uchar>(i,j);
		}
	}
	fdog_im = mymatrix(h, w);
	etf.init(src_im.getRow(), src_im.getCol());
	etf.set(src_im);
	etf.Smooth(4, 2);

	src_mat.convertTo(src_mat, CV_32F);
	//src_mat /= MAX_PIX_VAL;
	dog_mat = cv::Mat::zeros(src_mat.size(), src_mat.type());
	res_mat = cv::Mat::zeros(src_mat.size(), src_mat.type());
}

// menu : predefined params
void xdog_gui::setPencilShading()
{
	curr_sel = PENCIL_SHADING;
	param->setSigma(0.4);
	param->setKernelSz(5);
	param->setTau(0.914);
	param->setEpsilon(0.056);
	param->setPhi(20);

	updateParam();
}

void xdog_gui::setCharcoal()
{
	curr_sel = CHARCOAL;
	param->setSigma(7);
	param->setKernelSz(21);
	param->setTau(0.914);
	param->setEpsilon(0.056);
	param->setPhi(20);

	updateParam();
}

void xdog_gui::setPastel()
{
	curr_sel = PASTEL;
	param->setSigma(5);
	param->setKernelSz(15);
	param->setTau(0.914);
	param->setEpsilon(0.056);
	param->setPhi(20);

	updateParam();
}

void xdog_gui::setPastelColor()
{
	curr_sel = COLOR_PASTEL;
	param->setSigma(5);
	param->setKernelSz(15);
	param->setTau(0.914);
	param->setEpsilon(0.056);
	param->setPhi(20);

	updateParam();
}

void xdog_gui::setThresholding()
{
	curr_sel = THRESHOLD;
	param->setSigma(1);
	param->setKernelSz(5);
	param->setTau(0.914);
	param->setEpsilon(0.5);
	param->setPhi(200);

	updateParam();
}
