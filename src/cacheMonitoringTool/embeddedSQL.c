#include <mysql/mysql.h>
#include <string.h>
#include <stdio.h>
#define DB_HOST "192.168.0.5"
#define DB_USER "root"
#define DB_PASS "1234"
#define DB_NAME "my_report"
#define CHOP(x) x[strlen(x) - 1] = ' '

int main(void)
{
	MYSQL       *connection, conn;
	MYSQL_RES   *sql_result;
	MYSQL_ROW   sql_row;
	int       query_stat;

	//Initialize
	mysql_init(&conn);

	//Connection
	connection = NULL;
	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);
	if(connection == NULL)
	{
		fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
		return 1;
	}

	//Select Query
	query_stat = mysql_query(connection, "select * from mydata");
	if(query_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		return 1;
	}

	//Print
	printf("%+11s %-5s", "time", "temperature");
	sql_result = mysql_store_result(connection);
	while((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		printf("%+11s %-5s", sql_row[0], sql_row[1]);
	}

	mysql_free_result(sql_result);

	//Insert Query
	sprintf(query, "insert into mydata values ""('%s', '%f')", "2015-12-31", 13.1);
	query_stat = mysql_query(connection, query);
	if(query_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		return 1;
	}

	mysql_close(connection);
}
