#include "stdafx.h"
#include "GameCache.h"
#include <TlHelp32.h>
#include <QFileInfo>
#include <QJsonDocument>
#include "SWFDecoder.h"

GameCache::GameCache()
{
}


GameCache::~GameCache()
{
}

void GameCache::init()
{
	detectCachePath();
	collectShips();
	collectRoom();
	collectAlbum();
}

void GameCache::detectCachePath()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return;
	}

	QString fullName;
	PROCESSENTRY32W entry = { sizeof(PROCESSENTRY32W) };
	for (BOOL bSucc = Process32FirstW(hSnapshot, &entry);
		bSucc; bSucc = Process32NextW(hSnapshot, &entry))
	{
		QString processName = QString::fromUtf16((ushort*)entry.szExeFile).toLower();
		if (processName == QString("shimakazego.exe"))
		{
			CloseHandle(hSnapshot);
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, entry.th32ProcessID);
			DWORD size = MAX_PATH;
			WCHAR szFullName[MAX_PATH];
			QueryFullProcessImageNameW(hProcess, 0, szFullName, &size);
			CloseHandle(hProcess);
			fullName = QString::fromUtf16((ushort*)szFullName);
			break;
		}
	}
	if (fullName.isEmpty())
		return;

	QFileInfo fileInfo(fullName);
	m_cacheDir = fileInfo.dir();
	m_cacheDir.cd("cache/kcs");

	loadGameData(fileInfo.absolutePath() + "/data/KanColle/start_info.json");
}

void GameCache::loadGameData(const QString& dataPath)
{
	QFile file(dataPath);
	if (!file.open(QFile::ReadOnly))
		return;

	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	QJsonObject  objData = doc.object()["api_data"].toObject();

	QJsonArray arrShips = objData["api_mst_ship"].toArray();
	//for (auto iter = arrShips.begin(); iter != arrShips.end(); ++iter)

	for each (auto val in objData["api_mst_ship"].toArray())
		loadShipInfo(val.toObject());

	for each (auto val in objData["api_mst_shipgraph"].toArray())
		loadShipGraph(val.toObject());

	for each (auto val in objData["api_mst_stype"].toArray())
		loadShipType(val.toObject());

	for each (auto val in objData["api_mst_furniture"].toArray())
		loadFurniture(val.toObject());

	for each (auto val in objData["api_mst_furnituregraph"].toArray())
		loadFurnitureGraph(val.toObject());

	for (auto iter = m_ships.begin(); iter != m_ships.end(); ++iter)
		m_shipIdMap[iter->filename] = iter.key();
}

#define LoadInt(attr) info.attr = obj["api_" #attr].toInt();
#define LoadStr(attr) info.attr = obj["api_" #attr].toString();
#define LoadPoint(attr) info.pos.attr = loadPoint(obj["api_" #attr].toArray());
#define LoadPoints(attr)\
{\
	info.pos.attr##_n = loadPoint(obj["api_" #attr "_n"].toArray());\
	info.pos.attr##_d = loadPoint(obj["api_" #attr "_d"].toArray());\
}

void GameCache::loadShipInfo(const QJsonObject& obj)
{
	if (obj.empty())
		return;

	int id = obj["api_id"].toInt();
	ShipInfo& info = m_ships[id];
	info.id = id;
	LoadStr(name);
	LoadInt(stype);
	LoadInt(aftershipid);
	info.beforeshipid = 0;
	if (info.aftershipid)
	{
		ShipInfo& afterShip = m_ships[info.aftershipid];
		if (0 == afterShip.beforeshipid)
			afterShip.beforeshipid = id;
	}
}

void GameCache::loadShipGraph(const QJsonObject& obj)
{
	if (obj.empty())
		return;

	int id = obj["api_id"].toInt();
	ShipInfo& info = m_ships[id];
	LoadStr(filename);
	LoadPoints(boko);
	LoadPoints(kaisyu);
	LoadPoints(kaizo);
	LoadPoints(map);
	LoadPoints(ensyuf);
	LoadPoint(ensyue_n);
	LoadPoints(battle);
	LoadPoint(weda);
	LoadPoint(wedb);
}

void GameCache::loadShipType(const QJsonObject& obj)
{
	if (obj.empty())
		return;

	int id = obj["api_id"].toInt();
	while (id >= m_shipType.size())
		m_shipType.push_back(QString());

	m_shipType[id] = obj["api_name"].toString();
}

void GameCache::loadFurniture(const QJsonObject& obj)
{
	if (obj.empty())
		return;

	int id = obj["api_id"].toInt();
	FurnitureInfo& info = m_furnitures[id];
	info.type = (FurnitureInfo::Type)obj["api_type"].toInt();
	LoadStr(title);
}

void GameCache::loadFurnitureGraph(const QJsonObject& obj)
{
	if (obj.empty())
		return;

	int id = obj["api_id"].toInt();
	FurnitureInfo& info = m_furnitures[id];

	LoadStr(filename);
}

QPoint GameCache::loadPoint(const QJsonArray& obj)
{
	QPoint res;
	if (obj.size() == 2)
		res = QPoint(obj[0].toInt(), obj[1].toInt());
	return res;
}

