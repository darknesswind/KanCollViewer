#include "stdafx.h"
#include "KanColl.h"

KanColl::KanColl(QWidget *parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);
	m_spCache = std::make_unique<GameCache>();
	m_spCache->init();
	m_ui.preview->init(m_spCache.get());
	
	initMenu();
	initShipDock();

	m_ui.preview->updateRoom();
}

KanColl::~KanColl()
{

}

void KanColl::initMenu()
{
	connect(m_ui.actionChange_Wall, SIGNAL(triggered()), this, SLOT(onChangeWall()));
	connect(m_ui.actionChange_Floor, SIGNAL(triggered()), this, SLOT(onChangeFloor()));
	connect(m_ui.actionChange_Desk, SIGNAL(triggered()), this, SLOT(onChangeDesk()));
	connect(m_ui.actionChange_Window, SIGNAL(triggered()), this, SLOT(onChangeWindow()));
	connect(m_ui.actionChange_Object, SIGNAL(triggered()), this, SLOT(onChangeObject()));
	connect(m_ui.actionChange_Chest, SIGNAL(triggered()), this, SLOT(onChangeChest()));
}

void KanColl::initShipDock()
{
	m_ui.shipDockWidget->init(m_spCache.get());
	connect(m_ui.shipDockWidget, &ShipDockWidget::shipSelected, this, &KanColl::onShipSelected);
	connect(m_ui.shipDockWidget, &ShipDockWidget::shipActivated, this, &KanColl::onShipActivated);
}

void KanColl::onShipSelected(const ShipGraphFile& file)
{
	m_ui.preview->changeShipImage(file);
}

void KanColl::onShipActivated(const ShipGraphFile& file)
{

}

#pragma region Menus

bool KanColl::changeFurniture(const QString& type, QPixmap& img)
{
	QDir dir = m_spCache->furnitureDir();
	dir.cd(type);

	QString file = QFileDialog::getOpenFileName(this, tr("Change Furniture"), dir.path(), "*.png");
	QPixmap newImg;
	if (!file.isEmpty())
		newImg = QPixmap(file);

	if (newImg.isNull())
		return false;

	img = newImg;
	m_ui.preview->updateRoom();
	return true;
}

void KanColl::onChangeWall()
{
	changeFurniture("wall", m_spCache->room().wall);
}
void KanColl::onChangeFloor()
{
	changeFurniture("floor", m_spCache->room().floor);
}
void KanColl::onChangeDesk()
{
	changeFurniture("desk", m_spCache->room().desk);
}
void KanColl::onChangeWindow()
{
	changeFurniture("window", m_spCache->room().window);
}
void KanColl::onChangeObject()
{
	changeFurniture("object", m_spCache->room().object);
}
void KanColl::onChangeChest()
{
	changeFurniture("chest", m_spCache->room().chest);
}

#pragma endregion
