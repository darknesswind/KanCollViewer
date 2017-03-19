#include "stdafx.h"
#include "KanColl.h"
#include <QtWidgets/QApplication>
// #include "SWFDecoder.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

// 	SWFDecoder decoder;
// 	decoder.decode();

	KanColl w;
	w.show();
	return a.exec();
}
