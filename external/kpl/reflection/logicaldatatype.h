#ifndef LOGICALDATATYPE_H
#define LOGICALDATATYPE_H

#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>

#include "kpl/reflection/datatype.h"

/* LogicalDataType - Represents a data type used in code by name and size.
 * Wrapper class for DataType that allows user-defined(non-primative), hierarchical types.
 */

namespace kpl {
    namespace reflection
    {

    /* Prevents cyclic include loop between Variable & LogicalDataType */
    class Variable;

        class LogicalDataType : public DataType
        {
        public:
            /* True logical types */
            LogicalDataType(std::string name, std::vector<Variable> memberList, const std::string aliasFor);
            LogicalDataType(std::string name, std::vector<Variable> memberList);

            /* Primative types */
            LogicalDataType(const DataType & dataType);
            LogicalDataType(std::string name, size_t szBits);
            LogicalDataType(std::string name, size_t szBits, const std::string aliasFor);

            static LogicalDataType makeAlias(std::string newName, const LogicalDataType& dataType);

            bool memberExists(std::string name) const;
            bool getMemberList(std::vector<Variable>& memberListOutput) const;

        private:
            static size_t calculateSizeBits(std::vector<Variable> memberList);

            std::vector<Variable> mMemberVariables;
        };
    }
}

#endif // LOGICALDATATYPE_H
