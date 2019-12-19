#ifndef CLASS_H
#define CLASS_H

#include <vector>
#include <string>
#include <optional>
#include <algorithm>

#include <kpl/reflection/function.h>
#include <kpl/reflection/variable.h>

#include <kpl/reflection/memberfunction.h>
#include <kpl/reflection/membervariable.h>
#include <kpl/reflection/accessmodifier.h>

namespace kpl {
    namespace reflection {

        class Class
        {
        public:

            Class(const std::string& name, const std::string& classNamespace);

            Class()=delete;
            Class(const Class&)=delete;
            Class operator=(const Class&)=delete;

            Class(Class&& rhs) noexcept
                : mName { std::move(rhs.mName) },
                  mNamespace { std::move(rhs.mNamespace) },
                  mMethods { std::move(rhs.mMethods) },
                  mMethodDecorations { std::move(rhs.mMethodDecorations) },
                  mVariables { std::move(rhs.mVariables) },
                  mVariableDecorations { std::move(rhs.mVariableDecorations) }
            { }

            static constexpr uint8_t NO_DECORATIONS = 0b00000000;

            bool addMethod( kpl::reflection::Function function,
                            kpl::reflection::AccessModifier accessModifier = kpl::reflection::AccessModifier::PUBLIC,
                            bool isStatic = false,
                            bool isConst = false );

            bool addMethod( kpl::reflection::Function function,
                            uint8_t decorations,
                            kpl::reflection::AccessModifier accessModifier = kpl::reflection::AccessModifier::PUBLIC,
                            bool isStatic = false,
                            bool isConst = false );

            bool addVariable( kpl::reflection::Variable variable,
                              kpl::reflection::AccessModifier accessModifier = kpl::reflection::AccessModifier::PUBLIC,
                              bool isStatic = false );

            bool addVariable( kpl::reflection::Variable variable,
                              uint8_t decorations,
                              kpl::reflection::AccessModifier accessModifier = kpl::reflection::AccessModifier::PUBLIC,
                              bool isStatic = false );

            bool containsVariable( const std::string& name ) const;
            bool containsVariable( const std::string& name, const uint8_t decorationMask ) const;

            bool containsMethod( const std::string& name ) const;
            bool containsMethod( const std::string& name, const uint8_t decorationMask ) const;

            std::optional<kpl::reflection::MemberFunction> getMethod( const std::string& name ) const;

            // TODO: Should be made to return references
            kpl::reflection::MemberFunction * getMethodPtr( const std::string& name );
            const kpl::reflection::MemberFunction * getMethodPtr( const std::string& name ) const;

            std::optional<kpl::reflection::MemberVariable> getVariable( const std::string& name ) const;

            std::vector<kpl::reflection::MemberFunction> getMethods( const uint8_t decorationMask ) const;
            const std::vector<kpl::reflection::MemberFunction>& getMethods() const;

            std::vector<kpl::reflection::MemberVariable> getVariables( const  uint8_t decorationMask ) const;
            const std::vector<kpl::reflection::MemberVariable>& getVariables() const;

            uint8_t methodDecoration( const std::string& methodName ) const;
            uint8_t variableDecoration( const std::string& variableName ) const;

            const std::string& classNamespace() const;
            const std::string& name() const;
            std::string qualifiedName() const;

            bool operator==(const Class& classData) const;
            bool operator!=(const Class& classData) const;
            bool operator<(const Class& classData) const;
            bool operator>(const Class& classData) const;

            /* Source Code Generators */

            std::string asHeaderFileGuard() const;

            std::string singletonInstanciationCode() const;
            std::string objectInstanciationCode(const std::string& parameters = "") const;
            std::string objectInstanciationCode(const std::string& objName, const std::string& parameters = "") const;
            std::string includeDirectiveCode( const std::string& pathPrefix = "" ) const;
            std::string singletonAssignmentCode(std::string instanceName = "") const;

            /* All lowercase + .cpp */
            std::string asSourceFileName() const;
            /* All lowercase + .h */
            std::string asHeaderFileName() const;
            /* Lowers the first letter */
            std::string asSingletonName() const;

        private:
            std::string mName;
            std::string mNamespace;

            std::vector<kpl::reflection::MemberFunction> mMethods;
            std::vector<uint8_t> mMethodDecorations;

            std::vector<kpl::reflection::MemberVariable> mVariables;
            std::vector<uint8_t> mVariableDecorations;
        };
    }
}

#endif // CLASS_H
