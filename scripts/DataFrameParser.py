import os
import re
import openpyxl
import traceback
import pandas as pd


COL_PATH = "Path"
COL_NAME = "Name"
COL_DESCRIPTION = "Description"
COL_TYPE = "Type"
COL_LENGTH = "Length"
COL_STYLE = "Style"
COL_RANGE = "Range"
COL_DEFAULT = "Default"
COL_UNIT = "Unit"
COL_BLOB = "Blob"
COL_FEATURE = "Feature"


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

    def parse_data_frame(self, workbook):
        sheet = workbook.worksheets[1]  # 打开第二页
        data = sheet.values
        columns = next(data)[0:]  # 读取第一行作为列名
        df = pd.DataFrame(data, columns=columns)

        df = DataFrameParser.pre_process(df)
        df = DataFrameParser.extend_rows(df)
        return df

    @staticmethod
    def pre_process(df: pd.DataFrame):
        warnings = []
        if COL_NAME in df.columns:
            df = df[df[COL_NAME].notna()]
            warnings.append(f'Warning: Deleted rows with empty {COL_NAME}.')

        if COL_LENGTH in df.columns:
            df[COL_LENGTH] = pd.to_numeric(df[COL_LENGTH], errors='coerce')
            df = df[df[COL_LENGTH].notna()]
            warnings.append(f'Warning: Deleted rows with invalid {COL_LENGTH}.')

        if COL_TYPE in df.columns:
            invalid_types = df[~df[COL_TYPE].isin(TYPE_MAPPING.keys())]
            df = df[df[COL_TYPE].isin(TYPE_MAPPING.keys())]
            for index in invalid_types.index:
                warnings.append(f'Warning: Deleted row with invalid COL_TYPE at index {index}.')

        if COL_STYLE in df.columns:
            invalid_styles = df[~df[COL_STYLE].isin(STYLE_LIST)]
            df = df[df[COL_STYLE].isin(STYLE_LIST)]
            for index in invalid_styles.index:
                warnings.append(f'Warning: Deleted row with invalid COL_STYLE at index {index}.')

        if COL_FEATURE in df.columns:
            invalid_features = df[~df[COL_FEATURE].isin(FEATURE_LIST)]
            df = df[df[COL_FEATURE].isin(FEATURE_LIST)]
            for index in invalid_features.index:
                warnings.append(f'Warning: Deleted row with invalid COL_FEATURE at index {index}.')

        for warning in warnings:
            print(warning)
        return df

    @staticmethod
    def extend_rows(df: pd.DataFrame):
        # 找到所有包含子设备组的行
        pattern = re.compile(r'(.+?)\[(\d+)\](\d*)')
        subdevice_groups = []
        for index, row in df.iterrows():
            path = row['COL_PATH']
            matches = pattern.finditer(path)
            for match in matches:
                base_path = match.group(1)
                count = int(match.group(2))
                subdevice_number = match.group(3)
                subdevice_groups.append((base_path, count, subdevice_number, index))

        # 展开所有子设备组
        new_rows = []
        for _, row in df.iterrows():
            path = row[COL_PATH]
            matches = pattern.finditer(path)
            for match in matches:
                base_path = match.group(1)
                count = int(match.group(2))
                subdevice_number = match.group(3)
                for i in range(count):
                    new_row = row.copy()
                    new_row[COL_PATH] = f"{base_path}{subdevice_number}{i}"
                    new_rows.append(new_row)
            else:  # 如果没有子设备组，则直接添加当前行
                new_rows.append(row)

        # 处理连续的子设备组
        last_base_path = None
        last_subdevice_number = None
        for i in range(len(new_rows)):
            row = new_rows[i]
            path = row['COL_PATH']
            matches = pattern.match(path)
            if matches:
                base_path, count, subdevice_number = matches.groups()
                if base_path == last_base_path:
                    new_rows[i][COL_PATH] = \
                        f"{base_path}{last_subdevice_number}{int(subdevice_number) + i - matches.start(0)}"
                else:
                    last_base_path = base_path
                    last_subdevice_number = subdevice_number
            else:
                last_base_path = None
                last_subdevice_number = None

        return pd.DataFrame(new_rows)

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



