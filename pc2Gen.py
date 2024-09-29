# BSD 3-Clause License
#
# Copyright (c) 2024, fofolevrai
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.

# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# -------------------------------------------------------------------------------
import argparse
import shutil
import os
import sys
import yaml

# Author :      fofolevrai
# Revision :    0.0.1
# Date :        11-09-2024
# Comments :    Create 'point_cloud2_iterator.h'
#               and 'point_cloud2_iterator.c' files
#               to incorporate in your project which describes PointCloud2
#               messages of your described sensor device to incorporate 

import yaml

def read_file(file_path):
    # Check string is not empty
    try:
        with open(file_path, 'r') as file:
            data = file.read()
            return data
    except Exception as e:
        print(f"Error reading file: {e}")
        return None

def write_file(file_path, data):
    # Check string is not empty
    try:
        if len(data):
            with open(file_path, 'w') as file:
                file.write(data)
    except Exception as e:
        print(f"Error writing file: {e}")
        return None

# Read the YAML file
def read_yaml_file(file_path):
    try:
        with open(file_path, "r") as yaml_file:
            data = yaml.safe_load(yaml_file)
            return data
    except Exception as e:
        print(f"Error reading YAML file: {e}")
        return None

def generate_c_point_field_iterator_header_content(file_model, data):
    # Initialize the point field section
    pf_section = ""

    # Initialize the C struct template
    c_struct_template = '''#define _POINTFIELD_INITIALIZER_ (const sensor_msgs__msg__PointField[]) {{  \\
{pointFields}                               \\
    /* Keep at the tail */      \\
    {{{{NULL, (size_t) 0, (size_t) 0}}, (uint32_t) 0, (uint8_t) 0}}     \\
}}
'''

    # Iterate over each device
    for pf in data["pointFields"]:
        pf_name = pf["name"]
        pf_name_size = len(pf["name"])

        pf_rosidl_runtime_c__String = f'{{.data="{pf_name}", .size={pf_name_size}}}'

        # Feed optionnal 'count' value
        if not "count" in pf:
            pf_count = 1
        else:
            pf_count = pf["count"]
        
        # Feed optionnal 'datatype' value
        if not "datatype" in pf:
            pf_datatype = 'sensor_msgs__msg__PointField__FLOAT32'
        else:
            pf_datatype = pf["datatype"]
    
        # Add the pointField information to the struct section
        pf_section += f'    {{.name = {pf_rosidl_runtime_c__String}, .count = {pf_count}, .datatype = {pf_datatype}}}, \\\n'

    # Fill in the C struct template
    filled_c_struct = c_struct_template.format(pointFields=pf_section)

    # (Debug) Print the filled C struct
    # print(filled_c_struct)

    # Extract header file content
    header_file_content = read_file(file_model)

    # Is the file model empty
    if not header_file_content:
        return None
    
    # Replace '#define _POINTFIELD_INITIALIZER_ NULL'
    return header_file_content.replace('#define _POINTFIELD_INITIALIZER_ NULL', filled_c_struct)
    
