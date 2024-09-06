import re
import string
import hashlib
import itertools
import openpyxl
import traceback
import numpy as np
import pandas as pd
from typing import Any, List


COLUMN_PATH = "Path"
COLUMN_NAME = "Name"
COLUMN_DESCRIPTION = "Description"
COLUMN_TYPE = "Type"
COLUMN_LENGTH = "Length"
COLUMN_STYLE = "Style"
COLUMN_RANGE = "Range"
COLUMN_DEFAULT = "Default"
COLUMN_UNIT = "Unit"
COLUMN_BLOB = "Blob"
COLUMN_FEATURE = "Feature"

STANDARD_COLUMNS = [COLUMN_PATH, COLUMN_NAME, COLUMN_DESCRIPTION, COLUMN_TYPE, COLUMN_LENGTH,
                    COLUMN_STYLE, COLUMN_RANGE, COLUMN_DEFAULT, COLUMN_UNIT, COLUMN_BLOB, COLUMN_FEATURE]


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


TEMPLATE_ENUM_DECLARE = """
typedef enum 
{
<<enum_items>>
} <<enum_name>>

"""


def str_to_int(s, err: Any = np.NaN):
    try:
        if s is None:
            return err
        elif isinstance(s, str):
            return int(s[2:], 16) if s.lower().startswith("0x") else int(s, 10)
        else:
            return int(s)
    except Exception as e:
        print(f'Error parse {s} to int')
        return err


def str_to_float(s, err=np.NaN):
    try:
        i = str_to_int(s, None)
        return float(i) if i is not None else (float(s) if s is not None else err)
    except Exception as e:
        print(f'Error parse {s} to float')
        return err


def str_or_empty(value: Any):
    if value is None:
        return ''
    if isinstance(value, str):
        return value
    return str(value)


def format_bit_meanings(value, value_range) -> str:
    result = []
    for bit in value_range:
        if value & (1 << bit):
            result.append(f"bit{bit} - {value_range[bit]}")
    return '\n'.join(result)


def float_eq(x, y, epsilon=1e-7):
    return abs(x - y) < epsilon


def float_neq(x, y, epsilon=1e-7):
    return not float_eq(x, y, epsilon)


def str_available(val: str) -> bool:
    return isinstance(val, str) and len(val) > 0


def index_to_excel_column_name(index: int) -> str:
    index = int(index)
    column_index = ''
    while index > 0:
        remainder = (index - 1) % 26 + 1
        column_index += string.ascii_uppercase[remainder - 1]
        index -= remainder
        index //= 26
    return column_index[::-1]


def str_hash_32_bit(text: str) -> int:
    return int.from_bytes(hashlib.sha256(text.encode('utf-8')).digest()[:4], 'little')


def str_hash_32_bit_formatted(text: str) -> str:
    return '%08X' % str_hash_32_bit(text)


def select_rows_by_column_value(df: pd.DataFrame, col: str, col_val: any):
    """
    Get rows from a DataFrame by column value filter
    """
    try:
        return df.loc[df[col] == col_val]
    except Exception as e:
        print(e)
        print(traceback.format_exc())
        return None
    finally:
        pass


def get_cel_value_by_column_value(df: pd.DataFrame, col: str, col_val: any, cell_column: str):
    """
    Find row by column filter and get the cell value
    """
    try:
        return df.loc[df[col] == col_val, cell_column].values[0]
    except Exception as e:
        print(e)
        print(traceback.format_exc())
        return None
    finally:
        pass


def align_to_n(x, n):
    remainder = x % n
    if remainder == 0:
        return x
    else:
        return x + (n - remainder)


