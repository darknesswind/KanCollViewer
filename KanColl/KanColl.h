#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_KanColl.h"

class GameCache;
class KanColl : public QMainWindow
{
	Q_OBJECT

public:
	KanColl(QWidget *parent = Q_NULLPTR);
	~KanColl();

public slots:
	void onChangeWall();
	void onChangeFloor();
	void onChangeDesk();
	void onChangeWindow();
	void onChangeObject();
	void onChangeChest();

	void onExportSwf();

	void onShipSelected(const ShipGraphFile& file);
	void onShipActivated(const ShipGraphFile& file);

private:
	void initMenu();
	void initShipDock();
	void initPreview();

	bool changeFurniture(const QString& type, QPixmap& img);

private:
	Ui::QtGuiApplication1Class m_ui;
	std::unique_ptr<GameCache> m_spCache;
};
