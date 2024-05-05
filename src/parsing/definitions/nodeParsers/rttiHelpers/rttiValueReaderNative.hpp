#pragma once
#include <RED4ext/RED4ext.hpp>
#include <RedLib.hpp>

#include "../../../../context/context.hpp"
#include "../../../cursorDef.hpp"

namespace redRTTI::native
{
class NativeReader
{
protected:
    virtual void ReadInt8(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<std::int8_t*>(aOut) = aCursor.readByte();
    }
    virtual void ReadInt16(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<std::int16_t*>(aOut) = aCursor.readShort();
    }
    virtual void ReadInt32(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<std::int32_t*>(aOut) = aCursor.readInt();
    }
    virtual void ReadInt64(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<std::int64_t*>(aOut) = aCursor.readInt64();
    }

    virtual void ReadUInt8(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<std::uint8_t*>(aOut) = aCursor.readValue<std::uint8_t>();
    }
    virtual void ReadUInt16(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<std::uint16_t*>(aOut) = aCursor.readUShort();
    }
    virtual void ReadUInt32(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<std::uint32_t*>(aOut) = aCursor.readUInt();
    }
    virtual void ReadUInt64(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<std::uint64_t*>(aOut) = aCursor.readUInt64();
    }

    virtual void ReadFloat(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<float*>(aOut) = aCursor.readFloat();
    }
    virtual void ReadDouble(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<double*>(aOut) = aCursor.readDouble();
    }
    virtual void ReadBool(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<bool*>(aOut) = aCursor.readValue<bool>();
    }
    virtual void ReadTDBID(FileCursor& aCursor, Red::ScriptInstance aOut)
    {
        *reinterpret_cast<Red::TweakDBID*>(aOut) = aCursor.readTdbId();
    }

    // Those are a little more class-defined
    virtual void ReadNodeRef(FileCursor& aCursor, Red::ScriptInstance aOut) = 0;
    virtual void ReadCName(FileCursor& aCursor, Red::ScriptInstance aOut) = 0;

    // These need a bit of info about the inner RTTI type...
    virtual void ReadEnum(FileCursor& aCursor, Red::ScriptInstance aOut, Red::CBaseRTTIType* aPropType) = 0;
    virtual void ReadArray(FileCursor& aCursor, Red::ScriptInstance aOut, Red::CBaseRTTIType* aPropType)
    {
        const auto arraySize = aCursor.readInt();
        const auto arrayType = static_cast<Red::CRTTIArrayType*>(aPropType);
        const auto innerType = arrayType->GetInnerType();

        arrayType->Resize(aOut, arraySize);

        // THIS IS LIKELY WRONG
        for (auto i = 0; i < arraySize; i++)
        {
            // Might be leaky, unsure
            auto instance =
                arrayType->GetAllocator()->AllocAligned(innerType->GetSize(), innerType->GetAlignment()).memory;

            innerType->Construct(instance);

            ReadValue(aCursor, instance, innerType);
            
            innerType->Assign(arrayType->GetElement(aOut, i), instance);
        }
    }

    virtual void ReadStaticArray(FileCursor& aCursor, Red::ScriptInstance aOut, Red::CBaseRTTIType* aPropType)
    {
        const auto arraySize = aCursor.readInt();
        const auto arrayType = static_cast<Red::CRTTIStaticArrayType*>(aPropType);

        const auto innerType = arrayType->GetInnerType();
        const auto innerTypeSize = innerType->GetSize();

        const auto maxLength = arrayType->GetMaxLength();

        auto outPtr = reinterpret_cast<uintptr_t>(aOut);

        for (auto i = 0; i < std::min(arraySize, maxLength - 1); i++)
        {
            // Should work nicer than the previous implementation
            auto elem = arrayType->GetElement(aOut, i);

            ReadValue(aCursor, elem, innerType);
        }

        arrayType->Resize(aOut, arraySize);
    }

    virtual void ReadHandle(FileCursor& aCursor, Red::ScriptInstance aOut, Red::CBaseRTTIType* aPropType) = 0;
    virtual void ReadWeakHandle(FileCursor& aCursor, Red::ScriptInstance aOut, Red::CBaseRTTIType* aPropType) = 0;

