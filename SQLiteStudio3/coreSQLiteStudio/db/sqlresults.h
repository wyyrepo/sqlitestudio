#ifndef SQLRESULTS_H
#define SQLRESULTS_H

#include "sqlresultsrow.h"
#include <QList>
#include <QSharedPointer>

/** @file */

/**
 * @brief Row ID of the row in any table.
 *
 * It's a advanced ROWID container that can hold either simple integer ROWID,
 * or a set of column values reflecting one or more PRIMARY KEY columns.
 *
 * This way it RowId can be applied to both regular tables, as well as "WITHOUT ROWID" tables.
 *
 * Each entry in the RowId has a key and a value (after all it's a QHash). Keys are names
 * of the column and values are values in that columns. For regular tables the RowId
 * will contain exactly one entry: <tt>ROWID -> some_integer</tt>
 */
typedef QHash<QString,QVariant> RowId;

/**
 * @brief SQL query results
 *
 * This object is created by and returned from Db::exec() (and familiar methods) calls.
 * It uses incremental reading for accessing data, so it only reads as much data
 * as you ask it to. It can tell you how many rows and how many columns are available
 * in the results. It also provides information about errors that occured during query execution.
 *
 * Typical workflow looks like this:
 * @code
 * SqlResultsPtr results = db->exec("SELECT * FROM table");
 * SqlResultsRowPtr row;
 * while (row = results->next())
 * {
 *     qDebug() << row->valueList();
 * }
 * @endcode
 *
 *
 */
class API_EXPORT SqlResults
{
    public:
        /**
         * @brief Releases result resources.
         */
        virtual ~SqlResults();

        /**
         * @brief Reads next row of results
         * @return Next results row, or null pointer if no more rows are available.
         */
        virtual SqlResultsRowPtr next() = 0;

        /**
         * @brief Tells if there is next row available.
         * @return true if there's next row, of false if there's not.
         *
         * If you just want to iterate through rows, you don't need to call this method.
         * The next() method will return null pointer if there is no next row available,
         * so you can tell when to stop iterating. Furthermore, you should avoid
         * calling this method just for iterating through rows, because next() method
         * already does that internally (in most implementations).
         *
         * In other cases this method might be useful. For example when you read single cell:
         * @code
         * SqlResultsPtr results = db->("SELECT value FROM table WHERE rowid = ?", {rowId});
         * if (results->isError() || !results->hasNext())
         *    return "some default value";
         *
         * return results->getSingleCell().toString();
         * @endcode
         */
        virtual bool hasNext() = 0;

        /**
         * @brief Gets error test of the most recent error.
         * @return Error text.
         */
        virtual QString getErrorText() = 0;

        /**
         * @brief Gets error code of the most recent error.
         * @return Error code as returned from DbPlugin.
         */
        virtual int getErrorCode() = 0;

        /**
         * @brief Gets list of column names in the results.
         * @return List of column names.
         */
        virtual QStringList getColumnNames() = 0;

        /**
         * @brief Gets number of columns in the results.
         * @return Columns count.
         */
        virtual int columnCount() = 0;

        /**
         * @brief Gets number of rows in the results.
         * @return Rows count.
         */
        virtual qint64 rowCount() = 0;

        /**
         * @brief Gets number of rows that were affected by the query.
         * @return Number of rows affected.
         *
         * For SELECT statements this is number of returned rows.
         * For UPDATE this is number of rows updated.
         * For DELETE this is number of rows deleted.
         * FOR INSERT this is number of rows inserted (starting with SQLite 3.7.11 you can insert multiple rows with single INSERT statement).
         */
        virtual qint64 rowsAffected() = 0;

        /**
         * @brief Goes to the begining of the results.
         *
         * Call this if you alread read some data and you want to go back to the first one.
         */
        virtual void restart() = 0;

        /**
         * @brief Reads all rows immediately and returns them.
         * @return All data rows as a list.
         *
         * Don't use this method against huge data results.
         */
        virtual QList<SqlResultsRowPtr> getAll();

        /**
         * @brief Loads all data immediately into memory.
         *
         * This method makes sense only if you plan to use getAll() later on.
         * If you won't use getAll(), then calling this method is just a waste of memory.
         *
         * It is useful if you execute query asynchronously and you will be using all results.
         * In that case the asynchronous execution takes care of loading data from the database and the final code
         * just operates on in-memory data.
         */
        virtual void preload();

        /**
         * @brief Reads first column of first row and returns its value.
         * @return Value read.
         *
         * This method is useful when dealing with for example PRAGMA statement results,
         * or for SELECT queries with expected single row and single column.
         */
        virtual QVariant getSingleCell();

        /**
         * @brief Tells if there was an error while query execution.
         * @return true if there was an error, false otherwise.
         */
        virtual bool isError();

        /**
         * @brief Tells if the query execution was interrupted.
         * @return true if query was interrupted, or false otherwise.
         *
         * Interruption of execution is interpreted as an execution error,
         * so if this method returns true, then isError() will return true as well.
         */
        virtual bool isInterrupted();

        /**
         * @brief getInsertRowId Retrieves ROWID of the INSERT'ed row.
         * @return ROWID as 64-bit signed integer or set of multiple columns. If empty, then there was no row inserted.
         * @see RowId
         */
        virtual RowId getInsertRowId();

    protected:
        /**
         * @brief Row ID of the most recently inserted row.
         */
        RowId insertRowId;

        /**
         * @brief Flag indicating if the data was preloaded with preload().
         */
        bool preloaded = false;

        /**
         * @brief Data preloaded with preload().
         */
        QList<SqlResultsRowPtr> preloadedData;
};

/**
 * @brief Shared pointer to results object.
 * Results are usually passed as shared pointer, so it's used as needed and deleted when no longer required.
 */
typedef QSharedPointer<SqlResults> SqlResultsPtr;

#endif // SQLRESULTS_H