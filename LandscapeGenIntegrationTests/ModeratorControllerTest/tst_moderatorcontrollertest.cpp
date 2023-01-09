#include <QtTest>

#define FILENAME "ModeratorControllerTest.cfg"

#include "Settings.h"
#include "Controllers/ModeratorController.h"
#include "Repositorys/UsersRepository.h"
#include "common.h"

class ModeratorControllerTest : public QObject
{
    Q_OBJECT

public:
    ModeratorControllerTest();
    ~ModeratorControllerTest();

private slots:
    void initTestCase();
    void init();

    void addUserTest();
    void deleteUserTest();

    void cleanup();
    void cleanupTestCase();

private:
    unique_ptr<ModeratorController> moderator_controller;
    unique_ptr<UsersRepository> users_repository;
    unique_ptr<QSqlDatabase> db;
    string m_schema;
};

ModeratorControllerTest::ModeratorControllerTest()
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
    users_repository = make_unique<UsersRepository>();
}

ModeratorControllerTest::~ModeratorControllerTest()
{

}

void ModeratorControllerTest::initTestCase()
{
    if (DataBaseBuilder::createSchema() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

}

void ModeratorControllerTest::init()
{
    if (DataBaseBuilder::createUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    if (DataBaseBuilder::fillUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    shared_ptr<UserBL> ubl = users_repository->getUser("moderator1", "123");
    moderator_controller->login(ubl);
}

void ModeratorControllerTest::addUserTest()
{
    //ARRANGE
    shared_ptr<UserBL> canvas_user = users_repository->getCanvasUser("user1");
    UserBL new_user_bl(canvas_user->getId(), canvas_user->getLogin(), canvas_user->getPassword(), canvas_user->getRole(), moderator_controller->getUser()->getId());

    //ACT
    users_repository->updateUser(new_user_bl, canvas_user->getId());

    //ASSERT
    shared_ptr<UserBL> user;
    if (DataBaseBuilder::getUser(user, 1) != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    QVERIFY2(user->getModeratorId() == moderator_controller->getUser()->getId(), "Error in addUserTest");
}

void ModeratorControllerTest::deleteUserTest()
{
    //ARRANGE
    shared_ptr<UserBL> canvas_user = users_repository->getCanvasUser("user3");
    UserBL new_user_bl(canvas_user->getId(), canvas_user->getLogin(), canvas_user->getPassword(), canvas_user->getRole(), -1);

    //ACT
    users_repository->updateUser(new_user_bl, canvas_user->getId());

    //ASSERT
    shared_ptr<UserBL> user;
    if (DataBaseBuilder::getUser(user, 4) != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
    QVERIFY2(user->getModeratorId() == 0, "Error in deleteUserTest");
}

void ModeratorControllerTest::cleanup()
{
    moderator_controller = make_unique<ModeratorController>();
    if (DataBaseBuilder::dropUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

void ModeratorControllerTest::cleanupTestCase()
{
    if (DataBaseBuilder::dropSchemaCascade() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

QTEST_APPLESS_MAIN(ModeratorControllerTest)

#include "tst_moderatorcontrollertest.moc"
