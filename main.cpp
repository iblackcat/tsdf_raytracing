#include "QtGuiApplication.h"
#include <QtWidgets/QApplication>

#include "TriangleWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	/*
	QSurfaceFormat format;
	format.setSamples(16);

	TriangleWindow window;
	window.setFormat(format);
	window.resize(320, 240);
	window.show();

	window.setAnimating(true);
	*/

	QtGuiApplication w;
	w.show();

	return app.exec();
}
