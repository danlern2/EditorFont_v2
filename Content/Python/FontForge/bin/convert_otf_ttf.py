import fontforge
import argparse
import pathlib
import os
import sys
import logging

parser = argparse.ArgumentParser(
        description="Convert TTF files to OTF (and vice versa) using FontForge."
    )
parser.add_argument("input_filename", help="Path to the input font file (.ttf or .otf)")
parser.add_argument("output_filename", help="Path for the output font file (e.g., path/to/converted.otf)")

if '\\' in sys.argv[1] or ':' in sys.argv[1]:
  input_filename = os.path.abspath(pathlib.Path(pathlib.PureWindowsPath(sys.argv[1])))
else:
  input_filename = os.path.abspath(pathlib.Path(pathlib.PurePosixPath(sys.argv[1])))
if '\\' in sys.argv[2] or ':' in sys.argv[2]:
  output_filename = os.path.abspath(pathlib.Path(pathlib.PureWindowsPath(sys.argv[2])))
else:
  output_filename = os.path.abspath(pathlib.Path(pathlib.PurePosixPath(sys.argv[2])))

# logpath = "C:\\Users\\*User*\\Documents" + '\\convert.log'
# logger = logging.getLogger(__name__)
# logging.basicConfig(filename=logpath, encoding='utf-8', level=logging.DEBUG, filemode="a")
# logger.debug(f'\nInpath: {input_filename}, Outpath: {output_filename}')

print(f"Inpath: {input_filename}, Outpath: {output_filename}")


font = fontforge.open(input_filename)
font.generate(output_filename)