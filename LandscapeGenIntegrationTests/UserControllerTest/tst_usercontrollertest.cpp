#include <QtTest>

#define FILENAME "UserControllerTest.cfg"

#include "Settings.h"
#include "Controllers/usercontroller.h"
#include "Repositorys/UsersRepository.h"
#include "Repositorys/CanvasRepository.h"
#include "Repositorys/ParamsRepository.h"
#include "common.h"

class UserControllerTest : public QObject
{
    Q_OBJECT

public:
    UserControllerTest();
    ~UserControllerTest();

private slots:
    void initTestCase();
    void init();

    void getUserCanvases();
    void selectCanvasTest();
    void sendCanvasTest();

    void cleanup();
    void cleanupTestCase();

private:
    unique_ptr<UserController> user_controller;
    unique_ptr<UsersRepository> users_repository;
    unique_ptr<CanvasRepository> canvas_repository;
    unique_ptr<ParamsRepository> params_repository;
    unique_ptr<QSqlDatabase> db;
    string m_schema;
};

UserControllerTest::UserControllerTest()
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

    user_controller = make_unique<UserController>();
    users_repository = make_unique<UsersRepository>();
    canvas_repository = make_unique<CanvasRepository>();
    params_repository = make_unique<ParamsRepository>();
}

UserControllerTest::~UserControllerTest()
{

}

void UserControllerTest::initTestCase()
{
    if (DataBaseBuilder::createSchema() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

void UserControllerTest::init()
{
    if (DataBaseBuilder::createUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    if (DataBaseBuilder::fillUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    if (DataBaseBuilder::createCanvasTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    if (DataBaseBuilder::fillCanvasTable(4) != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    if (DataBaseBuilder::createParamsTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    if (DataBaseBuilder::fillParamsTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    shared_ptr<UserBL> ubl = users_repository->getUser("user1", "123");
    user_controller->login(ubl);
}

void UserControllerTest::getUserCanvases()
{
    //ARRANGE
    int u_id = 1;
    vector<CanvasBL> canvases_vec;
    if (DataBaseBuilder::getCanvasesByUid(canvases_vec, u_id) != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    vector<pair<int, string> > canvases_exp;
    for (const CanvasBL& c : canvases_vec)
        canvases_exp.push_back(make_pair(c.getId(), c.getName()));
    vector<pair<int, string> > canvases;

    //ACT
    try
    {
        canvases = canvas_repository->getCanvasesByUid(u_id);
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

    for (int i = 0; i < canvases_exp.size(); i++)
    {
        QString msg = QString("i = %1 canvases.id = %2; canvases.name = %3; same_canvases.id = %4; same_canvases.name = %5;").
                arg(QString::number(i),QString::number(canvases[i].first), QString::fromStdString(canvases[i].second),
                    QString::number(canvases_exp[i].first), QString::fromStdString(canvases_exp[i].second));
        QVERIFY2(canvases[i] == canvases_exp[i], msg.toStdString().c_str());
    }
}

void UserControllerTest::selectCanvasTest()
{
    //ARRANGE
    int id = 1;
    shared_ptr<CanvasBL> canvas_bl = canvas_repository->getCanvas(id);
    int r, g, b;
    canvas_bl->getColor(r, g, b);

    //ACT
    user_controller->selectCanvas(make_shared<LandscapeCanvas>(canvas_bl->getHeightsMap(), canvas_bl->getHeightsMapPoints(), r, g, b));

    //ASSERT
    shared_ptr<CanvasBL> canvas_exp;
    if (DataBaseBuilder::getCanvas(canvas_exp, id) != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    QVERIFY2(*user_controller->getHeightsMap() == canvas_exp->getHeightsMap(), "selectCanvasTest error");
    QVERIFY2(*user_controller->getHeightsMapPoints() == canvas_exp->getHeightsMapPoints(), "selectCanvasTest error");
}

void UserControllerTest::sendCanvasTest()
{
    //ARRANGE
    int id = 1;
    shared_ptr<CanvasBL> canvas_bl = canvas_repository->getCanvas(id);
    int r, g, b;
    canvas_bl->getColor(r, g, b);
    user_controller->selectCanvas(make_shared<LandscapeCanvas>(canvas_bl->getHeightsMap(), canvas_bl->getHeightsMapPoints(), r, g, b));
    user_controller->transform(Point(10, 20, 30), Point(10, 20, 30), Point(10, 20, 30));

    int u_id;
    user_controller->getColor(r, g, b);
    u_id = user_controller->getUser()->getId();
    CanvasBL cbl = CanvasBL(0, u_id, "name", *(user_controller->getHeightsMap()), *user_controller->getHeightsMapPoints(), r, g, b);

    //ACT
    canvas_repository->addCanvas(cbl);

    //ASSERT
    shared_ptr<CanvasBL> canvas_exp;
    if (DataBaseBuilder::getCanvas(canvas_exp, 5) != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    QVERIFY2(cbl == *canvas_exp, "sendCanvasTest error");
}

void UserControllerTest::cleanup()
{
    user_controller = make_unique<UserController>();
    if (DataBaseBuilder::dropParamsTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    if (DataBaseBuilder::dropCanvasTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    if (DataBaseBuilder::dropUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

void UserControllerTest::cleanupTestCase()
{
    if (DataBaseBuilder::dropSchemaCascade() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

QTEST_APPLESS_MAIN(UserControllerTest)

#include "tst_usercontrollertest.moc"
