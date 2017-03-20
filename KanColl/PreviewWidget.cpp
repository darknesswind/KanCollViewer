#include "stdafx.h"
#include "PreviewWidget.h"

PreviewWidget::PreviewWidget(QWidget *parent)
	: QWidget(parent)
	, m_pCache(nullptr)
	, m_room(s_width, s_height)
	, m_mode(pvHome)
	, m_standImgIdx(0)
	, m_albumImgIdx(0)
{
	setFixedSize(s_width, s_height);
}

PreviewWidget::~PreviewWidget()
{
}

void PreviewWidget::init(GameCache* pCache)
{
	m_pCache = pCache;
	m_album = pCache->album();
}

void PreviewWidget::changeShipImage(const ShipGraphFile& file)
{
	m_curShip = m_pCache->loadShipImage(file);
	repaint();
}

void PreviewWidget::updateRoom()
{
	QPainter painter(&m_room);

	const RoomImage& room = m_pCache->room();
	painter.drawPixmap(0, s_height - room.floor.height(), room.floor);
	painter.drawPixmap(0, 0, room.wall);
	painter.drawPixmap(0, 0, room.object);
	painter.drawPixmap(196, 0, room.window);
	painter.drawPixmap(s_width - room.chest.width(), 0, room.chest);
	painter.drawPixmap(0, s_height - room.desk.height(), room.desk);
}

void PreviewWidget::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.setClipRegion(e->region());

	if (m_mode == pvHome)
		drawRoom(painter);
	else if (m_mode == pvAlbum)
		drawAlbum(painter);

	e->accept();
}

void PreviewWidget::switchToHome(bool bActive)
{
	if (bActive && m_mode != pvHome)
	{
		m_mode = pvHome;
		repaint();
	}
}

void PreviewWidget::switchToAlbum(bool bActive)
{
	if (bActive && m_mode != pvAlbum)
	{
		m_mode = pvAlbum;
		repaint();
	}
}

void PreviewWidget::drawRoom(QPainter& painter)
{
	painter.drawPixmap(0, 0, m_room);

	if (m_curShip.images.isEmpty())
		return;

	static QPoint defaultPos(328, -62);
	const static ShipImage::ShipImageType standImg[2] =
	{
		ShipImage::tStand, ShipImage::tStandBroken
	};

	QPoint pos;
	int imgCount = m_curShip.images.size();
	if (imgCount >= 15)	// ÆÕÍ¨
	{
		if (m_curShip.pFile)
			pos = defaultPos + m_curShip.pFile->pos.boko_n;

		ImageShape& shape = m_curShip.images[standImg[m_standImgIdx]];
		QRectF target = shape.rect;
		target.moveTo(pos);
		painter.drawImage(target, shape.img);
	}
	else if (m_curShip.images.size() == 2) // µÐÈË
	{
		if (m_curShip.pFile)
			pos += m_curShip.pFile->pos.battle_n;

		ImageShape& shape = m_curShip.images[ShipImage::teStand];
		QRectF target = shape.rect;
		target.moveTo(pos);
		painter.drawImage(target, shape.img);
	}
	else	// Í¼¼ø
	{
		drawAlbum(painter);
	}
}

void PreviewWidget::drawAlbum(QPainter& painter)
{
	painter.drawPixmap(0, 0, m_album);
	if (m_curShip.images.isEmpty())
		return;

	ShipImage::ShipImageType id = (ShipImage::ShipImageType)0;
	int imgCount = m_curShip.images.size();
	if (imgCount >= 15)	// ÆÕÍ¨
	{
		id = ShipImage::tAlbumTitle;
	}
	else	// Í¼¼ø
	{
		switch (imgCount)
		{
		case 5: id = ShipImage::ta5AlbumTitle; break;
		case 7: id = ShipImage::ta7AlbumTitle; break;
		default:
			break;
		}
	}
	if (id > 0)
	{
		ImageShape& shape = m_curShip.images[id];
		QRectF target = shape.rect;
		target.moveTo(QPoint(26, 30));
		painter.drawImage(target, shape.img);
	}

	const static ShipImage::ShipImageType albumImg[][4] =
	{
		{ ShipImage::tAlbum, ShipImage::tAlbumBroken, ShipImage::tAlbumFull, ShipImage::tAlbumFullBroken },
		{ ShipImage::ta5Album, ShipImage::ta5AlbumBroken, ShipImage::ta5AlbumFull, ShipImage::ta5AlbumFullBroken },
		{ ShipImage::ta6Album, ShipImage::ta6AlbumBroken, ShipImage::ta6AlbumFull, ShipImage::ta6AlbumFullBroken }
	};
	id = (ShipImage::ShipImageType)0;
	if (imgCount >= 15)	// ÆÕÍ¨
	{
		id = albumImg[0][m_albumImgIdx];
	}
	else	// Í¼¼ø
	{
		switch (imgCount)
		{
		case 5: id = albumImg[1][m_albumImgIdx]; break;
		case 6:
		case 7: id = albumImg[2][m_albumImgIdx]; break;
		default:
			return;
		}
	}
	if (id > 0)
	{
		ImageShape& shape = m_curShip.images[id];
		QRectF target = shape.rect;
		target.moveTo(QPoint(470, 25));
		painter.drawImage(target, shape.img);
	}
}

