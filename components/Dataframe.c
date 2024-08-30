#include "Dataframe.h"
#include <memory.h>         // NULL


extern const uint32_t DF_DATA_COUNT;
extern DataPoint DataFrameData[];


uint32_t makeProperty(DF_TYPE_T type, DF_STYLE_T style, DF_FEATURE_T feature, DF_RESERVE_T reserved)
{
    return 
        ((type << DF_TYPE_SHIFT) & DF_TYPE_MASK) |
        ((style << DF_STYLE_SHIFT) & DF_STYLE_MASK) |
        ((feature << DF_FEATURE_SHIFT) & DF_FEATURE_MASK) |
        ((reserved << DF_RESERVE_SHIFT) & DF_RESERVE_MASK);
}


DF_TYPE_T getType(uint32_t property)
{
    return (DF_TYPE_T)((property & DF_TYPE_MASK) >> DF_TYPE_SHIFT);
}


DF_STYLE_T getStyle(uint32_t property)
{
    return (DF_STYLE_T)((property & DF_STYLE_MASK) >> DF_STYLE_SHIFT);
}


DF_FEATURE_T getFeature(uint32_t property)
{
    return (DF_FEATURE_T)((property & DF_FEATURE_MASK) >> DF_FEATURE_SHIFT);
}


DF_RESERVE_T getReserved(uint32_t property)
{
    return (DF_RESERVE_T)((property & DF_RESERVE_MASK) >> DF_RESERVE_SHIFT);
}


uint32_t typeSize(DF_TYPE_T type)
{
    switch (type) {
    case DT_BYTE:
        // A byte is typically 1 byte in size.
        return 1;
    case DT_CHAR:
        // A character is typically 1 byte in size.
        return 1;
    case DT_INT16:
    case DT_UINT16:
    case DT_ENUM16:
    case DT_BF16:
        // A 16-bit integer or bit field is typically 2 bytes in size.
        return 2;
    case DT_INT32:
    case DT_UINT32:
    case DT_ENUM32:
    case DT_BF32:
        // A 32-bit integer or bit field is typically 4 bytes in size.
        return 4;
    case DT_FLOAT32:
        // A 32-bit floating point number is typically 4 bytes in size.
        return 4;
    default:
        // If the type is unknown, return 0.
        return 0;
    }
}


static DataPoint* getDataPoint(DF_INDEX_T index)
{
    return (index < DF_DATA_COUNT) ? &DataFrameData[index] : NULL;
}


static uint32_t dataPointSize(const DataPoint* dp)
{
    return typeSize(getType(dp->properties)) * dp->length;
}


static bool copyDataRaw(DF_INDEX_T index, void *data, uint32_t size, bool toDataPoint)
{
    bool result = false;
    if (data != NULL)
    {
        DataPoint* dp = getDataPoint(index);
        if (dp != NULL)
        {
            uint32_t copySize = MIN(size, dataPointSize(dp));
            toDataPoint ? memcpy(dp->data, data, copySize) : memcpy(data, dp->data, copySize);
            result = true;
        }
    }
    return result;
}













