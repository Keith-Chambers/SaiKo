#ifndef KPL_FUNCTION_H
#define KPL_FUNCTION_H

#include <vector>
#include <string>
#include <ostream>

#include "kpl/reflection/datatype.h"
#include "kpl/reflection/variable.h"

namespace kpl {
    namespace reflection {

        class Function
        {
        public:
            Function(   const std::string name,
                        const kpl::reflection::DataType returnValue,
                        std::vector<Variable> parameters = {} );

            bool hasParameters() const;
            bool hasVoidReturnValue() const;
            std::string name() const;
            std::string returnValueString() const;
            std::string parameterListString() const;

            const kpl::reflection::DataType& returnDataType() const;

            //std::string invocationCode(const std::string& parameters = "") const;
            //std::string definitionCode(const std::string& body) const;
            //std::string declarationCode() const;

            bool operator==(const Function& func) const;
            bool operator!=(const Function& func) const;
            bool operator<(const Function& func) const;
            bool operator>(const Function& func) const;

        private:
            const std::string mName;
            const DataType mReturnValue;
            const std::vector<Variable> mParameters;
        };
    }
}

#endif // KPL_FUNCTION_H
