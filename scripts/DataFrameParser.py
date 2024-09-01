import os
import openpyxl
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
        self.enum_table = {}
        self.bit_field_table = {}
        self.dataframe = pd.DataFrame()

    def parse_excel(self, excel_file: str):
        workbook = openpyxl.load_workbook(excel_file)
        self.parse_enum(workbook)
        self.parse_bit_field(workbook)

    def parse_enum(self, workbook):
        self.enum_table = DataFrameParser.parse_value_declare(workbook, 2)

    def parse_bit_field(self, workbook):
        self.bit_field_table = DataFrameParser.parse_value_declare(workbook, 3)

    def parse_data_frame(self):
        pass

    @staticmethod
    def parse_value_declare(workbook, page):
        value_declare = {}
        current_value_group = None
        sheet = workbook.worksheets[page]
        for row in sheet.iter_rows(min_row=2, values_only=True):
            if row[0]:
                if row[0] not in value_declare:
                    value_declare[row[0]] = {}
                current_value_group = value_declare[row[0]]
                continue
            if current_value_group is not None and row[1] and row[2]:
                current_value_group[row[1]] = (row[2], row[3])
        return value_declare


# ---------------------------------------------------------------------------------------------------------------------

def main():
    parser = DataFrameParser()
    parser.parse_excel('./../Dataframe.xlsx')
    print(parser.enum_table)
    print(parser.bit_field_table)


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



