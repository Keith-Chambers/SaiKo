#ifndef KPL_MEMBERFUNCTION_H
#define KPL_MEMBERFUNCTION_H

#include <assert.h>

#include <fmt/format.h>

#include <kpl/reflection/function.h>
#include <kpl/reflection/accessmodifier.h>

namespace kpl {
    namespace reflection {

        class Class;

        class MemberFunction
        {
        public:
            MemberFunction( const kpl::reflection::Class& classRef,
                            const kpl::reflection::Function function,
                            kpl::reflection::AccessModifier accessModifier = kpl::reflection::AccessModifier::PUBLIC,
                            bool isStatic = false,
                            bool isConst = false );

            std::string asDeclarationCode(bool fullyQualify = false) const;
            std::string asInvocationCode(const std::string instanceName, const std::string passedParameterString, bool isPointer = false) const;
            std::string asDefinitionCode(const std::string& body, bool fullyQualify = false) const;
            std::string qualifiedName() const;

            /* Overloaded operators */
            bool operator==(const MemberFunction& memFunc) const;
            bool operator!=(const MemberFunction& memFunc) const;
            bool operator<(const MemberFunction& memFunc) const;
            bool operator>(const MemberFunction& memFunc) const;

            const kpl::reflection::Function& funcData() const;
            const kpl::reflection::Class& classData() const;
            bool isStatic() const;
            bool isConst() const;

            const kpl::reflection::AccessModifier& accessModifier() const;
            const std::string& accessModifierString() const;
            const kpl::reflection::DataType& returnDataType() const;

        private:
            const kpl::reflection::Class& mClass;
            kpl::reflection::Function mFunction;
            kpl::reflection::AccessModifier mAccessModifier;

            bool mIsStatic;
            bool mIsConst;
        };
    }
}

#endif // KPL_MEMBERFUNCTION_H
