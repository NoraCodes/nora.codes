#! /usr/bin/env python3
import argparse
import mmap
from datetime import datetime, timezone

parser = argparse.ArgumentParser('Update timestamps in YAML frontmatter.')
parser.add_argument('files', metavar='FILE', type=str, nargs='+',
        help='The file(s) on which to operate')
args = parser.parse_args()

files = [open(filename, 'r+') for filename in args.files]
indices = []

timestring = datetime.now(tz=timezone.utc).strftime('%Y-%m-%d %R:%S%z').encode('utf-8')
datelinestring = b'date: ' + timestring + b'\n'
print(timestring)

for f in files:
    with mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_WRITE) as s:
        start_index = s.find(b'date: ')
        if start_index  == -1:
            raise Exception('Could not find date: in f')
        end_index = s.find(b'\n', start_index)
        if end_index == -1:
            raise Exception('No newline after date: line')

        expected_len = len(datelinestring)
        real_len = end_index - start_index
        if real_len != expected_len:
            raise Exception('Wrong length in date: line (exp. ' + str(expected_len) + ', got ' + str(real_len) + ')');
        s[start_index:end_index] = datelinestring
        s.flush()

for f in files:
    f.close()

