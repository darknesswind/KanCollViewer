#include "stdafx.h"
#include "ShipDockWidget.h"
#include "GameCache.h"

class ShipFileItem : public QListWidgetItem
{
public:
	ShipFileItem(const ShipGraphFile& file)
		: QListWidgetItem(file.displayName)
		, m_file(file)
	{}

	int Id() const { return m_file.id; }
	const QString& fileName() const { return m_file.fileName; }
	const ShipGraphFile& file() const { return m_file; }
private:
	const ShipGraphFile& m_file;
};

ShipDockWidget::ShipDockWidget(QWidget *parent)
	: QWidget(parent)
	, m_pCache(nullptr)
{
	m_ui.setupUi(this);
}

ShipDockWidget::~ShipDockWidget()
{
}

void ShipDockWidget::init(GameCache* pCache)
{
	m_pCache = pCache;

	const QStringList& typeName = m_pCache->shipTypes();
	m_ui.cmbShipType->addItem(tr("No Filter"));
	m_ui.cmbShipType->addItem(tr("Other"));
	for (int i = 1; i < typeName.size(); ++i)
	{
		QString name = QString("%1 %2").arg(i).arg(typeName[i]);
		m_ui.cmbShipType->addItem(name);
	}
	connect(m_ui.cmbShipType, SIGNAL(currentIndexChanged(int)), this, SLOT(onShipTypeFilter(int)));

	int minWidth = 0;
	QFontMetrics font = m_ui.fileList->fontMetrics();
	const ShipGraphs& files = m_pCache->shipGraphs();
	for (auto iter = files.begin(); iter != files.end(); ++iter)
	{
		ShipFileItem* pItem = new ShipFileItem(*iter);
		pItem->setToolTip(iter->fileName);
		m_ui.fileList->addItem(pItem);
		minWidth = qMax(minWidth, font.width(pItem->text()));
	}
	m_ui.fileList->setMinimumWidth(minWidth + 25);
	connect(m_ui.fileList, &QListWidget::currentItemChanged, this, &ShipDockWidget::onSelectChanged);
	connect(m_ui.fileList, &QListWidget::itemActivated, this, &ShipDockWidget::onItemActivated);


	QAction* pActExplore = new QAction(tr("&Explore to"));
	m_ui.fileList->addAction(pActExplore);
	QAction* pActImage = new QAction(tr("Export &image"));
	m_ui.fileList->addAction(pActImage);
	QAction* pActUpdate = new QAction(tr("Update &time"));
	m_ui.fileList->addAction(pActUpdate);

	connect(pActExplore, &QAction::triggered, this, &ShipDockWidget::exploreTo);
	connect(pActImage, &QAction::triggered, this, &ShipDockWidget::exportImage);
	connect(pActUpdate, &QAction::triggered, this, &ShipDockWidget::updateTime);
	connect(this, &ShipDockWidget::hackFileSelected, pActUpdate, &QAction::setEnabled);
}

void ShipDockWidget::onSelectChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (!current) return;
	ShipFileItem* pItem = static_cast<ShipFileItem*>(current);

	bool bSelHack = (pItem->file().type == ShipGraphFile::tHack);
	emit hackFileSelected(bSelHack);
	emit shipSelected(pItem->file());
}

void ShipDockWidget::onItemActivated(QListWidgetItem *item)
{
	if (!item) return;
	ShipFileItem* pItem = static_cast<ShipFileItem*>(item);
	emit shipActivated(pItem->file());
}

void ShipDockWidget::onShipTypeFilter(int idx)
{
	if (idx < 0) return;

	int cnt = m_ui.fileList->count();
	for (int i = 0; i < cnt; ++i)
	{
		ShipFileItem* pItem = static_cast<ShipFileItem*>(m_ui.fileList->item(i));
		bool bVisible = true;
		if (0 == idx)
		{
			bVisible = true;
		}
		else
		{
			int stype = m_pCache->shipInfos()[pItem->Id()].stype;
			bVisible = (stype == (idx - 1));
		}
		pItem->setHidden(!bVisible);
	}
}

QString ShipDockWidget::getCurrentFile()
{
	ShipFileItem* pItem = static_cast<ShipFileItem*>(m_ui.fileList->currentItem());
	if (!pItem) return QString();

	return QString("%1/%2.swf").arg(m_pCache->shipsDir().path(), pItem->fileName());
}

void ShipDockWidget::exploreTo()
{
	QString filePath = getCurrentFile();
	if (filePath.isEmpty())
		return;

	QString arg = QString("/select, \"%1\"").arg(filePath.replace('/', '\\'));
	ShellExecuteW(0, L"open", L"explorer.exe", (WCHAR*)arg.utf16(), NULL, SW_SHOWNORMAL);
}

void ShipDockWidget::exportImage()
{
	ShipFileItem* pItem = static_cast<ShipFileItem*>(m_ui.fileList->currentItem());
	if (!pItem) return;

	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	QStringList files;
	if (dialog.exec())
		files = dialog.selectedFiles();
	
	if (files.empty())
		return;

	QString basePath = files[0];
	ShipImage& imgs = m_pCache->loadShipImage(pItem->file());
	for (auto iter = imgs.images.begin(); iter != imgs.images.end(); ++iter)
	{
		iter->img.save(QString("%1/%2.png").arg(basePath).arg(iter.key()));
	}
	ShellExecuteW(0, L"open", L"explorer.exe", (WCHAR*)basePath.replace('/', '\\').utf16(), NULL, SW_SHOWNORMAL);
}

void ShipDockWidget::updateTime()
{
	QString filePath = getCurrentFile();
	if (filePath.isEmpty())
		return;

	QFileInfo hackFile(filePath);
	QFileInfo oriFile(filePath.replace(".hack.", "."));

	if (!hackFile.exists() || !oriFile.exists())
		return;

	if (oriFile.lastModified() <= hackFile.lastModified())
		return;

	FILETIME fileTime = { 0 };
	SYSTEMTIME systime = { 0 };
	GetSystemTime(&systime);
	SystemTimeToFileTime(&systime, &fileTime);

	HANDLE hFile = CreateFileW((wchar_t*)hackFile.fileName().utf16(),
		GENERIC_WRITE, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	SetFileTime(hFile, NULL, NULL, &fileTime);
	CloseHandle(hFile);
}