def duplicate_rows(df, rows_to_copy, columns_to_keep, copies: int, insert_back: bool) -> pd.DataFrame:
    """
    Duplicate specified rows in a DataFrame and insert them below the original rows.

    Parameters:
    df (pd.DataFrame): The original DataFrame.
    rows_to_copy (list): A list of row indices to duplicate.
    columns_to_keep (list): A list of column names to keep in the duplicated rows.
    copies (int): Number of times to duplicate the rows.
    insert_back (bool): True if insert back duplicated rows behind rows_to_copy else just return duplicated rows.

    Returns:
    pd.DataFrame: A new DataFrame with the duplicated rows inserted.
    """
    # 获取要复制的行，并完整复制
    rows = df.loc[rows_to_copy].copy()

    # 清空不需要保留的列
    for col in df.columns:
        if col not in columns_to_keep:
            rows[col] = None

    # 复制指定的行N份
    copied_rows = pd.concat([rows] * copies, ignore_index=True)

    # 将复制的行插入到原始DataFrame中的指定位置
    if insert_back:
        part_before = df.iloc[:rows_to_copy[-1] + 1]
        part_after = df.iloc[rows_to_copy[-1] + 1:]
        new_df = pd.concat([part_before, copied_rows, part_after], ignore_index=True)
        return new_df
    else:
        return copied_rows


class LevelingPathParser:
    INDEXES_FINDER = re.compile(r'\[(.*?)\]')

    def __init__(self, duplicate_columns=None):
        if duplicate_columns is None:
            duplicate_columns = STANDARD_COLUMNS
        self.duplicate_columns = duplicate_columns

    def extend_rows(self, df: pd.DataFrame) -> pd.DataFrame:
        if df.empty:
            return df

        df['expand_path'] = ''

        prev_path = ''
        same_path_rows = []
        df = df.reset_index(drop=True)

        columns = df.columns.tolist()
        extend_df = pd.DataFrame(columns=columns)

        # TODO: Append expanded rows to extend_df

        for index, row in df.iterrows():
            data_path = row[COLUMN_PATH]

            if str_available(prev_path):
                # There's a leveling path in previous line
                if data_path == prev_path:
                    # The next row has the same path
                    same_path_rows.append((index, row))
                    continue
                else:
                    # The data belongs to another path
                    # Process the recorded rows that in the same path
                    rows = self.expand_data_frame_rows(df, prev_path, same_path_rows)
                    extend_df = pd.concat([extend_df, rows], ignore_index=True)
                    prev_path = ''
                    same_path_rows.clear()

            indexes = LevelingPathParser.parse_path_indexes(data_path)
            if len(indexes) > 0:
                # Means it's an array path
                prev_path = data_path
                same_path_rows = [(index, row)]

        if str_available(prev_path):
            rows = self.expand_data_frame_rows(df, prev_path, same_path_rows)
            extend_df = pd.concat([extend_df, rows], ignore_index=True)
        extend_df = extend_df.reset_index(drop=True)

        def clean_and_join_paths(expand_path, column_name):
            clean_expand_path = expand_path.rstrip('/')
            clean_column_name = column_name.strip('/')
            full_path = f"{clean_expand_path}/{clean_column_name}"
            return full_path
        extend_df['full_path'] = extend_df.apply(
            lambda _row: clean_and_join_paths(_row['expand_path'], _row[COLUMN_NAME]), axis=1)

        return extend_df

    def expand_data_frame_rows(self, df: pd.DataFrame, data_path: str, same_path_rows: list):
        expanded_paths = LevelingPathParser.expand_array_path(data_path)
        duplicate_row_indexes = [index for index, _ in same_path_rows]
        group_size = len(same_path_rows)
        duplicate_count = len(expanded_paths)
        expanded_rows = duplicate_rows(df, duplicate_row_indexes, self.duplicate_columns, duplicate_count, False)

        for i, expanded_path in enumerate(expanded_paths):
            start_idx = i * group_size
            end_idx = (i + 1) * group_size
            for j in range(start_idx, end_idx):
                expanded_rows.at[j, 'expand_path'] = expanded_path
        return expanded_rows

    @staticmethod
    def expand_array_path(data_path: str) -> [tuple]:
        indexes = LevelingPathParser.parse_path_indexes(data_path)
        path_index_iteration = list(itertools.product(*[range(0, i) for i in indexes]))

        expanded_path = []
        for path_index in path_index_iteration:
            sub_path_formatter = LevelingPathParser.INDEXES_FINDER.sub('%s', data_path)
            sub_path = sub_path_formatter % tuple([i for i in path_index])
            expanded_path.append(sub_path)
        return expanded_path

    @staticmethod
    def parse_path_indexes(data_path: str) -> List[int]:
        """
        Extract all digit in [].
        :param data_path: The path string that may have [] or not.
        :return: The digit list of []s
        """
        indexes = LevelingPathParser.INDEXES_FINDER.findall(data_path)
        return [int(i) for i in indexes]


