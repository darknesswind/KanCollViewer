#include "stdafx.h"
#include "KanColl.h"
#include "SWFDecoder.h"

KanColl::KanColl(QWidget *parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);
	m_spCache = std::make_unique<GameCache>();
	m_spCache->init();
	
	initMenu();
	initShipDock();
	initPreview();
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

	connect(m_ui.actionExportSwf, SIGNAL(triggered()), this, SLOT(onExportSwf()));
}

void KanColl::initShipDock()
{
	m_ui.shipDockWidget->init(m_spCache.get());
	connect(m_ui.shipDockWidget, &ShipDockWidget::shipSelected, this, &KanColl::onShipSelected);
	connect(m_ui.shipDockWidget, &ShipDockWidget::shipActivated, this, &KanColl::onShipActivated);
}

void KanColl::initPreview()
{
	m_ui.preview->init(m_spCache.get());
	m_ui.preview->updateRoom();

	connect(m_ui.rbHome, &QRadioButton::toggled, m_ui.preview, &PreviewWidget::switchToHome);
	connect(m_ui.rbAlbum, &QRadioButton::toggled, m_ui.preview, &PreviewWidget::switchToAlbum);
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

void KanColl::onExportSwf()
{
	QStringList files;
	static QDir s_defaultOpenDir = m_spCache->cacheDir();
	{
		QFileDialog dialog;
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setAcceptMode(QFileDialog::AcceptOpen);
		dialog.setWindowTitle(tr("Select SWF File"));
		dialog.setNameFilter("*.swf");
		dialog.setDirectory(s_defaultOpenDir);
		if (dialog.exec())
			files = dialog.selectedFiles();
		if (files.empty())
			return;
		s_defaultOpenDir = dialog.directory();
	}


	SWFDecoder swf(SWFDecoder::modeImage);
	if (!swf.open(files[0]))
	{
		QMessageBox::warning(this, tr("Warning"), tr("Decode File \"%1\" Failed!").arg(files[0]));
		return;
	}

	static QDir s_defaultSaveDir = QDir::current();
	{
		QFileDialog dialog;
		dialog.setFileMode(QFileDialog::DirectoryOnly);
		dialog.setWindowTitle(tr("Select Save Directory"));
		dialog.setDirectory(s_defaultSaveDir);
		if (dialog.exec())
			files = dialog.selectedFiles();
		if (files.empty())
			return;
		s_defaultSaveDir = dialog.directory();
	}

	QString basePath = files[0];
	for (auto iter = swf.images().begin(); iter != swf.images().end(); ++iter)
	{
		iter->img.save(QString("%1/%2.png").arg(basePath).arg(iter.key()));
	}
	ShellExecuteW(0, L"open", L"explorer.exe", (WCHAR*)basePath.replace('/', '\\').utf16(), NULL, SW_SHOWNORMAL);
}

#pragma endregion
