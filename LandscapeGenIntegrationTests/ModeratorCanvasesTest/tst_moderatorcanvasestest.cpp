#include <QtTest>

#define FILENAME "../default.cfg"

#include "Settings.h"
#include "Controllers/ModeratorCanvasesController.h"
#include "Repositorys/UsersRepository.h"
#include "Repositorys/CanvasRepository.h"
#include "common.h"

class ModeratorCanvasesTest : public QObject
{
    Q_OBJECT

public:
    ModeratorCanvasesTest();
    ~ModeratorCanvasesTest();

private slots:
    void initTestCase();
    void init();

    void selectCanvasTest();

    void cleanup();
    void cleanupTestCase();

private:
    unique_ptr<ModeratorCanvasesController> moderator_canvases_controller;
    unique_ptr<UsersRepository> users_repository;
    unique_ptr<CanvasRepository> canvas_repository;
    unique_ptr<QSqlDatabase> db;
    string m_schema;
};

ModeratorCanvasesTest::ModeratorCanvasesTest()
{
    QFile cfgDefaults(FILENAME);
    cfgDefaults.open(QIODevice::ReadOnly);
    Settings::setDefaults(cfgDefaults.readAll());

    m_schema = Settings::get(Settings::Schema, Settings::DataBase).toString().toStdString();

    QString m_dbhost = Settings::get(Settings::DBHost, Settings::DataBase).toString();
    int m_dbport = Settings::get(Settings::DBPort, Settings::DataBase).toInt();
    QString m_dbname = Settings::get(Settings::DBName, Settings::DataBase).toString();
    QString m_dbuser = Settings::get(Settings::DBUser, Settings::DataBase).toString();
    QString m_dbpass = Settings::get(Settings::DBPass, Settings::DataBase).toString();

    db = DataBaseBuilder::createPostgresDataBase(m_dbhost, m_dbport, m_dbname, m_dbuser, m_dbpass, m_schema);

    moderator_canvases_controller = make_unique<ModeratorCanvasesController>();
    users_repository = make_unique<UsersRepository>();
    canvas_repository = make_unique<CanvasRepository>();
}

ModeratorCanvasesTest::~ModeratorCanvasesTest()
{

}

void ModeratorCanvasesTest::initTestCase()
{
    if (DataBaseBuilder::createSchema() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

}

void ModeratorCanvasesTest::init()
{
    if (DataBaseBuilder::createUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    if (DataBaseBuilder::fillUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());


    if (DataBaseBuilder::createCanvasTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    if (DataBaseBuilder::fillCanvasTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    shared_ptr<UserBL> ubl = users_repository->getUser("moderator1", "123");
    moderator_canvases_controller->login(ubl);
}

void ModeratorCanvasesTest::selectCanvasTest()
{
    //ARRANGE
    int id = 1;
    shared_ptr<CanvasBL> canvas_bl = canvas_repository->getCanvas(id);
    int r, g, b;
    canvas_bl->getColor(r, g, b);

    //ACT
    moderator_canvases_controller->selectCanvas(make_shared<LandscapeCanvas>(canvas_bl->getHeightsMap(), canvas_bl->getHeightsMapPoints(), r, g, b));

    //ASSERT

    shared_ptr<CanvasBL> canvas_exp;
    if (DataBaseBuilder::getCanvas(canvas_exp, id) != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    QVERIFY2(*moderator_canvases_controller->getHeightsMap() == canvas_exp->getHeightsMap(), "selectCanvasTest error");
    QVERIFY2(*moderator_canvases_controller->getHeightsMapPoints() == canvas_exp->getHeightsMapPoints(), "selectCanvasTest error");
}

void ModeratorCanvasesTest::cleanup()
{
    if (DataBaseBuilder::dropCanvasTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    if (DataBaseBuilder::dropUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

void ModeratorCanvasesTest::cleanupTestCase()
{
    if (DataBaseBuilder::dropSchemaCascade() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

QTEST_APPLESS_MAIN(ModeratorCanvasesTest)

#include "tst_moderatorcanvasestest.moc"
