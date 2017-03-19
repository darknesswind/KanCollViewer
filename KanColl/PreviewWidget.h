#pragma once

#include <QWidget>
#include "GameCache.h"

class GameCache;
class PreviewWidget : public QWidget
{
	Q_OBJECT

public:
	enum PreviewMode
	{
		pvHome,
		pvAlbum,
	};
	PreviewWidget(QWidget *parent);
	~PreviewWidget();
	void init(GameCache* pCache);
	void changeShipImage(const ShipGraphFile& file);
	void updateRoom();

	virtual QSize sizeHint() const override { return QSize(s_width, s_height); }
	virtual void paintEvent(QPaintEvent *e) override;

	void switchToHome(bool bActive);
	void switchToAlbum(bool bActive);

private:
	void drawRoom(QPainter& painter);
	void drawAlbum(QPainter& painter);

private:
	const static int s_width = 800;
	const static int s_height = 480;

private:
	QPixmap m_room;
	QPixmap m_album;
	GameCache* m_pCache;
	ShipImage m_curShip;
	PreviewMode m_mode;
	int m_standImgIdx;
	int m_albumImgIdx;
};