def generate_c_device_iterator_header_content(file_model, data):
    # Initialize the devices section
    devices_section = ""

    # Initialize the C struct template
    c_struct_template = '''#define _DEVICE_INITIALIZER_ (const pointCloud2Generator_t[]) {{ \\
{devices}                           \\
    /* Keep at the tail */      \\
    {{NULL}}                \\
}}
'''

    # Iterate over each device
    for device in data["devices"]:
        device_name = device["name"]
        point_fields = device.get("pointFields", [])
        nbr_point_fields = len(point_fields)

        # Feed optionnal 'count' value
        if not "is_bigendian" in device:
            device_is_bigendian = True
        else:
            device_is_bigendian = device["is_bigendian"]
        
        # Feed optionnal 'datatype' value
        if not "is_dense" in device:
            device_is_dense = True
        else:
            device_is_dense = device["is_dense"]
        
        # Create the pointFieldCapacity string
        '''
                for field in point_fields:
            if not "callback" in field:
                field_callback = 'NULL'
            else:
                field_callback = field["callback"]
        '''
        point_field_capacity = ', '.join(f'{{.name="{field["name"]}", .callback={field.get("callback", "NULL")}}}' for field in point_fields)
    
        # Add the device information to the devices section
        devices_section += f'   {{.deviceName = "{device_name}", .pointFieldCapacity = {{{point_field_capacity}}}, .nbr_pointFields = {nbr_point_fields}}}, \\\n'

    # Fill in the C struct template
    filled_c_struct = c_struct_template.format(devices=devices_section)

    # (Debug) Print the filled C struct
    # print(filled_c_struct)

    # Extract header file content
    header_file_content = read_file(file_model)

    # Is the file model empty
    if not header_file_content:
        return None
    
    # Replace '#define _DEVICE_INITIALIZER_'
    return header_file_content.replace('#define _DEVICE_INITIALIZER_ NULL', filled_c_struct)

# @fofolevrai TBD
def create_static_pc2_message(device):
    return None

def main():
    parser = argparse.ArgumentParser(prog="pc2Gen", description="C poinCloud2 header file generator")
    parser.add_argument("--version", action='version', version="%(prog)s 0.0.1")
    parser.add_argument("--devFile", "-df", type=str, required=False, help="Yaml dictionnary file path containing available frames (default \"device_def.yaml\")")
    parser.add_argument("--pfFile", "-pf", type=str, required=False, help="Yaml dictionnary file path containing available point fields (default \"asset/pointField_def.yaml\")")
    parser.add_argument("--model", "-m", type=str, required=False, help="C header file taken as reference for completion (default \"asset/point_cloud2_iterator_model.h\")")
    parser.add_argument("--output", "-o", type=str, required=False, help="C header and source output file name (default \"point_cloud2_iterator.h\")")
    args = parser.parse_args()

    # Default devices description file if no '--file' argument given
    dev_file = 'device_def.yaml'
    # PointField description file if no '--pfFile' argument given
    pf_file = 'asset/pointField_def.yaml'
    # Default C header model file if no '--model' argument given
    h_file_model = 'asset/point_cloud2_iterator_model.h'
    # Default C output header file if no '--output' argument given
    out_file = 'point_cloud2_iterator.h'
    
    if args.devFile:
        dev_file = args.devFile
    if args.pfFile:
        pf_file = args.pfFile
    if args.model:
        h_file_model = args.model
    if args.output:
        out_file = args.output

    if not os.path.isfile(dev_file):
        print(f"File \'{dev_file}\' not found")
        sys.exit(1)

    if not os.path.isfile(pf_file):
        print(f"File \'{pf_file}\' not found")
        sys.exit(1)
  
    if not os.path.isfile(h_file_model):
        print(f"File \'{h_file_model}\' not found")
        sys.exit(1)

    # Extract yaml devices file information
    dev_data = read_yaml_file(dev_file)

    # Replace generated C struct from yaml inputs into header file
    modified_point_cloud2_iterator_header_file = generate_c_device_iterator_header_content(h_file_model, dev_data)

    # Write done C header file
    write_file(out_file, modified_point_cloud2_iterator_header_file)

    # Extract yaml point field information
    pf_data = read_yaml_file(pf_file)

    # Replace generated C struct from yaml inputs into header file
    modified_point_cloud2_iterator_header_file = generate_c_point_field_iterator_header_content(out_file, pf_data)

    # Write done C header file
    write_file(out_file, modified_point_cloud2_iterator_header_file)

    # Copy done C source file
    shutil.copy('asset/point_cloud2_iterator_model.c', 'point_cloud2_iterator.c')

    # Comfirm operation success to user
    print(f"Files generated with success :\n\t* {out_file}\n\t* point_cloud2_iterator.c")

if __name__ == "__main__":
    main()
