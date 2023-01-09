#include <QtTest>
#include <QSqlQuery>
#include <QSqlError>

#define FILENAME "ParamsRepositoryTest.cfg"

#include "Settings.h"
#include "Repositorys/ParamsRepository.h"
#include "Errors/RepositoryErrors.h"
#include "HeightsMap/HeightsMap.h"
#include "Essensities/CanvasBL.h"
#include "common.h"

class ParamsRepositoryTest : public QObject
{
    Q_OBJECT

public:
    ParamsRepositoryTest();
    ~ParamsRepositoryTest();

private slots:
    void initTestCase();
    void init();

    void getParamsTest();
    void addParamsTest();
    void deleteParamsTest();
    void updateParamsTest();

    void cleanup();
    void cleanupTestCase();

    private:
    string m_schema;
    unique_ptr<QSqlDatabase> db;
    ParamsRepository sut;
};

ParamsRepositoryTest::ParamsRepositoryTest()
{
    QFile cfgDefaults(FILENAME);
    cfgDefaults.open(QIODevice::ReadOnly);
    Settings::setDefaults(cfgDefaults.readAll());

    m_schema = Settings::get(Settings::Schema, Settings::DataBase).toString().toStdString();
    db = make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QPSQL"));

    QString m_dbhost = Settings::get(Settings::DBHost, Settings::DataBase).toString();
    int m_dbport = Settings::get(Settings::DBPort, Settings::DataBase).toInt();
    QString m_dbname = Settings::get(Settings::DBName, Settings::DataBase).toString();
    QString m_dbuser = Settings::get(Settings::DBUser, Settings::DataBase).toString();
    QString m_dbpass = Settings::get(Settings::DBPass, Settings::DataBase).toString();


    db = DataBaseBuilder::createPostgresDataBase(m_dbhost, m_dbport, m_dbname, m_dbuser, m_dbpass, m_schema);

    if (!db->open())
        qDebug() << db->lastError().text();
        //QFAIL(db->lastError().text().toStdString().c_str());

    sut = ParamsRepository();
}

ParamsRepositoryTest::~ParamsRepositoryTest()
{

}

