#include "stdafx.h"
#include "PreviewWidget.h"

PreviewWidget::PreviewWidget(QWidget *parent)
	: QWidget(parent)
	, m_pCache(nullptr)
	, m_preview(s_width, s_height)
	, m_room(s_width, s_height)
	, m_defaultMode(pvHome)
	, m_displayMode(pvHome)
	, m_standImgIdx(0)
	, m_albumImgIdx(0)
	, m_bPreviewDirty(true)
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

void PreviewWidget::savePreview()
{
	static QDir s_defaultSaveDir = QDir::current();

	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save Preview"));
	dialog.setDirectory(s_defaultSaveDir);
	dialog.setNameFilter("*.png");

	QStringList files;
	if (dialog.exec())
		files = dialog.selectedFiles();
	if (files.empty())
		return;

	m_preview.save(files[0], "png");
	s_defaultSaveDir = dialog.directory();
}

void PreviewWidget::changeShipImage(const ShipGraphFile& file)
{
	m_curShip = m_pCache->loadShipImage(file);
	setDirty(true);
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

	setDirty(true);
}

void PreviewWidget::paintEvent(QPaintEvent *e)
{
	updatePreview();

	QPainter painter(this);
	painter.setClipRegion(e->region());
	painter.drawPixmap(0, 0, m_preview);
	e->accept();
}

void PreviewWidget::switchToHome(bool bActive)
{
	if (bActive && m_defaultMode != pvHome)
	{
		m_defaultMode = pvHome;
		setDirty(true);
	}
}

void PreviewWidget::switchToAlbum(bool bActive)
{
	if (bActive && m_defaultMode != pvAlbum)
	{
		m_defaultMode = pvAlbum;
		setDirty(true);
	}
}

void PreviewWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		switch (m_displayMode)
		{
		case PreviewWidget::pvHome:
			++m_standImgIdx;
			break;
		case PreviewWidget::pvAlbum:
			++m_albumImgIdx;
			break;
		default:
			return;
		}
		setDirty(true);
	}
}

void PreviewWidget::updatePreview()
{
	if (!m_bPreviewDirty)
		return;

	m_bPreviewDirty = false;
	m_preview.fill(Qt::black);
	QPainter painter(&m_preview);
	switch (m_defaultMode)
	{
	case PreviewWidget::pvHome:
		drawRoom(painter);
		break;
	case PreviewWidget::pvAlbum:
		drawAlbum(painter);
		break;
	default:
		break;
	}
}

void PreviewWidget::drawRoom(QPainter& painter)
{
	const static QPoint s_defStandPos(328, -62);
	const static ShipImage::ShipImageType standImg[2] =
	{
		ShipImage::tStand, ShipImage::tStandBroken
	};

	m_displayMode = pvHome;
	painter.drawPixmap(0, 0, m_room);
	if (m_curShip.images.isEmpty())
		return;

	QPoint pos;
	int imgCount = m_curShip.images.size();
	if (imgCount >= 15)	// ÆÕÍ¨
	{
		if (m_curShip.pFile)
			pos = s_defStandPos + m_curShip.pFile->pos.boko_n;

		m_standImgIdx = m_standImgIdx % ARRAYSIZE(standImg);
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
	const static QPoint s_defTitlePos(26, 30);
	const static QPoint s_defAlbumPos(470, 25);
	const static ShipImage::ShipImageType albumImg[][4] =
	{
		{ ShipImage::tAlbum, ShipImage::tAlbumBroken, ShipImage::tAlbumFull, ShipImage::tAlbumFullBroken },
		{ ShipImage::ta5Album, ShipImage::ta5AlbumBroken, ShipImage::ta5AlbumFull, ShipImage::ta5AlbumFullBroken },
		{ ShipImage::ta6Album, ShipImage::ta6AlbumBroken, ShipImage::ta6AlbumFull, ShipImage::ta6AlbumFullBroken }
	};

	m_displayMode = pvAlbum;
	painter.drawPixmap(0, 0, m_album);
	if (m_curShip.images.isEmpty())
		return;

	ShipImage::ShipImageType id = (ShipImage::ShipImageType)m_curShip.images.firstKey();
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
		target.moveTo(s_defTitlePos);
		painter.drawImage(target, shape.img);
	}
	//////////////////////////////////////////////////////////////////////////
	id = (ShipImage::ShipImageType)m_curShip.images.firstKey();
	m_albumImgIdx = m_albumImgIdx % ARRAYSIZE(albumImg);
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
		target.moveTo(s_defAlbumPos);
		painter.drawImage(target, shape.img);
	}
}

void PreviewWidget::setDirty(bool bRepaint)
{
	m_bPreviewDirty = true;
	if (bRepaint)
		repaint();
}

