import sys
import zipfile
import os
import datetime
import xml.etree.ElementTree as ET
import zipfile
import xml.dom.minidom
from os import rename
from docx import Document
from pptx import Presentation
from openpyxl import load_workbook

# 확장자별 zip 변환
def convert(file_path):

    if file_path.endswith('.docx'):
        zip_file_path = file_path.replace('.docx', '.zip')
        try:
            rename(file_path, zip_file_path)
            analyze_zip_central_directory(zip_file_path)
            return zip_file_path
        except FileNotFoundError:
            print(f"파일 '{file_path}'을 찾을 수 없습니다.")
        except Exception as e:
            print(f"파일 변환 중 오류가 발생했습니다: {e}")
    
    elif file_path.endswith('.pptx'):
        zip_file_path = file_path.replace('.pptx', '.zip')
        try:
            rename(file_path, zip_file_path)
            analyze_zip_central_directory(zip_file_path)
            return zip_file_path
        except FileNotFoundError:
            print(f"파일 '{file_path}'을 찾을 수 없습니다.")
        except Exception as e:
            print(f"파일 변환 중 오류가 발생했습니다: {e}")

    elif file_path.endswith('.xlsx'):
        zip_file_path = file_path.replace('.xlsx', '.zip')
        try:
            rename(file_path, zip_file_path)
            analyze_zip_central_directory(zip_file_path)
            return zip_file_path
        except FileNotFoundError:
            print(f"파일 '{file_path}'을 찾을 수 없습니다.")
        except Exception as e:
            print(f"파일 변환 중 오류가 발생했습니다: {e}")
    else:
        print("지원하지 않는 파일 형식입니다.")

# zip 파일에서 입력받은 xml을 찾아 출력
def xml_from_zip(zip_path, user_input):
    try:
        with zipfile.ZipFile(zip_path, 'r') as zip_file:
            with zip_file.open(user_input) as xml_file:
                xml_content = xml_file.read().decode('utf-8')
                return xml_content
    except FileNotFoundError:
        print(f"파일 '{zip_path}'을 찾을 수 없습니다.")
    except Exception as e:
        print(f"파일을 읽는 동안 오류가 발생했습니다: {e}")


# zip 파일 내부에 있는 모든 파일의 정보 출력
def analyze_zip_central_directory(zip_file_path):
    # ZIP 파일 열기
    with zipfile.ZipFile(zip_file_path, 'r') as zip_file:
        # 모든 파일에 대한 Central Directory 정보 출력
        for info in zip_file.infolist():
            print("File Name:", info.filename) # 파일 이름
            print("Version Made By:", info.create_version) # 해당 파일을 생성한 버전
            print("Version Needed To Extract:", info.extract_version) # 압축 해제하기 위해 필요한 zip 압축 해제 버전
            print("General Purpose Bit Flags:", hex(info.flag_bits)) # 바이트 식별자
            print("Compression Method:", info.compress_type) # 압축 유형
            file_timestamp = os.path.getmtime(zip_file_path)
            last_mod_time = datetime.datetime.fromtimestamp(file_timestamp)
            print("Last Mod Time:", last_mod_time) # 마지막 파일 수정 날짜, 시간
            print("CRC32 Checksum:", hex(info.CRC)) # 파일 내용의 오류 체크
            print("Compressed Size:", info.compress_size)  # 압축된 데이터의 크기
            print("Uncompressed Size:", info.file_size) # 원본 데이터의 바이트 크기
            print("File Comment:", info.comment) # 파일에 대한 코멘트나 설명
            print("Internal File Attributes:", hex(info.internal_attr)) # 파일 내부 속성
            print("External File Attributes:", hex(info.external_attr)) # 파일 외부 속성
            print("Local Header Offset:", hex(info.header_offset)) # 해당 파일의 로컬 파일 헤더의 오프셋 위치
            print()

if __name__ == "__main__":
    file_path = input("분석할 파일 경로 :")
    zip_path = convert(file_path)

    while True:
        user_input = input("내용을 확인하고 싶은 파일명(xml만 가능, exit은 종료) :")
        if user_input =="exit":
            break
        else:
            xml_content = xml_from_zip(zip_path, user_input)
            dom=xml.dom.minidom.parseString(xml_content)
            pretty_xml = dom.toprettyxml()
            print(pretty_xml)
            print()
