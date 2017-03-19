#pragma once

#include <QWidget>
#include "GameCache.h"

class GameCache;
class PreviewWidget : public QWidget
{
	Q_OBJECT

public:
	PreviewWidget(QWidget *parent);
	~PreviewWidget();
	void init(GameCache* pCache);
	void changeShipImage(const ShipGraphFile& file);
	void updateRoom();

	virtual QSize sizeHint() const override { return QSize(s_width, s_height); }
	virtual void paintEvent(QPaintEvent *e) override;

private:
	void drawRoom(QPainter& painter);

private:
	const static int s_width = 800;
	const static int s_height = 480;

private:
	QPixmap m_room;
	GameCache* m_pCache;
	ShipImage m_curShip;
};