void GameCache::collectShips()
{
	QDir dir = shipsDir();
	QStringList filter("*.swf");
	QFileInfoList infos = dir.entryInfoList(filter, QDir::Files | QDir::NoDotAndDotDot);
	for (auto iter = infos.begin(); iter != infos.end(); ++iter)
	{
		m_shipGraphs.push_back(ShipGraphFile());
		
		ShipGraphFile& ship = m_shipGraphs.back();
		ship.fileName = iter->completeBaseName();
		ship.type = ShipGraphFile::tOrigin;
		ship.id = resolveShipId(iter->baseName());
		ship.displayName = resolveShipName(iter->baseName());
		ship.pos = m_ships[ship.id].pos;

		QStringList baseName = iter->completeBaseName().split('.');
		if (baseName.size() > 1)
		{
			QString typestr = baseName[1].toLower();
			if (typestr == "2d")
			{
				ship.type = ShipGraphFile::tLive2D;
				ship.displayName.append(".2d");
			}
			else if (typestr == "hack")
			{
				ship.type = ShipGraphFile::tHack;
				ship.displayName.append(".hack");
			}
		}
		if (ship.type == ShipGraphFile::tHack)
		{
			QString config = QString("%1.config.ini");
			if (QFile(config).exists())
			{
				QSettings setting(config, QSettings::IniFormat);
				auto loadInt = [&](const QString& key, int& val)
				{
					QVariant var = setting.value(key);
					if (!var.isNull()) val = var.toInt();
				};
				setting.beginGroup("info");
				ship.customName = setting.value("ship_name").toString();
				setting.endGroup();
#define LoadCustomPoint(attr) loadInt(#attr "_left", ship.pos.attr.rx()); loadInt(#attr "_top", ship.pos.attr.ry());
				setting.beginGroup("graph");
				LoadCustomPoint(boko_n);
				LoadCustomPoint(boko_n);
				LoadCustomPoint(boko_d);
				LoadCustomPoint(kaisyu_n);
				LoadCustomPoint(kaisyu_d);
				LoadCustomPoint(kaizo_n);
				LoadCustomPoint(kaizo_d);
				LoadCustomPoint(map_n);
				LoadCustomPoint(map_d);
				LoadCustomPoint(ensyuf_n);
				LoadCustomPoint(ensyuf_d);
				LoadCustomPoint(ensyue_n);
				LoadCustomPoint(battle_n);
				LoadCustomPoint(battle_d);
				LoadCustomPoint(weda);
				LoadCustomPoint(wedb);
				setting.endGroup();
#undef LoadCustomPoint
			}
		}
	}
	std::sort(m_shipGraphs.begin(), m_shipGraphs.end());
}

void GameCache::collectRoom()
{
	QRegExp nameExp("\\d{3}.png");
	auto loadDefPixmap = [&](const QString& subDir)
	{
		QDir dir = furnitureDir();
		dir.cd(subDir);
		QFileInfoList infos = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
		for (auto iter = infos.begin(); iter != infos.end(); ++iter)
		{
			if (nameExp.exactMatch(iter->fileName()));
				return QPixmap(iter->filePath());
		}
		return QPixmap();
	};
	m_room.chest = loadDefPixmap("chest");
	m_room.desk = loadDefPixmap("desk");
	m_room.floor = loadDefPixmap("floor");
	m_room.object = loadDefPixmap("object");
	m_room.wall = loadDefPixmap("wall");
	m_room.window = loadDefPixmap("window");
}

void GameCache::collectAlbum()
{
	SWFDecoder swf(SWFDecoder::modeImage);
	swf.imageFilter() =
	{
		AlbumImage::tDetialBg,
		AlbumImage::tTitle,
		AlbumImage::tStatus
	};

	QString file = m_cacheDir.filePath("./scenes/AlbumMain.swf");
	if (!swf.open(file))
		return;

	const static int s_width = 800;
	const static int s_height = 480;
	m_album = QPixmap(s_width, s_height);
	m_album.fill(Qt::black);
	
	QPainter painter(&m_album);
	painter.drawImage(0, 0, swf.images()[AlbumImage::tDetialBg].img);
	painter.drawImage(26, 30, swf.images()[AlbumImage::tTitle].img);
	painter.drawImage(71, 236, swf.images()[AlbumImage::tStatus].img);
}

bool ShipGraphFile::operator<(const ShipGraphFile& rhs)
{
	if (id == rhs.id)
	{
		if (type == rhs.type)
			return fileName < rhs.fileName;
		else
			return type < rhs.type;
	}
	else
	{
		return id < rhs.id;
	}
}

QDir GameCache::shipsDir()
{
	QDir ships = m_cacheDir;
	ships.cd("resources/swf/ships");
	return ships;
}

QDir GameCache::furnitureDir()
{
	QDir furniture = m_cacheDir;
	furniture.cd("resources/image/furniture");
	return furniture;
}

const ShipInfo& GameCache::shipInfo(const QString& fileName)
{
	auto iter = m_shipIdMap.find(fileName);
	if (iter == m_shipIdMap.end())
		return m_ships[0];

	return m_ships[iter.value()];
}

QString GameCache::resolveShipName(const QString& fileName)
{
	auto iter = m_shipIdMap.find(fileName);
	if (iter == m_shipIdMap.end())
		return fileName;

	const ShipInfo& info = m_ships[iter.value()];
	if (info.name.isEmpty())
		return fileName;

	return info.name;
}

int GameCache::resolveShipId(const QString& fileName)
{
	auto iter = m_shipIdMap.find(fileName);
	if (iter == m_shipIdMap.end())
		return 0;

	return iter.value();
}

ShipImage& GameCache::loadShipImage(const ShipGraphFile& file)
{
	if (m_shipImgCache.contains(file.fileName))
		return m_shipImgCache[file.fileName];

	QDir dir = shipsDir();
	QString filePath = QString("%1/%2.swf").arg(dir.path(), file.fileName);

	SWFDecoder decoder(SWFDecoder::modeShape);
	decoder.open(filePath);

	ShipImage& ship = m_shipImgCache[file.fileName];
	ship.images = decoder.images();

	ship.pInfo = &m_ships[file.id];
	ship.pFile = &file;
	return ship;
}
