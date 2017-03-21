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
		pvManoeuvre,
	};
	PreviewWidget(QWidget *parent);
	~PreviewWidget();
	void init(GameCache* pCache);

	virtual QSize sizeHint() const override { return QSize(s_width, s_height); }
	virtual void paintEvent(QPaintEvent *e) override;

signals:
	//
public slots:
	void savePreview();
	void changeShipImage(const ShipGraphFile& file);
	void updateRoom();
	void switchToHome(bool bActive);
	void switchToAlbum(bool bActive);

protected:
	void mouseReleaseEvent(QMouseEvent *event) override;
	void updatePreview();
	void drawRoom(QPainter& painter);
	void drawAlbum(QPainter& painter);
	void setDirty(bool bRepaint);

private:
	const static int s_width = 800;
	const static int s_height = 480;

private:
	QPixmap m_preview;
	QPixmap m_room;
	QPixmap m_album;

	GameCache* m_pCache;
	ShipImage m_curShip;

	PreviewMode m_defaultMode;
	PreviewMode m_displayMode;
	int m_standImgIdx;
	int m_albumImgIdx;

	bool m_bPreviewDirty;
};
