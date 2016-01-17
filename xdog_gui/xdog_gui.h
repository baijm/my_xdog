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
	enum selection{PENCIL_SHADING=1, CHARCOAL=2, PASTEL=3, THRESHOLD=4, COLOR_PASTEL=5};
	selection curr_sel;

	// menu
	QAction *fileSrc;
	QAction *styPencilShading, *styCharcoal, *styPastel, *styPastelColor, *styThresholding;
	QMenu *fileMenu, *styleMenu;

	// ui control
	Ui::xdog_guiClass ui;

	QWidget* mainWidget;

	QPushButton* processButton;

	QSlider *sigmaSlider, *kernelszSlider, *tauSlider, *epsilonSlider, *phiSlider;
	static const int slider_factor = 1000;

	QLabel *sigmaLabel, *kernelszLabel, *tauLabel, *epsilonLabel, *phiLabel;
	QLabel *srcLabel, *resLabel;

	QLineEdit *sigmaEdit, *kernelszEdit, *tauEdit, *epsilonEdit, *phiEdit;

	// global data
	XDOG_param* param;
	QString src_file;
	cv::Mat src_mat, rgb_mat, dog_mat, res_mat; 
	QImage srcQimg, resQimg;

	imatrix src_im;
	mymatrix fdog_im;
	ETF etf;

	void createActions();
	void createMenu();
	void updateParam();

private slots:
	// menu : file operation
	void loadSrc();
	void loadTexture();
	// menu : predefined params
	void setPencilShading();
	void setCharcoal();
	void setPastel();
	void setPastelColor();
	void setThresholding();

	// button
	void processClicked();// process img

	// slide bar
	void slideSigma(int v);
	void slideKernelSz(int v);
	void slideTau(int v);
	void slideEpsilon(int v);
	void slidePhi(int v);
};

#endif // XDOG_GUI_H