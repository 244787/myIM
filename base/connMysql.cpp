#include"connMysql.h"

connMysql::connMysql() {
    init();
}
connMysql::~connMysql() {
    uninit();
}
void connMysql::init() {
    m_mysql = nullptr;
    m_result = nullptr;
    m_mysql = mysql_init(nullptr);
    if (m_mysql) {
        mysql_set_character_set(m_mysql, "utf8");
        isConnected = true;
    }
    Connect("127.0.0.1","root","Linux666.","IMDB",0);

}
void connMysql::uninit() {
    freeResult();
    if (m_mysql) {
        mysql_close(m_mysql);
        m_mysql = nullptr;
    }
    isConnected = false;
}
void connMysql::freeResult() {
    if (m_result) {
        mysql_free_result(m_result);
        m_result = nullptr;
    }
}
//连接数据库
bool connMysql::Connect(const std::string& ip,
    const std::string& user,
    const std::string& pwd,
    const std::string& db,
    const unsigned short& port) {
    if (!m_mysql) {
        return false;
    }

    mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "gbk");
    m_mysql = mysql_real_connect(m_mysql, ip.c_str(), user.c_str(), pwd.c_str(), db.c_str(), port, NULL, 0);
    if (!m_mysql) {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(m_mysql));
        return false;
    }
    std::cout << "连接数据库成功！" << std::endl;
    return true;
}
//更新数据库，增删改操作
bool connMysql::Update(const std::string& sql) {
    std::unique_lock<std::mutex> locker(mtx);
    if (mysql_query(m_mysql, sql.c_str())) {
        return false;
    }
    return true;
}
//查找操作
bool connMysql::Query(const std::string& sql) {
    std::unique_lock<std::mutex> locker(mtx);
    //清除上一次的结果
    freeResult();
    if (mysql_query(m_mysql, sql.c_str())) {
        return false;
    }
    m_result = mysql_store_result(m_mysql);
    return true;
}
MYSQL_RES* connMysql::getResult() {
    std::unique_lock<std::mutex> locker(mtx);
    return m_result;
}

int connMysql::affectRows() {
    std::unique_lock<std::mutex> locker(mtx);
    return mysql_affected_rows(m_mysql);
}

