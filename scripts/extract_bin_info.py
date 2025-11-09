#!/usr/bin/env python3
import struct
import os
import sys

path = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'build', 'merged-binary.bin')
if len(sys.argv) > 1:
    path = sys.argv[1]

if not os.path.exists(path):
    print(f"File not found: {path}")
    sys.exit(2)

data = open(path, 'rb').read()

# find app partition in partition table at 0x8000..0xBFFF
app_partition = None
for i in range(0x8000, 0x8000 + 0x4000, 0x20):
    if data[i] == 0xAA and data[i+1] == 0x50 and data[i+2] == 0x00:
        offset = int.from_bytes(data[i+4:i+8], 'little')
        size = int.from_bytes(data[i+8:i+12], 'little')
        label = data[i+12:i+28].decode('utf-8').strip('\0')
        app_partition = {'offset': offset, 'size': size, 'label': label}
        break

if app_partition is None:
    print('No app partition found in merged binary')
    sys.exit(1)

print(f"Found app partition: offset=0x{app_partition['offset']:08x}, size=0x{app_partition['size']:08x}, label={app_partition['label']}")
app_data = data[app_partition['offset']: app_partition['offset'] + app_partition['size']]

if len(app_data) < 0x18:
    print('App data too small')
    sys.exit(1)

if app_data[0] != 0xE9:
    print('App data does not look like a valid ESP image (first byte != 0xE9)')
    # continue, maybe still can parse

segment_count = app_data[1]
segments = []
offset = 0x18
for i in range(segment_count):
    seg_size = int.from_bytes(app_data[offset+4:offset+8], 'little')
    offset += 8
    seg = app_data[offset:offset+seg_size]
    segments.append(seg)
    offset += seg_size

if not segments:
    print('No segments found')
    sys.exit(1)

# parse app_desc in first segment
seg0 = segments[0]
if len(seg0) < 0xb0:
    print('First segment too small for app_desc')
    sys.exit(1)

magic = int.from_bytes(seg0[0:4], 'little')
if magic != 0xabcd5432:
    print(f'app_desc magic mismatch: 0x{magic:08x}')

version = seg0[0x10:0x30].decode('utf-8', errors='ignore').rstrip('\0')
project = seg0[0x30:0x50].decode('utf-8', errors='ignore').rstrip('\0')
time_s = seg0[0x50:0x60].decode('utf-8', errors='ignore').rstrip('\0')
date_s = seg0[0x60:0x70].decode('utf-8', errors='ignore').rstrip('\0')
idf_ver = seg0[0x70:0x90].decode('utf-8', errors='ignore').rstrip('\0')
elf_sha = seg0[0x90:0xb0].hex()

print('--- app_desc ---')
print('project_name:', project)
print('version:', version)
print('compile_time:', date_s + 'T' + time_s)
print('idf_version:', idf_ver)
print('elf_sha256:', elf_sha)

# file metadata
print('\n--- file metadata ---')
st = os.stat(path)
print('path:', path)
print('size:', st.st_size)
print('mtime:', st.st_mtime)

# sha256
import hashlib
h = hashlib.sha256(open(path,'rb').read()).hexdigest().upper()
print('\nSHA256:', h)
