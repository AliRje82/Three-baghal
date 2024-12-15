#include "stdio.h"
#include "sqlite3.h"
#include "structs.h"

sqlite3 *db;
extern userInfo *user;

void database_creation()
{
    char *err_msg = 0;
    int rc;
    rc = sqlite3_open("purchases.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS purchases ("
                      "userid INTEGER, "
                      "orderid INTEGER, "
                      "store1 BOOLEAN DEFAULT 0, "
                      "store2 BOOLEAN DEFAULT 0, "
                      "store3 BOOLEAN DEFAULT 0, "
                      "PRIMARY KEY (userid, orderid));";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }
    printf("Database opens successfully\n");
    sqlite3_close(db);
}

void user_query()
{
    database_creation();

    char *err_msg = 0;
    sqlite3_stmt *res;
    int rc;

    rc = sqlite3_open("purchases.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql_select =
        "SELECT MAX(orderid), store1, store2, store3 "
        "FROM purchases WHERE userid = ?;";

    rc = sqlite3_prepare_v2(db, sql_select, -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(res, 1, user->user_id);

    if (sqlite3_step(res) == SQLITE_ROW)
    {
        if (sqlite3_column_type(res, 0) != SQLITE_NULL)
        {
            // User is in db
            user->order_id = sqlite3_column_int(res, 0);
            user->has_bought_from_store1 = sqlite3_column_int(res, 1);
            user->has_bought_from_store2 = sqlite3_column_int(res, 2);
            user->has_bought_from_store3 = sqlite3_column_int(res, 3);

            printf("User ID: %d exists.\n", user->user_id);
            printf("Last Order ID: %d\n", user->order_id);
            printf("Stores Purchased: Store1: %d, Store2: %d, Store3: %d\n", user->has_bought_from_store1,
                   user->has_bought_from_store2, user->has_bought_from_store3);

            sqlite3_finalize(res);
            return;
        }
    }

    sqlite3_finalize(res);

    // User is not in db
    printf("User ID: %d does not exist. Adding to database.\n", user->user_id);

    const char *sql_insert =
        "INSERT INTO purchases (userid, orderid, store1, store2, store3) "
        "VALUES (?, 0, 0, 0, 0);";

    rc = sqlite3_prepare_v2(db, sql_insert, -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare insert statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(res, 1, user->user_id);
    if (sqlite3_step(res) != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to insert user: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        user->order_id = 0;
        user->has_bought_from_store1 = 0;
        user->has_bought_from_store2 = 0;
        user->has_bought_from_store3 = 0;
        printf("User ID: %d added to the database with Order ID: 0\n", user->user_id);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
}

void update_order_and_stores() {
    sqlite3_stmt *res;
    int rc;

        rc = sqlite3_open("store.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    const char *sql_update = 
        "UPDATE purchases SET orderid = ?, store1 = ?, store2 = ?, store3 = ? WHERE userid = ?;";

    rc = sqlite3_prepare_v2(db, sql_update, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare update statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(res, 1, user->order_id); 
    sqlite3_bind_int(res, 2, user->has_bought_from_store1);
    sqlite3_bind_int(res, 3, user->has_bought_from_store2);
    sqlite3_bind_int(res, 4, user->has_bought_from_store3);
    sqlite3_bind_int(res, 5, user->user_id);  

    rc = sqlite3_step(res);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to update data: %s\n", sqlite3_errmsg(db));
    } else {
        printf("User ID: %d updated. Order ID: %d, Store1: %d, Store2: %d, Store3: %d\n", 
               user->user_id, user->order_id, user->has_bought_from_store1,
               user->has_bought_from_store2, user->has_bought_from_store3);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

}


