#ifndef KDATABASE_H
#define KDATABASE_H

#include <sqlite3.h>
#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <functional>

#include "kpl/kfile.h"

namespace kpl
{
    namespace sqlite
    {

        namespace Operators {
            const std::string EQUAL = "=";
            const std::string NOT_EQUAL = "!=";
        }

        class WhereClause
        {
        public:
            WhereClause(std::string attribute, const std::string& value, const std::string& op = "=")
                : mAttribute { attribute }, mValue { value }, mOperator { op }
            {}

            static WhereClause makeEqualsClause(std::string attribute, std::string value)
            {
                return WhereClause(attribute, value, "=");
            }

            static WhereClause makeNotEqualsClause(std::string attribute, std::string value)
            {
                return WhereClause(attribute, value, "!=");
            }

            std::string toString() const
            {
                return mAttribute + mOperator + mValue;
            }

        private:
            std::string mAttribute;
            std::string mValue;

            std::string mOperator;
        };

        sqlite3 * openSqliteDatabase(std::string databasePath);

        bool createDatabaseTablesFromFile(sqlite3 * database
                                          , std::string filePath
                                          );

        bool insertInto(sqlite3 * database
                        , const std::string& tableName
                        , const std::vector<std::pair<std::string, std::string>>& parameters
                        );

        bool insertIfUnique( sqlite3 * database
                            , const std::string& tableName
                            , const std::vector<std::pair<std::string, std::string>>& parameters
                            );

        bool select( sqlite3 * database
                    , const std::string& tableName
                    , const std::vector<std::string>& selectFields
                    , int (*callback)(void*,int,char**,char**)
                    , void *callbackData
                    );

        bool select( sqlite3 * database
                    , const std::string& tableName
                    , const std::vector<std::string>& selectFields
                    , int (*callback)(void*,int,char**,char**)
                    , void *callbackData
                    , const std::vector<WhereClause>& whereClauses
                    );

        int count(sqlite3 * database, const std::string& tableName, const std::vector<WhereClause>& whereClauses = {});
    }
}

#endif // KDATABASE_H
