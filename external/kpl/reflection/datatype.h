#ifndef KPL_DATATYPE_H
#define KPL_DATATYPE_H

#include <string>
#include <optional>
#include <vector>
#include <array>

#include <kpl/chainedarray.h>

/* DataType - Represents a DataType that can be used in code, defined by it's name and size
 * Supports alias and non-primative types
 */

namespace kpl {
    namespace reflection
    {
        class DataType
        {
        public:
            DataType(std::string name, size_t szBits, bool isPrimative = true, bool isPointer = false, bool isReference = false);
            DataType(std::string name, size_t szBits, const std::string aliasFor, bool isPrimative = true, bool isPointer = false, bool isReference = false);

            static constexpr uint8_t BITS_PER_BYTE = 8;

            size_t sizeBits() const;
            std::string name() const;
            bool isAlias() const;
            bool isPrimative() const;
            std::string getUnAliasedTypeName() const;

            /* Static Methods */
            static DataType makeAlias(std::string newName, const DataType& parentType);
            static bool isDefaultSupported(std::string name);
            static std::optional<DataType> constructDefaultSupportedType(std::string name);

            static DataType makeVoidType();
            static DataType makeUint8Type();
            static DataType makeUint16Type();
            static DataType makeCharType();

        private:
            const std::string mName;
            const std::size_t mSzBits;
            const bool mIsAlias;
            const bool mIsPrimative;
            const std::optional<std::string> mAliasFor;
            const bool mIsPointer;
            const bool mIsReference;
        };

        static const std::array<DataType, 9> DEFAULT_DATATYPES {
            DataType("void", 0),
            DataType("char", 1 * kpl::reflection::DataType::BITS_PER_BYTE),
            DataType("bool", 1),
            DataType("uint8_t", 1 * kpl::reflection::DataType::BITS_PER_BYTE),
            DataType("int8_t", 1 * kpl::reflection::DataType::BITS_PER_BYTE),
            DataType("uint16_t", 2 * kpl::reflection::DataType::BITS_PER_BYTE),
            DataType("int16_t", 2 * kpl::reflection::DataType::BITS_PER_BYTE),
            DataType("uint32_t", 4 * kpl::reflection::DataType::BITS_PER_BYTE),
            DataType("int32_t", 4 * kpl::reflection::DataType::BITS_PER_BYTE)
        };
    }
}

#endif // KPL_DATATYPE_H
