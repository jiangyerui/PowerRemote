#include "sqlmanager.h"


QMutex SqlManager::mutex;
QWaitCondition SqlManager::waitConnection;
SqlManager* SqlManager::instance = NULL;
SqlManager::SqlManager()
{
    dataBaseName = "./RemoteClient.db";
    dataBaseType = "QSQLITE";
    testOnBorrow = true;

    maxWaitTime  = 1000;
    waitInterval = 200;
    maxConntCount = 10;
}

QSqlDatabase SqlManager::createConnection(const QString &connectionName)
{
    // 连接已经创建过了,复用它,而不是重新创建
    if(QSqlDatabase::contains(connectionName))
    {
        QSqlDatabase dbPre = QSqlDatabase::database(connectionName);

        if(testOnBorrow)
        {
            //返回连接前访问数据库，如果连接断开，重新建立连接
            //qDebug() << "Test connection on borrow, execute:" << testOnBorrowSql << ", for" << connectionName;
            QSqlQuery query(dbPre);
            if (query.lastError().type() != QSqlError::NoError && !dbPre.open())
            {
                qDebug() << "Open datatabase error:" << dbPre.lastError().text();
                return QSqlDatabase();
            }
        }
        return dbPre;
    }

    // 创建一个新的连接
    QSqlDatabase db = QSqlDatabase::addDatabase(dataBaseType, connectionName);
    db.setDatabaseName(dataBaseName);
    if (!db.open())
    {
        qDebug() << "Open datatabase error:" << db.lastError().text();
        return QSqlDatabase();
    }

    return db;
}

void SqlManager::release()
{
    QMutexLocker locker(&mutex);
    delete instance;
    instance = NULL;
}

QSqlDatabase SqlManager::openConnection()
{
    SqlManager& pool = SqlManager::getInstance();
    QMutexLocker locker(&mutex);

    // 已创建连接数
    int conntCount = pool.unusedConntNames.size() + pool.usedConntNames.size();

    // 如果连接已经用完,等待 waitInterval 毫秒看看是否有可用连接,最长等待 maxWaitTime 毫秒
    for(int i=0;i<pool.maxWaitTime&&pool.unusedConntNames.size()==0&&conntCount==pool.maxConntCount;i+=pool.waitInterval)
    {
        waitConnection.wait(&mutex,pool.waitInterval);
        // 重新计算已创建连接数
        conntCount = pool.unusedConntNames.size() + pool.usedConntNames.size();
    }
    QString connectionName;
    if(pool.unusedConntNames.size() > 0)
    {
        // 有已经回收的连接，复用它们
        connectionName = pool.unusedConntNames.dequeue();
    }
    else if(conntCount < pool.maxConntCount)
    {
        // 没有已经回收的连接，但是没有达到最大连接数，则创建新的连接
        connectionName = QString("Connection-%1").arg(conntCount + 1);
    }
    else
    {
        // 已经达到最大连接数
        qDebug() << "Cannot create more connections.";
        return QSqlDatabase();
    }

    // 创建连接
    QSqlDatabase db = pool.createConnection(connectionName);

    // 有效的连接才放入 usedConnectionNames
    if (db.isOpen())
    {
        pool.usedConntNames.enqueue(connectionName);
    }

    return db;
}

void SqlManager::closeConnection(QSqlDatabase connection)
{
    SqlManager& pool = SqlManager::getInstance();
    QString connectionName = connection.connectionName();

    // 如果是我们创建的连接，从 used 里删除,放入 unused 里
    if (pool.usedConntNames.contains(connectionName))
    {
        QMutexLocker locker(&mutex);
        pool.usedConntNames.removeOne(connectionName);
        pool.unusedConntNames.enqueue(connectionName);
        waitConnection.wakeOne();
    }
}

SqlManager::~SqlManager()
{
    // 销毁连接池的时候删除所有的连接
    foreach(QString connectionName, usedConntNames)
    {
        QSqlDatabase::removeDatabase(connectionName);
    }

    foreach(QString connectionName, unusedConntNames)
    {
        QSqlDatabase::removeDatabase(connectionName);
    }
}

bool SqlManager::insertAlarmRecord(QSqlDatabase db, QString host,uint pass,uint canId,uint nodeType,uint nodeSts,uint alarmValue,uint alarmTime)
{

    QString passStr       = QString::number(pass);
    QString canIdStr      = QString::number(canId);
    QString nodeTypeStr   = QString::number(nodeType);
    QString nodeStsStr    = QString::number(nodeSts);
    QString alarmValueStr = QString::number(alarmValue);
    QString alarmTimeStr  = QString::number(alarmTime);
    //QString nodeAddress;
    QString sql = "insert into RECORD values(";

    sql += "'"+host+"',"+passStr+","+canIdStr+","+nodeTypeStr+","+nodeStsStr+","+alarmValueStr+","+alarmTimeStr+");";
    QSqlQuery query(db);
    if(!query.exec(sql))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

SqlManager &SqlManager::getInstance()
{
    if (NULL == instance)
    {
        QMutexLocker locker(&mutex);

        if (NULL == instance)
        {
            instance = new SqlManager();
        }
    }
    return *instance;
}

bool SqlManager::insertHostList(QSqlDatabase db, QString host, QString port, QString able, QString address)
{
    QString sqlQuery = "insert into HOSTINFO values('"+host+"',"+port+","+able+",'"+address+"');";

    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

bool SqlManager::delelteHostItem(QSqlDatabase db, QString host, QString port)
{
    QString sqlQuery = "delete from HOSTINFO where PASS = '"+host+"' and CANID = "+port+";";

    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();

    return true;
}

bool SqlManager::delAllData(QSqlDatabase db,const QString &sqlQuery)
{

    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();

    return true;
}

bool SqlManager::delelteRecordItem(QSqlDatabase db, uint pass, uint canId, uint nodeType, uint nodeSts, uint alarmTime)
{
    QString passStr = QString::number(pass);
    QString canIdStr = QString::number(canId);
    QString nodeTypeStr = QString::number(nodeType);
    QString nodeStsStr = QString::number(nodeSts);
    QString alarmTimeStr = QString::number(alarmTime);
    QString sqlQuery = "delete from RECORD where PASS = "+passStr+" and CANID = "+canIdStr+" "
                       "and NODETYPE = "+nodeTypeStr+" "
                       "and NODESTS = "+nodeStsStr+" "
                       "and ALARMTIME = "+alarmTimeStr+";";

    QSqlQuery query(db);
    if(!query.exec(sqlQuery))
    {
        return false;
    }
    query.finish();
    query.clear();

    return true;
}

QList<QStringList> SqlManager::getHostList(QSqlDatabase db, const QString &sqlQuery, ListMode listMode)
{
    QList<QStringList> list;list.clear();

    QSqlQuery query(db);
    if(query.exec(sqlQuery))
    {
        while(query.next())
        {
            QStringList nodeList;
            for(int index = 0;index < listMode;index++)
            {
                nodeList.append(query.value(index).toString());
            }
            list.append(nodeList);
        }
    }
    query.finish();
    query.clear();

    return list;
}
