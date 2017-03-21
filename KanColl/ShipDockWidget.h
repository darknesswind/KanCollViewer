#pragma once

#include <QWidget>
#include "ui_ShipDockWidget.h"
#include "GameCache.h"

class ShipDockWidget : public QWidget
{
	Q_OBJECT

public:
	ShipDockWidget(QWidget *parent = Q_NULLPTR);
	~ShipDockWidget();

	void init(GameCache* pCache);

signals:
	void shipSelected(const ShipGraphFile&);
	void shipActivated(const ShipGraphFile&);
	void hackFileSelected(bool b);

public slots:
	void onSelectChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void onItemActivated(QListWidgetItem *item);
	void onShipTypeFilter(int type);

	void exploreTo();
	void exportImage();
	void updateTime();

private:
	QString getCurrentFile();

private:
	Ui::ShipDockWidget m_ui;
	GameCache* m_pCache;
};
