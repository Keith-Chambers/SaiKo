#ifndef KPL_VARIABLE_H
#define KPL_VARIABLE_H

#include "kpl/reflection/logicaldatatype.h"
#include "kpl/reflection/datatype.h"

/* Variable - Represents a named data type used in code
 * Supports cardinality so that arrays can be defined.
 */

namespace kpl {
    namespace reflection
    {

        enum Cardinality { INFINITE = UINT16_MAX };

        class Variable
        {
        public:
            Variable(std::string name, const DataType dataType, bool isConst = false, uint16_t min = 1, uint16_t max = 1);
            Variable(std::string name, const LogicalDataType dataType, bool isConst = false, uint16_t min = 1, uint16_t max = 1);

            void print(const std::string&& prefix = "") const;

            bool isArray() const;
            std::string name() const;
            std::string typeName() const;
            const DataType * dataTypePtr() const;

            uint16_t getCardinalityMax() const;
            uint16_t getCardinalityMin() const;
            bool isConst() const;

        private:
            const std::string mName;
            const LogicalDataType mDataType;
            bool mIsConst;
            uint16_t mMin;
            uint16_t mMax;
        };
    }
}

#endif // KPL_VARIABLE_H
