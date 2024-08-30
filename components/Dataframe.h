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
#define DF_TYPE_BYTE        ((DF_TYPE_T)1)   /**< BYTE: Unsigned 8-bit integer, typically used for buffers */
#define DF_TYPE_CHAR        ((DF_TYPE_T)2)   /**< CHAR: Signed 8-bit integer, typically used for strings */
#define DF_TYPE_INT16       ((DF_TYPE_T)3)   /**< INT16: Signed 16-bit integer */
#define DF_TYPE_UINT16      ((DF_TYPE_T)4)   /**< UINT16: Unsigned 16-bit integer */
#define DF_TYPE_INT32       ((DF_TYPE_T)5)   /**< INT32: Signed 32-bit integer */
#define DF_TYPE_UINT32      ((DF_TYPE_T)6)   /**< UINT32: Unsigned 32-bit integer */
#define DF_TYPE_FLOAT32     ((DF_TYPE_T)11)  /**< FLOAT32: 32-bit floating point number */


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
uint32_t makeProperty(DF_TYPE_T type, DF_FEATURE_T feature, DF_RESERVE_T reserved);


/**
 * @brief Extracts the type value from the property
 *
 * @param property The combined property value
 * @return DF_TYPE_TYPE The extracted type value
 */


DF_TYPE_T getType(uint32_t property);

/**
 * @brief Extracts the type value from the property
 *
 * @param property The combined property value
 * @return DF_STYLE_T The extracted type value
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





#endif // __DATA_FRAME_SLEEPY_H__