class DataFrameParser:
    def __init__(self):
        self.enum_table = {}
        self.bit_field_table = {}
        self.dataframe = pd.DataFrame()

    def parse_excel(self, excel_file: str):
        workbook = openpyxl.load_workbook(excel_file)
        self.parse_enum(workbook)
        self.parse_bit_field(workbook)
        self.parse_data_frame(workbook)

        print(self.generate_enum_declaration())

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
        df = LevelingPathParser().extend_rows(df)

        self.dataframe = df

        return self.dataframe

    @staticmethod
    def pre_process(df: pd.DataFrame):
        warnings = []
        if COLUMN_NAME in df.columns:
            df = df[df[COLUMN_NAME].notna()]
            warnings.append(f'Warning: Deleted rows with empty {COLUMN_NAME}.')

        if COLUMN_LENGTH in df.columns:
            df[COLUMN_LENGTH] = pd.to_numeric(df[COLUMN_LENGTH], errors='coerce')
            df = df[df[COLUMN_LENGTH].notna()]
            warnings.append(f'Warning: Deleted rows with invalid {COLUMN_LENGTH}.')

        if COLUMN_TYPE in df.columns:
            invalid_types = df[~df[COLUMN_TYPE].isin(TYPE_MAPPING.keys())]
            df = df[df[COLUMN_TYPE].isin(TYPE_MAPPING.keys())]
            for index in invalid_types.index:
                warnings.append(f'Warning: Deleted row with invalid COLUMN_TYPE at index {index}.')

        if COLUMN_STYLE in df.columns:
            invalid_styles = df[~df[COLUMN_STYLE].isin(STYLE_LIST)]
            df = df[df[COLUMN_STYLE].isin(STYLE_LIST)]
            for index in invalid_styles.index:
                warnings.append(f'Warning: Deleted row with invalid COLUMN_STYLE at index {index}.')

        if COLUMN_FEATURE in df.columns:
            invalid_features = df[~df[COLUMN_FEATURE].isin(FEATURE_LIST)]
            df = df[df[COLUMN_FEATURE].isin(FEATURE_LIST)]
            for index in invalid_features.index:
                warnings.append(f'Warning: Deleted row with invalid COLUMN_FEATURE at index {index}.')

        for warning in warnings:
            print(warning)
        return df

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
            value = str_to_int(row[1], None)
            if current_value_group is not None and value is not None and row[2]:
                current_value_group[value] = (row[2], str_or_empty(row[3]))
        return value_declare

    def generate_enum_declaration(self) -> str:
        generated_code = ''

        for enum_name, enum_values in self.enum_table.items():
            enum_items = []
            enum_comments = []

            max_enum_item_length = 0
            for enum_value, (enum_item, enum_item_text) in enum_values.items():
                enum_item_str = f"    {enum_item} = {enum_value},"
                enum_items.append(enum_item_str)
                enum_comments.append(enum_item_text)
                max_enum_item_length = max(max_enum_item_length, len(enum_item_str))
            if len(enum_items) > 0:
                enum_items[-1] = enum_items[-1].rstrip(',')
            comments_indentation = align_to_n(max_enum_item_length + 4, 4)

            enum_item_with_comments = [f"{item}{' ' * (comments_indentation - len(item))}#{comments}"
                                       for item, comments in zip(enum_items, enum_comments)]

            generated_code += TEMPLATE_ENUM_DECLARE.\
                replace('<<enum_items>>', '\n'.join(enum_item_with_comments)).\
                replace('<<enum_name>>', enum_name)

        return generated_code


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



