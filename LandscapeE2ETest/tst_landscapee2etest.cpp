#include <QtTest>

#define FILENAME "E2ETest.cfg"

#include "Settings.h"
#include "Controllers/ModeratorController.h"
#include "Controllers/ModeratorCanvasesController.h"
#include "Controllers/usercontroller.h"
#include "Repositorys/UsersRepository.h"
#include "Repositorys/CanvasRepository.h"
#include "Repositorys/ParamsRepository.h"
#include "common.h"

class LandscapeE2ETest : public QObject
{
    Q_OBJECT

public:
    LandscapeE2ETest();
    ~LandscapeE2ETest();

private slots:
    void initTestCase();
    void init();
    void moderatorShowcase();
    void userShowcase();
    void cleanup();
    void cleanupTestCase();

private:
    unique_ptr<BaseController> base_controller;
    unique_ptr<ModeratorController> moderator_controller;
    unique_ptr<ModeratorCanvasesController> moderator_canvases_controller;
    unique_ptr<UserController> user_controller;
    unique_ptr<UsersRepository> users_repository;
    unique_ptr<CanvasRepository> canvas_repository;
    unique_ptr<ParamsRepository> params_repository;
    unique_ptr<QSqlDatabase> db;
    string m_schema;
};

LandscapeE2ETest::LandscapeE2ETest()
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

    moderator_controller = make_unique<ModeratorController>();
    moderator_canvases_controller = make_unique<ModeratorCanvasesController>();
    user_controller = make_unique<UserController>();
    users_repository = make_unique<UsersRepository>();
    canvas_repository = make_unique<CanvasRepository>();
    params_repository = make_unique<ParamsRepository>();
}

LandscapeE2ETest::~LandscapeE2ETest()
{

}

void LandscapeE2ETest::initTestCase()
{
    if (DataBaseBuilder::createSchema() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

void LandscapeE2ETest::init()
{
    if (DataBaseBuilder::createUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    if (DataBaseBuilder::fillUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    if (DataBaseBuilder::createCanvasTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    if (DataBaseBuilder::fillCanvasTable(4) != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    /*if (DataBaseBuilder::createParamsTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    if (DataBaseBuilder::fillParamsTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());*/
}

void LandscapeE2ETest::moderatorShowcase()
{
    try
    {
        //login moderator
        shared_ptr<UserBL> moderator = users_repository->getUser("moderator1", "123");
        moderator_controller->login(moderator);
        QVERIFY(moderator_controller->getUser()->getId() == 3);

        //get free users
        vector<string> free_canvas_users = users_repository->getFreeCanvasUsers();
        QVERIFY(free_canvas_users.size() == 2);

        //get our users
        vector<string> canvas_users = users_repository->getCanvasUsersByMid(moderator_controller->getUser()->getId());
        QVERIFY(canvas_users.size() == 1);

        //add user
        shared_ptr<UserBL> canvas_user = users_repository->getCanvasUser(free_canvas_users[0]);
        UserBL new_user_bl(canvas_user->getId(), canvas_user->getLogin(), canvas_user->getPassword(), canvas_user->getRole(), moderator_controller->getUser()->getId());
        users_repository->updateUser(new_user_bl, canvas_user->getId());

        {
            shared_ptr<UserBL> user;
            if (DataBaseBuilder::getUser(user, 1) != OK)
                QFAIL(DataBaseBuilder::lastError().c_str());
            QVERIFY2(user->getModeratorId() == moderator_controller->getUser()->getId(), "Error in addUserTest");
        }

        //watch user canvses
        moderator_canvases_controller->login(moderator_controller->getUser());
        vector<pair<int, string>> canvases = canvas_repository->getCanvasesByUid(canvas_user->getId());
        QVERIFY(canvases.size() == 4);
    }
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
}

void LandscapeE2ETest::userShowcase()
{
    try
    {
        //login canvas_user
        shared_ptr<UserBL> canvas_user = users_repository->getUser("user1", "123");
        user_controller->login(canvas_user);
        QVERIFY(user_controller->getUser()->getId() == 1);

        //get canvases
        vector<pair<int, string>> canvases = canvas_repository->getCanvasesByUid(user_controller->getUser()->getId());
        QVERIFY(canvases.size() == 4);

        //load canvas
        shared_ptr<CanvasBL> canvas_bl = canvas_repository->getCanvas(1);
        int r, g, b;
        canvas_bl->getColor(r, g, b);
        user_controller->selectCanvas(make_shared<LandscapeCanvas>(canvas_bl->getHeightsMap(), canvas_bl->getHeightsMapPoints(), r, g, b));

        shared_ptr<CanvasBL> canvas_exp;
        if (DataBaseBuilder::getCanvas(canvas_exp, 1) != OK)
            QFAIL(DataBaseBuilder::lastError().c_str());
        QVERIFY2(*user_controller->getHeightsMap() == canvas_exp->getHeightsMap(), "selectCanvasTest error");
        QVERIFY2(*user_controller->getHeightsMapPoints() == canvas_exp->getHeightsMapPoints(), "selectCanvasTest error");

        //update canvas
        user_controller->transform(Point(10, 20, 30), Point(10, 20, 30), Point(10, 20, 30));

        int u_id;
        user_controller->getColor(r, g, b);
        u_id = user_controller->getUser()->getId();
        string name = canvas_exp->getName();
        CanvasBL cbl = CanvasBL(1, u_id, name, *(user_controller->getHeightsMap()), *(user_controller->getHeightsMapPoints()), r, g, b);
        canvas_repository->updateCanvas(cbl, 1);

        if (DataBaseBuilder::getCanvas(canvas_exp, 1) != OK)
            QFAIL(DataBaseBuilder::lastError().c_str());
        QVERIFY2(cbl == *canvas_exp, "sendCanvasTest error");

        //send canvas
        user_controller->getColor(r, g, b);
        u_id = user_controller->getUser()->getId();
        cbl = CanvasBL(0, u_id, "name", *(user_controller->getHeightsMap()), *user_controller->getHeightsMapPoints(), r, g, b);
        canvas_repository->addCanvas(cbl);

        if (DataBaseBuilder::getCanvas(canvas_exp, 5) != OK)
            QFAIL(DataBaseBuilder::lastError().c_str());
        QVERIFY2(cbl == *canvas_exp, "sendCanvasTest error");

        //delete canvas
        canvas_repository->deleteCanvas(2);

        Status status = DataBaseBuilder::getCanvas(canvas_exp, 2);
        if (status == EXEC_ERROR)
            QFAIL(DataBaseBuilder::lastError().c_str());
        if (status != EMPTY_RES)
            QFAIL("Canvas is not deleted");
    }
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
}

void LandscapeE2ETest::cleanup()
{
    moderator_controller = make_unique<ModeratorController>();
    moderator_canvases_controller = make_unique<ModeratorCanvasesController>();
    user_controller = make_unique<UserController>();
    /*if (DataBaseBuilder::dropParamsTable() != OK)
        qDebug() << DataBaseBuilder::lastError().c_str();*/
    if (DataBaseBuilder::dropCanvasTable() != OK)
        qDebug() << DataBaseBuilder::lastError().c_str();
    if (DataBaseBuilder::dropUsersTable() != OK)
        qDebug() << DataBaseBuilder::lastError().c_str();
}

void LandscapeE2ETest::cleanupTestCase()
{
    if (DataBaseBuilder::dropSchemaCascade() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

QTEST_APPLESS_MAIN(LandscapeE2ETest)

#include "tst_landscapee2etest.moc"
