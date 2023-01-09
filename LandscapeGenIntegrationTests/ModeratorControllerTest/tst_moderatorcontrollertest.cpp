#include <QtTest>

#define FILENAME "../default.cfg"

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
    unique_ptr<UsersRepository> user_repository;
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
    user_repository = make_unique<UsersRepository>();
}

ModeratorControllerTest::~ModeratorControllerTest()
{

}

void ModeratorControllerTest::initTestCase()
{

}

void ModeratorControllerTest::init()
{

}

void ModeratorControllerTest::addUserTest()
{

}

void ModeratorControllerTest::deleteUserTest()
{

}

void ModeratorControllerTest::cleanup()
{

}

void ModeratorControllerTest::cleanupTestCase()
{

}

QTEST_APPLESS_MAIN(ModeratorControllerTest)

#include "tst_moderatorcontrollertest.moc"
