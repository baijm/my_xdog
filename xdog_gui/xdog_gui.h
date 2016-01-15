#ifndef XDOG_GUI_H
#define XDOG_GUI_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLayout>
#include <QtWidgets/QFileDialog>

#include "ui_xdog_gui.h"
#include "xdog_data.h"

#include "imatrix.h"
#include "myvec.h"
#include "ETF.h"
#include "fdog.h"

class xdog_gui : public QMainWindow
{
	Q_OBJECT

public:
	xdog_gui(QWidget *parent = 0);
	~xdog_gui();

private:
	// menu
	QAction *fileSrc, *fileTexture;
	QAction *effAbstraction, *effIndication, *effSpeedLine, *effGhosting, *effNegativeEdge;
	QAction *styPencilShading, *styCharcoal, *styPastel, *styPastelColor, *styThresholding, *styHatching;
	QMenu *fileMenu, *effectMenu, *styleMenu;

	// ui control
	Ui::xdog_guiClass ui;

	QWidget* mainWidget;

	QPushButton* processButton;

	QSlider *sigmaSlider, *kernelszSlider, *tauSlider, *epsilonSlider, *phiSlider;
	static const int slider_factor = 1000;

	QLabel *sigmaLabel, *kernelszLabel, *tauLabel, *epsilonLabel, *phiLabel;
	QLabel *srcLabel0, *srcLabel1, *srcLabel2, *resLabel;

	QLineEdit *sigmaEdit, *kernelszEdit, *tauEdit, *epsilonEdit, *phiEdit;

	// global data
	XDOG_param* param;
	QString src_file;
	cv::Mat src_mat, dog_mat, res_mat; 
	QImage srcQimg, resQimg;

	imatrix src_im;
	ETF etf;

	void createActions();
	void createMenu();
	void updateParam();

private slots:
	// menu : file operation
	void loadSrc();
	void loadTexture();
	// menu : predefined params
	void setAbstraction();
	void setIndication();
	void setSpeedLine();
	void setGhosting();
	void setNegativeEdge();
	void setPencilShading();
	void setCharcoal();
	void setPastel();
	void setPastelColor();
	void setThresholding();
	void setHatching();

	// button
	void processClicked();// process img

	// slide bar
	void slideSigma(int v);
	void slideKernelSz(int v);
	void slideTau(int v);
	void slideEpsilon(int v);
	void slidePhi(int v);

	// line edit
	void editSigma(const QString& t);
	void editKernelSz(const QString& t);
	void editTau(const QString& t);
	void editEpsilon(const QString& t);
	void editPhi(const QString& t);
};

#endif // XDOG_GUI_H