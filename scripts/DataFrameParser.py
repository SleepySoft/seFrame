import os
import traceback
import pandas as pd


TYPE_ENUM_PREFIX = 'DF_TYPE_'
TYPE_MAPPING = {
    'BYTE': 'uint8_t',      # UNSIGNED 8-BIT INTEGER
    'CHAR': 'int8_t',       # SIGNED 8-BIT INTEGER
    'INT16': 'int16_t',     # SIGNED 16-BIT INTEGER
    'UINT16': 'uint16_t',   # UNSIGNED 16-BIT INTEGER
    'INT32': 'int32_t',     # SIGNED 32-BIT INTEGER
    'UINT32': 'uint32_t',   # UNSIGNED 32-BIT INTEGER
    'FLOAT32': 'float',     # 32-BIT FLOATING POINT NUMBER
    'FLOAT64': 'double',    # 64-BIT FLOATING POINT NUMBER
}

STYLE_ENUM_PREFIX = 'DF_STYLE_'
STYLE_LIST = ['COMMON', 'ENUM', 'BITFIELD']

FEATURE_ENUM_PREFIX = 'F_FEATURE_'
FEATURE_LIST = ['GENERAL', 'MEASUREMENT', 'STATUS', 'SETTING', 'COMMAND']


class DataFrameParser:
    def __init__(self):
        self.dataframe = pd.DataFrame()


# ---------------------------------------------------------------------------------------------------------------------

def main():
    pass


# ----------------------------------------------------------------------------------------------------------------------

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print('Error =>', e)
        print('Error =>', traceback.format_exc())
        exit()
    finally:
        pass



