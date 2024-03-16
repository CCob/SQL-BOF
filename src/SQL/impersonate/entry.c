#include <windows.h>
#include "bofdefs.h"
#include "base.c"
#include "sql.c"


void CheckImpersonate(char* server, char* database)
{
    SQLHENV env		= NULL;
    SQLHSTMT stmt 	= NULL;


    SQLHDBC dbc = ConnectToSqlServer(&env, server, database);

    if (dbc == NULL) 
	{
		goto END;
	}

	internal_printf("[*] Enumerating users that can be impersonated on %s\n\n", server);

	//
	// allocate statement handle
	//
	ODBC32$SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

	//
	// Run the query
	//
	SQLCHAR* query = (SQLCHAR*)"SELECT distinct b.name FROM sys.server_permissions a "
            		"INNER JOIN sys.server_principals b ON a.grantor_principal_id "
            		"= b.principal_id WHERE a.permission_name = 'IMPERSONATE';";
	if (!ExecuteQuery(stmt, query))
	{
		goto END;
	}
	PrintQueryResults(stmt, TRUE);

	//
	// close the cursor
	//
	ODBC32$SQLCloseCursor(stmt);

END:
	DisconnectSqlServer(env, dbc, stmt);
}


#ifdef BOF
VOID go( 
	IN PCHAR Buffer, 
	IN ULONG Length 
) 
{
	char* server 	= NULL;
	char* database 	= NULL;

	//
	// parse beacon args 
	//
	datap parser;
	BeaconDataParse(&parser, Buffer, Length);

	server 		= BeaconDataExtract(&parser, NULL);
	database 	= BeaconDataExtract(&parser, NULL);

	server = *server == 0 ? "localhost" : server;
	database = *database == 0 ? "master" : database;

	if(!bofstart())
	{
		return;
	}
	
	CheckImpersonate(server, database);

	printoutput(TRUE);
};

#else

int main()
{
	CheckImpersonate("192.168.0.215", "master");
}

#endif