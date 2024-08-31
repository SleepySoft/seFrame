#ifndef __DATA_FRAME_SLEEPY_H__
#define __DATA_FRAME_SLEEPY_H__

#include <stdint.h>
#include <stdbool.h>


typedef uint32_t DF_INDEX_T;


typedef uint16_t DF_TYPE_T;
#define DF_TYPE_MASK 0x00000FFF
#define DF_TYPE_SHIFT 0

typedef uint8_t DF_STYLE_T;
#define DF_STYLE_MASK 0x0000F000
#define DF_STYLE_SHIFT 12

typedef uint8_t DF_FEATURE_T;
#define DF_FEATURE_MASK 0x00FF0000
#define DF_FEATURE_SHIFT 16

typedef uint8_t DF_RESERVE_T;
#define DF_RESERVE_MASK 0xFF000000
#define DF_RESERVE_SHIFT 24




/**
 * @brief Data type constants
 */
#define DF_TYPE_BYTE        ((DF_TYPE_T)0)  /**< BYTE: Unsigned 8-bit integer, typically used for buffers */
#define DF_TYPE_CHAR        ((DF_TYPE_T)1)  /**< CHAR: Signed 8-bit integer, typically used for strings */
#define DF_TYPE_INT16       ((DF_TYPE_T)2)  /**< INT16: Signed 16-bit integer */
#define DF_TYPE_UINT16      ((DF_TYPE_T)3)  /**< UINT16: Unsigned 16-bit integer */
#define DF_TYPE_INT32       ((DF_TYPE_T)4)  /**< INT32: Signed 32-bit integer */
#define DF_TYPE_UINT32      ((DF_TYPE_T)5)  /**< UINT32: Unsigned 32-bit integer */
#define DF_TYPE_FLOAT32     ((DF_TYPE_T)6)  /**< FLOAT32: 32-bit floating point number */
#define DF_TYPE_FLOAT64     ((DF_TYPE_T)10) /**< FLOAT32: 64-bit floating point number */


#define DF_STYLE_COMMON     ((DF_STYLE_T)0)
#define DF_STYLE_ENUM       ((DF_STYLE_T)1)
#define DF_STYLE_BITFIELD   ((DF_STYLE_T)2)


#define DF_FEATURE_GENERAL      ((DF_FEATURE_T)0)    /**< General: No specific function */
#define DF_FEATURE_MEASUREMENT  ((DF_FEATURE_T)1)    /**< Measurement: Measurement value */
#define DF_FEATURE_STATUS       ((DF_FEATURE_T)2)    /**< Measurement: Status: Status value */
#define DF_FEATURE_SETTING      ((DF_FEATURE_T)3)    /**< Setting: Used as setting value, usually used with Setting Manager */
#define DF_FEATURE_COMMAND      ((DF_FEATURE_T)4)    /**< Command: Used as command, commands usually have ACK */


typedef struct
{
    uint32_t properties;
    uint16_t length;
    uint16_t group;
    void* data;
} DataPoint;


/**
 * @brief Returns the size of the data type in bytes.
 *
 * @param type The data format type.
 * @return uint32_t The size of the data type in bytes.
 */
uint32_t typeSize(DF_TYPE_T type);


/**
 * @brief Combines type, feature, and reserved into a single uint32_t property value
 *
 * @param type The type value
 * @param feature The feature value
 * @param reserved The reserved value
 * @return uint32_t The combined property value
 */
uint32_t makeProperty(DF_TYPE_T type, DF_STYLE_T style, DF_FEATURE_T feature, DF_RESERVE_T reserved);


/**
 * @brief Extracts the type value from the property
 *
 * @param property The combined property value
 * @return DF_TYPE_TYPE The extracted type value
 */


DF_TYPE_T getType(uint32_t property);

/**
 * @brief Extracts the style value from the property
 *
 * @param property The combined property value
 * @return DF_STYLE_T The extracted style value
 */
DF_STYLE_T getStyle(uint32_t property);


/**
 * @brief Extracts the feature value from the property
 *
 * @param property The combined property value
 * @return DF_FEATURE_TYPE The extracted feature value
 */
