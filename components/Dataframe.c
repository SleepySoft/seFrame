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
    case DF_TYPE_BYTE:
        // A byte is typically 1 byte in size.
    case DF_TYPE_CHAR:
        // A character is typically 1 byte in size.
        return 1;
    case DF_TYPE_INT16:
    case DF_TYPE_UINT16:
        // A 16-bit integer or bit field is typically 2 bytes in size.
        return 2;
    case DF_TYPE_INT32:
    case DF_TYPE_UINT32:
        // A 32-bit integer or bit field is typically 4 bytes in size.
        return 4;
    case DF_TYPE_FLOAT32:
        // A 32-bit floating point number is typically 4 bytes in size.
        return 4;
    case DF_TYPE_FLOAT64:
        // A 32-bit floating point number is typically 4 bytes in size.
        return 8;
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


static bool copyDataRaw(DataPoint* dp, void *data, uint32_t length, bool toDataPoint)
{
    bool result = false;
    if ((dp != NULL) && (data != NULL))
    {
        uint32_t copySize = MIN(length, dataPointSize(dp));
        toDataPoint ? memcpy(dp->data, data, copySize) : memcpy(data, dp->data, copySize);
        result = true;
    }
    return result;
}


bool copyDataCheckingType(DF_INDEX_T index, DF_TYPE_T expectType, void* data, uint32_t length, bool toDataPoint)
{
    DataPoint* dp = getDataPoint(index);
    return ((dp != NULL) && (getType(dp->properties)) == expectType) ? 
        copyDataRaw(dp, data, length, toDataPoint) : false;
}


bool getAsBytes(DF_INDEX_T index, uint8_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_BYTE, data, length, false);
}

bool setAsBytes(DF_INDEX_T index, const uint8_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_BYTE, data, length, true);
}










