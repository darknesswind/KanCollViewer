#pragma once
#include <QDir>

class QJsonObject;

struct ShipPosInfo
{
	QPoint boko_n;		// 母港
	QPoint boko_d;
	QPoint kaisyu_n;	// 改修
	QPoint kaisyu_d;
	QPoint kaizo_n;		// 改造
	QPoint kaizo_d;
	QPoint map_n;		// 海域 （-53,138）
	QPoint map_d;
	QPoint ensyuf_n;	// 演习
	QPoint ensyuf_d;
	QPoint ensyue_n;	// 演习敌方
	QPoint battle_n;	// 战斗
	QPoint battle_d;
	QPoint weda;		// 结婚？
	QPoint wedb;
};
struct ShipGraphFile
{
	enum Type
	{
		tOrigin,
		tHack,
		tLive2D
	};
	int id = 0;
	Type type = tOrigin;
	QString fileName;
	QString displayName;
	QString customName;
	ShipPosInfo pos;

	bool operator<(const ShipGraphFile& rhs);
};
typedef QList<ShipGraphFile> ShipGraphs;
struct ShipInfo
{
	// info
	int id = 0;
	QString name;
	int stype = 0;
	int beforeshipid = 0;
	int aftershipid = 0;	// 改造后id
	// graph
	QString filename;
	ShipPosInfo pos;
};
typedef QMap<int, ShipInfo> ShipInfos;
struct FurnitureInfo
{
	enum Type : int
	{
		tFloor,
		tWall,
		tWindow,
		tObject,
		tChest,
		tDesk,
	};
	Type type = tFloor;
	QString title;
	QString filename;
};

struct RoomImage
{
	QPixmap chest;
	QPixmap desk;
	QPixmap floor;
	QPixmap object;
	QPixmap wall;
	QPixmap window;
};

struct AlbumImage
{
	enum AlbumImageType
	{
		tDetialBg = 141,
		tTitle = 433,
		tStatus = 516,
	};
};

struct ShipImage
{
	enum ShipImageType
	{
		tTag = 1,
		tTagBroken = 3,
		tCard = 5,
		tCardBroken = 7,
		tAlbum = 9,
		tAlbumBroken = 11,
		tAlbumFull = 13,
		tAlbumFullBroken = 15,
		tStand = 17,
		tStandBroken = 19,
		tUnk = 21,
		tUnkBroken = 23,
		tAlbumTitle = 25,
		tSupply = 27,
		tSupplyBroken = 29,
		// enemy
		teTag = 1,
		teStand = 3,
		// book
		ta6Tag = 1,
		ta6TagBroken = 3,
		ta6Album = 5,
		ta6AlbumBroken = 7,
		ta6AlbumFull = 9,
		ta6AlbumFullBroken = 11,
		ta7AlbumTitle = 13,

		ta5Album = 1,
		ta5AlbumBroken = 3,
		ta5AlbumFull = 5,
		ta5AlbumFullBroken = 7,
		ta5AlbumTitle = 11,
	};
	typedef QMap<int, ImageShape> Images;

	Images images;
	ShipInfo* pInfo = nullptr;
	const ShipGraphFile* pFile = nullptr;
};

class GameCache
{
public:
	GameCache();
	~GameCache();

	void init();

	QDir cacheDir() { return m_cacheDir; }
	QDir shipsDir();
	QDir furnitureDir();

	const QStringList& shipTypes() { return m_shipType; }
	const ShipGraphs& shipGraphs() { return m_shipGraphs; }
	const ShipInfos& shipInfos() { return m_ships; }
	RoomImage& room() { return m_room; }
	const QPixmap& album() const { return m_album; }

	const ShipInfo& shipInfo(const QString& fileName);
	QString resolveShipName(const QString& fileName);
	int resolveShipId(const QString& fileName);

	ShipImage& loadShipImage(const ShipGraphFile& file);
protected:
	void detectCachePath();
	void loadGameData(const QString& dataPath);
	void collectShips();
	void collectRoom();
	void collectAlbum();

	void loadShipInfo(const QJsonObject& obj);
	void loadShipGraph(const QJsonObject& obj);
	void loadShipType(const QJsonObject& obj);
	void loadFurniture(const QJsonObject& obj);
	void loadFurnitureGraph(const QJsonObject& obj);
	QPoint loadPoint(const QJsonArray& obj);

private:
	QDir m_cacheDir;

	ShipInfos m_ships;
	QMap<int, FurnitureInfo> m_furnitures;
	QHash<QString, int> m_shipIdMap;
	QStringList m_shipType;
	ShipGraphs m_shipGraphs;

	RoomImage m_room;
	QPixmap m_album;
	QHash<QString, ShipImage> m_shipImgCache;
};