DF_FEATURE_T getFeature(uint32_t property);

/**
 * @brief Extracts the reserved value from the property
 *
 * @param property The combined property value
 * @return DF_RESERVE_TYPE The extracted reserved value
 */
DF_RESERVE_T getReserved(uint32_t property);


/**
 * @brief Copies data from a DataPoint to an array of bytes.
 *
 * This function is used to retrieve data from a DataPoint where the data type
 * is expected to be DF_TYPE_BYTE. It copies the data into the provided byte array.
 *
 * @param index The index of the DataPoint from which to retrieve data.
 * @param data The pointer to the array where the data will be copied.
 * @param length The number of bytes to copy.
 * @return bool True if the data was successfully copied, false otherwise.
 */
bool getAsBytes(DF_INDEX_T index, uint8_t* data, uint32_t length);

/**
 * @brief Copies data from an array of bytes to a DataPoint.
 *
 * This function is used to set data into a DataPoint where the data type
 * is expected to be DF_TYPE_BYTE. It copies the data from the provided byte array.
 *
 * @param index The index of the DataPoint where the data will be set.
 * @param data The pointer to the array with the data to set.
 * @param length The number of bytes to copy.
 * @return bool True if the data was successfully copied, false otherwise.
 */
bool setAsBytes(DF_INDEX_T index, const uint8_t* data, uint32_t length);

/**
 * @brief Copies data from a DataPoint to an array of char if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array where data will be copied.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool getAsChar(DF_INDEX_T index, int8_t* data, uint32_t length);

/**
 * @brief Copies data from an array of char to a DataPoint if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array with data to set.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool setAsChar(DF_INDEX_T index, const int8_t* data, uint32_t length);

/**
 * @brief Copies data from a DataPoint to an array of int16_t if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array where data will be copied.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool getAsInt16(DF_INDEX_T index, int16_t* data, uint32_t length);

/**
 * @brief Copies data from an array of int16_t to a DataPoint if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array with data to set.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool setAsInt16(DF_INDEX_T index, const int16_t* data, uint32_t length);

/**
 * @brief Copies data from a DataPoint to an array of uint16_t if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array where data will be copied.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool getAsUint16(DF_INDEX_T index, uint16_t* data, uint32_t length);

/**
 * @brief Copies data from an array of uint16_t to a DataPoint if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array with data to set.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool setAsUint16(DF_INDEX_T index, const uint16_t* data, uint32_t length);

/**
 * @brief Copies data from a DataPoint to an array of int32_t if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array where data will be copied.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool getAsInt32(DF_INDEX_T index, int32_t* data, uint32_t length);

/**
 * @brief Copies data from an array of int32_t to a DataPoint if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array with data to set.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool setAsInt32(DF_INDEX_T index, const int32_t* data, uint32_t length);

/**
 * @brief Copies data from a DataPoint to an array of uint32_t if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array where data will be copied.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool getAsUint32(DF_INDEX_T index, uint32_t* data, uint32_t length);

/**
 * @brief Copies data from an array of uint32_t to a DataPoint if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array with data to set.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool setAsUint32(DF_INDEX_T index, const uint32_t* data, uint32_t length);

/**
 * @brief Copies data from a DataPoint to an array of float if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array where data will be copied.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool getAsFloat32(DF_INDEX_T index, float* data, uint32_t length);

/**
 * @brief Copies data from an array of float to a DataPoint if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array with data to set.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool setAsFloat32(DF_INDEX_T index, const float* data, uint32_t length);

/**
 * @brief Copies data from a DataPoint to an array of double if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array where data will be copied.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool getAsFloat64(DF_INDEX_T index, double* data, uint32_t length);

/**
 * @brief Copies data from an array of double to a DataPoint if the type matches.
 *
 * @param index The index of the DataPoint.
 * @param data The pointer to the array with data to set.
 * @param length The number of elements to copy.
 * @return bool True if the operation was successful, false otherwise.
 */
bool setAsFloat64(DF_INDEX_T index, const double* data, uint32_t length);


#endif // __DATA_FRAME_SLEEPY_H__
