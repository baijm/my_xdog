#include "xdog_gui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	xdog_gui w;
	w.show();
	return a.exec();
}