    virtual void ReadValue(FileCursor& aCursor, Red::ScriptInstance aOut, Red::CBaseRTTIType* aPropType)
    {
        const auto rttiType = aPropType->GetType();
        const auto typeName = aPropType->GetName();

        using Red::ERTTIType;

        if (rttiType == ERTTIType::Name)
        {
            if (typeName == "CName")
            {
                return ReadCName(aCursor, aOut);
            }
        }
        else if (rttiType == ERTTIType::Fundamental)
        {
            if (typeName == RED4ext::CName{"Int8"})
            {
                return ReadInt8(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Uint8"})
            {
                return ReadUInt8(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Int16"})
            {
                return ReadInt16(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Uint16"})
            {
                return ReadUInt16(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Int32"})
            {
                return ReadInt32(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Uint32"})
            {
                return ReadUInt32(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Int64"})
            {
                return ReadInt64(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Uint64"})
            {
                return ReadUInt64(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Float"})
            {
                return ReadFloat(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Double"})
            {
                return ReadDouble(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"Bool"})
            {
                return ReadBool(aCursor, aOut);
            }
        }
        else if (rttiType == ERTTIType::Simple)
        {
            if (typeName == RED4ext::CName{"TweakDBID"})
            {
                return ReadTDBID(aCursor, aOut);
            }
            else if (typeName == RED4ext::CName{"NodeRef"})
            {
                return ReadNodeRef(aCursor, aOut);
            }
        }
        else if (rttiType == ERTTIType::Enum)
        {
            return ReadEnum(aCursor, aOut, aPropType);
        }
        else if (rttiType == ERTTIType::Array)
        {
            return ReadArray(aCursor, aOut, aPropType);
        }
        else if (rttiType == ERTTIType::StaticArray)
        {
            return ReadStaticArray(aCursor, aOut, aPropType);
        }
        else if (rttiType == ERTTIType::Class)
        {
            return ReadClass(aCursor, aOut, aPropType);
        }
        else if (rttiType == ERTTIType::Handle)
        {
            return ReadHandle(aCursor, aOut, aPropType);
        }
        else if (rttiType == ERTTIType::WeakHandle)
        {
            return ReadWeakHandle(aCursor, aOut, aPropType);
        }

        throw std::runtime_error(std::format("NativeReader::ReadValue, unknown value type {}", typeName.ToString()));
    }
public:
    virtual void ReadClass(FileCursor& aCursor, Red::ScriptInstance aOut, Red::CBaseRTTIType* aClass) = 0;
};

class NativeDumper
{
private:
    // Very simple and PoC
    static void DumpValue(Red::ScriptInstance aPtr, Red::CBaseRTTIType* aType)
    {
        const auto rttiType = aType->GetType();
        const auto typeName = aType->GetName();

        using Red::ERTTIType;

        if (rttiType == ERTTIType::Name)
        {
            if (typeName == "CName")
            {
                PluginContext::Spew(std::format("{}", reinterpret_cast<Red::CName*>(aPtr)->ToString()));
            }
        }
        else if (rttiType == ERTTIType::Enum)
        {
            const auto enumType = static_cast<Red::CEnum*>(aType);
            
            auto enumValue = enumType->valueList.Back();

            switch (enumType->actualSize)
            {
            case 1:
                enumValue = *reinterpret_cast<std::int8_t*>(aPtr);
                break;
            case 2:
                enumValue = *reinterpret_cast<std::int16_t*>(aPtr);
                break;
            case 4:
                enumValue = *reinterpret_cast<std::int32_t*>(aPtr);
                break;
            case 8:
                enumValue = *reinterpret_cast<std::int64_t*>(aPtr);
                break;
            }

            for (auto i = 0; i < enumType->valueList.size; i++)
            {
                if (enumValue == enumType->valueList[i])
                {
                    PluginContext::Spew(std::format("{}.{}", aType->GetName().ToString(), enumType->hashList[i].ToString()));
                    return;
                }
            }

            for (auto i = 0; i < enumType->aliasValueList.size; i++)
            {
                if (enumValue == enumType->aliasValueList[i])
                {
                    PluginContext::Spew(std::format("{}.{}", aType->GetName().ToString(), enumType->aliasList[i].ToString()));
                    return;
                }
            }
        }
        else if (rttiType == ERTTIType::Array)
        {
            auto arrayType = static_cast<Red::CRTTIArrayType*>(aType);
            auto innerType = arrayType->GetInnerType();
            const auto len = arrayType->GetLength(aPtr);

            for (auto i = 0u; i < len; i++)
            {
                auto elem = arrayType->GetElement(aPtr, i);
                
                DumpValue(elem, innerType);
            }
        }
        else if (rttiType == ERTTIType::StaticArray)
        {
            auto arrayType = static_cast<Red::CRTTIStaticArrayType*>(aType);
            auto innerType = arrayType->GetInnerType();
            const auto len = arrayType->GetLength(aPtr);

            for (auto i = 0u; i < len; i++)
            {
                auto elem = arrayType->GetElement(aPtr, i);

                DumpValue(elem, innerType);
            }
        }
        else if (rttiType == ERTTIType::Class)
        {
            return DumpClass(aPtr, aType);
        }
    }
public:
    static void DumpClass(Red::ScriptInstance aData, Red::CBaseRTTIType* aType)
    {
        const auto classType = static_cast<Red::CClass*>(aType);

        Red::DynArray<Red::CProperty*> props{};

        classType->GetProperties(props);

        for (auto i : props)
        {
            if (i->flags.isSavable == 0 && i->flags.isPersistent == 0)
            {
                continue;
            }

            auto valuePtr = i->GetValuePtr<std::remove_pointer_t<Red::ScriptInstance>>(aData);

            DumpValue(valuePtr, i->type);
        }
    }
};
}