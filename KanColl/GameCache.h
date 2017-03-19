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
	QPoint map_n;		// 海域
	QPoint map_d;
	QPoint ensyuf_n;	// 演习
	QPoint ensyuf_d;
	QPoint ensyue_n;
	QPoint battle_n;	// 战斗
	QPoint battle_d;
	QPoint weda;
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

struct ShipImage
{
	enum ImageType
	{
		tTag = 1,
		tTagBroken = 3,
		tCard = 5,
		tCardBroken = 7,
		tBook = 9,
		tBookBroken = 11,
		tBookFull = 13,
		tBookFullBroken = 15,
		tStand = 17,
		tStandBroken = 19,
		tUnk = 21,
		tUnkBroken = 23,
		tBookName = 25,
		tSupply = 27,
		tSupplyBroken = 29,
		// enemy
		teTag = 1,
		teStand = 3,
		// book
		tb6Tag = 1,
		tb6TagBroken = 3,
		tb6Book = 5,
		tb6BookBroken = 7,
		tb6BookFull = 9,
		tb6BookFullBroken = 11,
		tb7BookName = 13,

		tb5Book = 1,
		tb5BookBroken = 3,
		tb5BookFull = 5,
		tb5BookFullBroken = 7,
		tb5BookName = 11,
	};
	typedef QMap<int, QImage> Images;

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

	QDir shipsDir();
	QDir furnitureDir();

	const QStringList& shipTypes() { return m_shipType; }
	const ShipGraphs& shipGraphs() { return m_shipGraphs; }
	const ShipInfos& shipInfos() { return m_ships; }
	RoomImage& room() { return m_room; }

	const ShipInfo& shipInfo(const QString& fileName);
	QString resolveShipName(const QString& fileName);
	int resolveShipId(const QString& fileName);

	ShipImage& loadShipImage(const ShipGraphFile& file);
protected:
	void detectCachePath();
	void loadGameData(const QString& dataPath);
	void collectShips();
	void collectRoom();

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
	QHash<QString, ShipImage> m_shipImgCache;
};

