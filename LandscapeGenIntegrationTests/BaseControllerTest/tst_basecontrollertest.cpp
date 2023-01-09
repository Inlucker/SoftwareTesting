#include <QtTest>

#define FILENAME "../default.cfg"

#include "Settings.h"
#include "Controllers/BaseController.h"
#include "Repositorys/UsersRepository.h"
#include "common.h"

class BaseControllerTest : public QObject
{
    Q_OBJECT

public:
    BaseControllerTest();
    ~BaseControllerTest();

private slots:
    void initTestCase();
    void init();

    void loginTest();
    void logoutTest();

    void cleanup();
    void cleanupTestCase();

private:
    unique_ptr<BaseController> base_controller;
    unique_ptr<UsersRepository> user_repository;
    unique_ptr<QSqlDatabase> db;
    string m_schema;
};

BaseControllerTest::BaseControllerTest()
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

    base_controller = make_unique<BaseController>();
    user_repository = make_unique<UsersRepository>();
}

BaseControllerTest::~BaseControllerTest()
{

}

void BaseControllerTest::initTestCase()
{
    if (DataBaseBuilder::createSchema() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

void BaseControllerTest::init()
{
    if (DataBaseBuilder::createUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());

    if (DataBaseBuilder::fillUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

void BaseControllerTest::loginTest()
{
    //ARRANGE
    shared_ptr<UserBL> same_ubl = user_repository->getUser("user1", "123");

    //ACT
    base_controller->login(same_ubl);

    //ASSERT
    shared_ptr<UserBL> ubl = base_controller->getUser();
    QVERIFY2(*ubl == *same_ubl, "Error in loginTest");
}

void BaseControllerTest::logoutTest()
{
    //ARRANGE

    //ACT
    base_controller->logout();

    //ASSERT
    shared_ptr<UserBL> ubl = base_controller->getUser();
    QVERIFY2(ubl == nullptr, "Error in logoutTest");
}

void BaseControllerTest::cleanup()
{
    if (DataBaseBuilder::dropUsersTable() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

void BaseControllerTest::cleanupTestCase()
{
    if (DataBaseBuilder::dropSchemaCascade() != OK)
        QFAIL(DataBaseBuilder::lastError().c_str());
}

QTEST_APPLESS_MAIN(BaseControllerTest)

#include "tst_basecontrollertest.moc"
