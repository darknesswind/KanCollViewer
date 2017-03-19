#include "stdafx.h"
#include "PreviewWidget.h"

PreviewWidget::PreviewWidget(QWidget *parent)
	: QWidget(parent)
	, m_pCache(nullptr)
	, m_room(s_width, s_height)
{
	setFixedSize(s_width, s_height);
}

PreviewWidget::~PreviewWidget()
{
}

void PreviewWidget::init(GameCache* pCache)
{
	m_pCache = pCache;
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

	drawRoom(painter);

	e->accept();
}

void PreviewWidget::drawRoom(QPainter& painter)
{
	painter.drawPixmap(0, 0, m_room);

	if (m_curShip.images.isEmpty())
		return;

	static QPoint defaultPos(328, -62);

	QPoint pos;
	int imgCount = m_curShip.images.size();
	if (imgCount == 15)	// ÆÕÍ¨
	{
		if (m_curShip.pFile)
			pos = defaultPos + m_curShip.pFile->pos.boko_n;
		painter.drawImage(pos, m_curShip.images[ShipImage::tStand]);
	}
	else if (m_curShip.images.size() == 2) // µÐÈË
	{
		if (m_curShip.pFile)
			pos += m_curShip.pFile->pos.battle_n;
		painter.drawImage(pos, m_curShip.images[ShipImage::teStand]);
	}
	else	// Í¼¼ø
	{
		ShipImage::ImageType id = (ShipImage::ImageType)1;
		switch (imgCount)
		{
		case 5: id = ShipImage::tb5Book; break;
		case 6: id = ShipImage::tb6Book; break;
		case 7: id = ShipImage::tb6Book; break;
		default:
			return;
		}

// 		if (m_curShip.pInfo)
// 			pos = defaultPos + m_curShip.pInfo->boko_n;
		painter.drawImage(pos, m_curShip.images[ShipImage::tb6Book]);
	}
}