void ParamsRepositoryTest::initTestCase()
{
    if (DataBaseBuilder::createSchema() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    DataBaseBuilder::createCanvasTable();

    DataBaseBuilder::fillCanvasTable();
}

void ParamsRepositoryTest::init()
{
    DataBaseBuilder::createParamsTable();

    DataBaseBuilder::fillParamsTable();
}

void ParamsRepositoryTest::getParamsTest()
{
    //ARRANGE in init()
    shared_ptr<ParamsBL> params, same_params;
    string login = "user1";
    string password = "123";

    //ACT
    try
    {
        params = sut.getParams(1);
    }
    //ASSERT
    catch (BaseError &er)
    {
        QFAIL(er.what());
        return;
    }
    catch (...)
    {
        QFAIL("Unexpected error");
        return;
    }

    string query = "SELECT * FROM " + m_schema + ".Params where canvas_id=" + to_string(1) + ";";
    QSqlQuery q;
    if (!q.exec(QString::fromStdString(query)))
        QFAIL(q.lastError().text().toStdString().c_str());
    if (q.next())
    {
        int c_id = q.value(0).toInt();
        int width = q.value(1).toInt();
        int height = q.value(2).toInt();
        double range = q.value(3).toDouble();
        bool smooth = q.value(4).toBool();
        int mult = q.value(5).toInt();
        int red = q.value(6).toInt();
        int green = q.value(7).toInt();
        int blue = q.value(8).toInt();
        int size = q.value(9).toInt();

        same_params =  make_shared<ParamsBL>(c_id, width, height, range, smooth, mult, red, green, blue, size);
    }

    QVERIFY2(*params == *same_params, "getParamsTest error");
}

void ParamsRepositoryTest::addParamsTest()
{
    //ARRANGE in init()
    ParamsBL params = ParamsBL(5, 860, 440, 24.00, false, 2, 110, 21, 140, 65);
    shared_ptr<ParamsBL> same_params;
    int id = -1;

    //ACT
    try
    {
        id = sut.addParams(params);
    }
    //ASSERT
    catch (BaseError &er)
    {
        QFAIL(er.what());
        return;
    }
    catch (...)
    {
        QFAIL("Unexpected error");
        return;
    }

    //user = UserBL(id, user.getLogin(), user.getPassword(), user.getRole(), user.getModeratorId());
    string query = "SELECT * FROM " + m_schema + ".Params where canvas_id=" + std::to_string(id) + ";";
    QSqlQuery q;
    if (!q.exec(QString::fromStdString(query)))
        QFAIL(q.lastError().text().toStdString().c_str());
    if (q.next())
    {
        int c_id = q.value(0).toInt();
        int width = q.value(1).toInt();
        int height = q.value(2).toInt();
        double range = q.value(3).toDouble();
        bool smooth = q.value(4).toBool();
        int mult = q.value(5).toInt();
        int red = q.value(6).toInt();
        int green = q.value(7).toInt();
        int blue = q.value(8).toInt();
        int size = q.value(9).toInt();

        same_params =  make_shared<ParamsBL>(c_id, width, height, range, smooth, mult, red, green, blue, size);
    }

    QVERIFY2(params == *same_params, "addParamsTest error");
}

void ParamsRepositoryTest::deleteParamsTest()
{
    //ARRANGE в init()

    //ACT
    try
    {
        sut.deleteParams(1);
    }
    //ASSERT
    catch (DeleteCanvasError &er)
    {
        QFAIL(er.what());
        return;
    }
    catch (...)
    {
        QFAIL("Unexpected error");
        return;
    }

    string query = "SELECT * FROM " + m_schema + ".Params where canvas_id=" + to_string(1) + ";";
    QSqlQuery q;
    if (!q.exec(QString::fromStdString(query)))
        QFAIL(q.lastError().text().toStdString().c_str());
    if (q.next())
        QFAIL("Canvas is not deleted");
}

void ParamsRepositoryTest::updateParamsTest()
{
    //ARRANGE
    ParamsBL params = ParamsBL(1, 860, 440, 24.00, false, 2, 110, 21, 140, 65);
    shared_ptr<ParamsBL> same_params;

    //ACT
    try
    {
        sut.updateParams(params, 1);
    }
    //ASSERT
    catch (BaseError &er)
    {
        QFAIL(er.what());
        return;
    }
    catch (...)
    {
        QFAIL("Unexpected error");
        return;
    }

    string query = "SELECT * FROM " + m_schema + ".Params where canvas_id=" + to_string(1) + ";";
    QSqlQuery q;
    if (!q.exec(QString::fromStdString(query)))
        QFAIL(q.lastError().text().toStdString().c_str());
    if (q.next())
    {
        int c_id = q.value(0).toInt();
        int width = q.value(1).toInt();
        int height = q.value(2).toInt();
        double range = q.value(3).toDouble();
        bool smooth = q.value(4).toBool();
        int mult = q.value(5).toInt();
        int red = q.value(6).toInt();
        int green = q.value(7).toInt();
        int blue = q.value(8).toInt();
        int size = q.value(9).toInt();

        same_params =  make_shared<ParamsBL>(c_id, width, height, range, smooth, mult, red, green, blue, size);
    }

    QVERIFY2(params == *same_params, "addParamsTest error");
}

void ParamsRepositoryTest::cleanup()
{
    DataBaseBuilder::dropParamsTable();
}

void ParamsRepositoryTest::cleanupTestCase()
{
    if (DataBaseBuilder::dropCanvasTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    if (DataBaseBuilder::dropSchemaCascade() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

QTEST_GUILESS_MAIN(ParamsRepositoryTest)

#include "tst_paramsrepositorytest.moc"
