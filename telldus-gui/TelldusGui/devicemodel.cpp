#include "devicemodel.h"
#include <telldus-core.h>
#include <QQueue>
#include <QIcon>
#include <QDebug>

DeviceModel::DeviceModel(QObject *parent)
		:QAbstractTableModel(parent)
{
}


DeviceModel::~DeviceModel()
{
}

int DeviceModel::rowCount(const QModelIndex &) const {
	return tdGetNumberOfDevices();
}

int DeviceModel::columnCount(const QModelIndex &) const {
	return 3;
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const {
	if (index.column() > 2) {
		return QVariant();
	}

	if (index.column() == 0) {
		if (role == Qt::DisplayRole) {
			Device *device = this->device( index );
			switch( device->lastSentCommand() ) {
				case TELLSTICK_TURNON:
					return tr("on");
				case TELLSTICK_TURNOFF:
					return tr("off");
				case TELLSTICK_DIM:
					return tr("dimmed");
			}
			return tr("unknown %1").arg(device->lastSentCommand());
		} else if (role == Qt::DecorationRole) {
			Device *device = this->device( index );
			int lastSentCommand = device->lastSentCommand();
			return QIcon( QString(":/images/state_%1.png").arg(lastSentCommand) );
		} else if (role == Qt::TextAlignmentRole) {
			return Qt::AlignCenter;
		}
	} else if (index.column() == 1) {
		if (role == Qt::DisplayRole) {
			Device *device = this->device( index );
			return device->name();
		}
	} else if (index.column() == 2) {
//		if (role == Qt::DisplayRole) {
//			Device *device = this->device( index );
//			return device->methods();
//		}
	}

	return QVariant();
}

/*Qt::ItemFlags DeviceModel::flags ( const QModelIndex & index ) const {
	if (!index.isValid()) {
		 return Qt::ItemIsEnabled;
	}

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}*/

QVariant DeviceModel::headerData ( int section, Qt::Orientation orientation, int role ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}

	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	switch (section) {
		case 0:
			return tr("State");
		case 1:
			return tr("Device name");
		case 2:
			return tr("Action");
	}

	return QVariant();
}

bool DeviceModel::removeRows ( int row, int count, const QModelIndex & parent ) {
	beginRemoveRows( parent, row, row + count - 1 );

	QQueue<int> idList;
	for ( int i = row; i < row + count; ++i ) {
		idList.enqueue( deviceId(i) );
	}

	while(!idList.isEmpty()) {
		tdRemoveDevice( idList.dequeue() );
	}
	indexToId.clear(); //Clear the index-to-id cache

	endRemoveRows();
	return true;
}

Device *DeviceModel::device( const QModelIndex &index ) const {
	int id = deviceId( index );
	bool loaded = Device::deviceLoaded( id );
	Device *device = Device::getDevice( id );
	if (!loaded) {
		connect(device, SIGNAL(showMessage(const QString &, const QString &, const QString &)), this, SIGNAL(showMessage(const QString &, const QString &, const QString &)));
		connect(device, SIGNAL(eventTriggered(const QString &, const QString &)), this, SIGNAL(eventTriggered(const QString &, const QString &)));
	}
	return device;
}

Device *DeviceModel::newDevice() const {
	Device *device = Device::newDevice();
	connect(device, SIGNAL(deviceAdded(int)), this, SLOT(deviceAdded(int)));
	connect(device, SIGNAL(showMessage(const QString &, const QString &, const QString &)), this, SIGNAL(showMessage(const QString &, const QString &, const QString &)));
	connect(device, SIGNAL(eventTriggered(const QString &, const QString &)), this, SIGNAL(eventTriggered(const QString &, const QString &)));
	return device;
}

void DeviceModel::deviceAdded( int id ) {
	Q_UNUSED(id);
	int deviceCount = tdGetNumberOfDevices();
	beginInsertRows( QModelIndex(), deviceCount, deviceCount );
	endInsertRows();
}

int DeviceModel::deviceId( const QModelIndex &index ) const {
	return deviceId( index.row() );
}

int DeviceModel::deviceId( int index ) const {
	if (indexToId.contains(index)) {
		return indexToId[index];
	}
	int id = tdGetDeviceId( index );
	indexToId[index] = id;
	return id;
}

void DeviceModel::deviceEvent(int /*deviceId*/, int /*method*/, const char */*data*/, int /*callbackId*/, void */*context*/) {
	//TODO:
}
