#include "waveprodlg.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	WaveProDlg w;
	w.show();
	return a.exec();
}
