#include "Dataframe.h"
#include "inlines.h"
#include <memory.h>         // NULL


extern const uint32_t DF_DATA_COUNT;
extern DataPoint DataFrameData[];


/**
 * ---------------------------------------------------------- Static functions ----------------------------------------------------------
*/

/**
 * @brief Retrieves a pointer to a DataPoint by index.
 *
 * This function returns a pointer to the DataPoint structure at the specified index.
 * If the index is out of bounds, it returns NULL to indicate that no valid DataPoint
 * exists at that index.
 *
 * @param index The index of the DataPoint to retrieve.
 * @return DataPoint* Pointer to the DataPoint structure, or NULL if the index is invalid.
 */
static DataPoint* getDataPoint(DF_INDEX_T index)
{
    return (index < DF_DATA_COUNT) ? &DataFrameData[index] : NULL;
}

/**
 * @brief Calculates the total size in bytes of the data in a DataPoint.
 *
 * This function calculates the size of the data held in a DataPoint based on its type
 * and length. It multiplies the size of the type (as returned by typeSize()) by the
 * length of the data to get the total size in bytes.
 *
 * @param dp Pointer to the DataPoint whose size is to be calculated.
 * @return uint32_t The total size of the data in the DataPoint in bytes.
 */
static uint32_t dataPointSize(const DataPoint* dp)
{
    return typeSize(getType(dp->properties)) * dp->length;
}

/**
 * @brief Copies raw data to or from a DataPoint.
 *
 * This function performs a raw memory copy operation to or from a DataPoint.
 * It does not perform any type checking or size adjustments based on the DataPoint's type.
 * The caller must ensure that the memory regions do not overlap and that the sizes are correct.
 *
 * @param dp Pointer to the DataPoint structure.
 * @param data Pointer to the memory area where data will be copied to or from.
 * @param size Number of bytes to copy.
 * @param toDataPoint Boolean flag indicating the direction of the copy:
 *                  - \c true means copy to the DataPoint.
 *                  - \c false means copy from the DataPoint.
 * @return bool Returns \c true if the copy was successful, \c false otherwise.
 */
static bool copyDataRaw(DataPoint* dp, void* data, uint32_t size, bool toDataPoint)
{
    bool result = false;
    if ((dp != NULL) && (data != NULL) && (size > 0))
    {
        uint32_t dpSize = dataPointSize(dp);
        uint32_t copySize = (size < dpSize) ? size : dpSize;
        toDataPoint ? memcpy(dp->data, data, copySize) : memcpy(data, dp->data, copySize);
        result = true;
    }
    return result;
}

/**
 * @brief Copies data to or from a DataPoint after checking the type.
 *
 * This function checks if the DataPoint's type matches the expected type and
 * then copies the data to or from the DataPoint. It is used to ensure that
 * the data being copied is of the correct type and within the bounds of the DataPoint's capacity.
 *
 * @param index The index of the DataPoint.
 * @param expectType The expected data type of the DataPoint.
 * @param data Pointer to the memory area where data will be copied to or from.
 * @param length The number of elements of the specified type to copy.
 * @param toDataPoint Boolean flag indicating the direction of the copy:
 *                  - \c true means copy to the DataPoint.
 *                  - \c false means copy from the DataPoint.
 * @return bool Returns \c true if the data was successfully copied, \c false otherwise.
 */
static bool copyDataCheckingType(DF_INDEX_T index, DF_TYPE_T expectType, void* data, uint32_t length, bool toDataPoint)
{
    bool result = false;
    DataPoint* dp = getDataPoint(index);
    if ((dp != NULL) && (getType(dp->properties)) == expectType)
    {
        result = copyDataRaw(dp, data, typeSize(expectType) * length, toDataPoint);
    }
    return result;
}


/**
 * ---------------------------------------------------------- Public functions ----------------------------------------------------------
*/

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


bool getAsBytes(DF_INDEX_T index, uint8_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_BYTE, data, length, false);
}

bool setAsBytes(DF_INDEX_T index, const uint8_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_BYTE, (void*)data, length, true);
}


bool getAsChar(DF_INDEX_T index, int8_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_CHAR, data, length, false);
}

bool setAsChar(DF_INDEX_T index, const int8_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_CHAR, CONST_CAST(void*, data), length, true);
}


bool getAsInt16(DF_INDEX_T index, int16_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_INT16, data, length, false);
}

bool setAsInt16(DF_INDEX_T index, const int16_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_INT16, CONST_CAST(void*, data), length, true);
}


bool getAsUint16(DF_INDEX_T index, uint16_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_UINT16, data, length, false);
}

bool setAsUint16(DF_INDEX_T index, const uint16_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_UINT16, CONST_CAST(void*, data), length, true);
}


bool getAsInt32(DF_INDEX_T index, int32_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_INT32, data, length, false);
}

bool setAsInt32(DF_INDEX_T index, const int32_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_INT32, CONST_CAST(void*, data), length, true);
}


bool getAsUint32(DF_INDEX_T index, uint32_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_UINT32, data, length, false);
}

bool setAsUint32(DF_INDEX_T index, const uint32_t* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_UINT32, CONST_CAST(void*, data), length, true);
}


bool getAsFloat32(DF_INDEX_T index, float* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_FLOAT32, data, length, false);
}

bool setAsFloat32(DF_INDEX_T index, const float* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_FLOAT32, CONST_CAST(void*, data), length, true);
}


bool getAsFloat64(DF_INDEX_T index, double* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_FLOAT64, data, length, false);
}

bool setAsFloat64(DF_INDEX_T index, const double* data, uint32_t length)
{
    return copyDataCheckingType(index, DF_TYPE_FLOAT64, CONST_CAST(void*, data), length, true);
}


