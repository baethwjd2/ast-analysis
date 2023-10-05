import sys
import zipfile
import os
import datetime
from os import rename
from docx import Document
from pptx import Presentation
from openpyxl import load_workbook

file_path = r"C:\Workspace\parser\xl_ex.xlsx"

def analyze(file_path):
    if file_path.endswith('.docx'):
        document = Document(file_path)
        text = ""
        for paragraph in document.paragraphs:
            text += paragraph.text+"\n"
        print("[파일 내용]")
        print(text)
    elif file_path.endswith('.pptx'):
        presentation = Presentation(file_path)
        text = ""
        for slide in presentation.slides:
            for shape in slide.shapes:
                if hasattr(shape,"text"):
                    text += shape.text + "\n"
        print("[파일 내용]")
        print(text)
    elif file_path.endswith('.xlsx'):
        workbook = load_workbook(filename=file_path)
        sheet = workbook.active
        data=[]
        for row in sheet.iter_rows(values_only=True):
            data.append(row)
        print("[파일 내용]")
        print(row)
    else:
        print("지원하지 않는 파일 형식입니다.")


def convert(file_path):
    # 확장자별 zip 변환
    if file_path.endswith('.docx'):
        zip_file_path = file_path.replace('.docx', '.zip')
        try:
            rename(file_path, zip_file_path)
            print(f"'{file_path}' 파일이 '{zip_file_path}'로 성공적으로 변환되었습니다.")
            analyze_zip_central_directory(zip_file_path)
        except FileNotFoundError:
            print(f"파일 '{file_path}'을 찾을 수 없습니다.")
        except Exception as e:
            print(f"파일 변환 중 오류가 발생했습니다: {e}")

    elif file_path.endswith('.pptx'):
        zip_file_path = file_path.replace('.pptx', '.zip')
        try:
            rename(file_path, zip_file_path)
            print(f"'{file_path}' 파일이 '{zip_file_path}'로 성공적으로 변환되었습니다.")
            analyze_zip_central_directory(zip_file_path)
        except FileNotFoundError:
            print(f"파일 '{file_path}'을 찾을 수 없습니다.")
        except Exception as e:
            print(f"파일 변환 중 오류가 발생했습니다: {e}")

    elif file_path.endswith('.xlsx'):
        zip_file_path = file_path.replace('.xlsx', '.zip')
        try:
            rename(file_path, zip_file_path)
            print(f"'{file_path}' 파일이 '{zip_file_path}'로 성공적으로 변환되었습니다.")
            analyze_zip_central_directory(zip_file_path)
        except FileNotFoundError:
            print(f"파일 '{file_path}'을 찾을 수 없습니다.")
        except Exception as e:
            print(f"파일 변환 중 오류가 발생했습니다: {e}")
    else:
        print("지원하지 않는 파일 형식입니다.")

def analyze_zip_central_directory(zip_file_path):
    # ZIP 파일 열기
    with zipfile.ZipFile(zip_file_path, 'r') as zip_file:
        # 모든 파일에 대한 Central Directory 정보 출력
        for info in zip_file.infolist():
            print("File Name:", info.filename)
            print("Version Made By:", info.create_version)
            print("Version Needed To Extract:", info.extract_version)
            print("General Purpose Bit Flags:", hex(info.flag_bits))
            print("Compression Method:", info.compress_type)
            file_timestamp = os.path.getmtime(zip_file_path)
            last_mod_time = datetime.datetime.fromtimestamp(file_timestamp)
            print("Last Mod Time:", last_mod_time)
            print("CRC32 Checksum:", hex(info.CRC))
            print("Compressed Size:", info.compress_size)
            print("Uncompressed Size:", info.file_size)
            print("File Comment:", info.comment)
            print("Internal File Attributes:", hex(info.internal_attr))
            print("External File Attributes:", hex(info.external_attr))
            print("Local Header Offset:", hex(info.header_offset))
            print()

if __name__ == "__main__":
    analyze(file_path)
    convert(file_path)
