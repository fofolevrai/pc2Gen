// Copyright (c) 2022, fofolevrai
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the copyright holder nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// This file is originally from:
// https://github.com/ros/common_msgs/blob/50ee957/sensor_msgs/include/sensor_msgs/impl/point_cloud2_iterator.h

#ifndef __APP_PC2_ITERATOR_H
#define __APP_PC2_ITERATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sensor_msgs/msg/point_cloud2.h>

/// @brief 
typedef struct _pointCloud2Generator_t_{
    char * deviceName;
    char * pointFieldCapacity[20];
    uint16_t nbr_pointFields; 
} pointCloud2Generator_t;

/// @brief Custom device initialization structure
/// This line will be replaced by device initializer 'pc2Gen.py' script
/// following the informed yaml definitions of the project root folder
///
/// Do not change the line as pc2Gen.py
/// will look and replace '#define _DEVICE_INITIALIZER_'
#define _DEVICE_INITIALIZER_ NULL

/// @brief Custom point field initialization structure
/// This line will be replaced by device initializer 'pc2Gen.py' script
/// following the informed yaml definitions of the project root folder
///
/// Do not change the line as pc2Gen.py
/// will look and replace '#define _POINTFIELD_INITIALIZER_'
#define _POINTFIELD_INITIALIZER_ NULL

extern const sensor_msgs__msg__PointField communPointFields[];
extern const pointCloud2Generator_t g_customDevice[];

/// @brief A a Point field to point cloud
/// @param field_to_add The point field to add to the PointCloud2 message
/// @param pc_msg The PointCloud2 message
/// @param offset offset pointer of that element
/// @return Operation status
bool addPointFieldtoPointCloud2(const sensor_msgs__msg__PointField field_to_add, sensor_msgs__msg__PointCloud2 * pc_msg, uint32_t * offset);

/**
 * @brief Private function that adds a PointField to the "fields" member of a PointCloud2
 * @param cloud_msg the PointCloud2 to add a field to
 * @param name the name of the field
 * @param count the number of elements in the PointField
 * @param datatype the datatype of the elements
 * @return Operation status
 */
bool addPointFieldFromArgs(sensor_msgs__msg__PointCloud2 *cloud_msg,
                                     const char * name, uint32_t count,
                                     uint8_t datatype);

/// @brief Return the number of elements (points) present in the cloud message
/// @param cloud_msg The PointCloud2 message pointer to fetch size from
/// @return The number of point cloud elements present into the 'cloud_msg' frame
size_t GetSize(sensor_msgs__msg__PointCloud2 *cloud_msg);

/// @brief Resize point cloud size
/// @param cloud_msg The point cloud message to update
/// @param newSize The new data size
/// @return Operation status
bool dataResize(sensor_msgs__msg__PointCloud2 *cloud_msg, size_t newSize);

/// @brief Resize point cloud size
/// @param cloud_msg The point cloud message to update
/// @param width Number of width point into the message
/// @param height Number of height point into the message
/// @return Operation status
bool ResizeFromArgs(sensor_msgs__msg__PointCloud2 *cloud_msg, uint32_t width, uint32_t height);


/// @brief Clear point cloud & reset field
/// @param cloud_msg The point cloud message to clear
/// @return Operation status
void Clear(sensor_msgs__msg__PointCloud2 *cloud_msg);

/// @brief Set field array to point cloud fields
/// @param cloud_msg The point cloud message to update
/// @param fieldPoints_array Fields to add to the point cloud
/// @param fieldPoint_size Number of fields to add
/// @return Operation status
bool SetPointCloud2Fields(sensor_msgs__msg__PointCloud2 *cloud_msg,
                            const sensor_msgs__msg__PointField * fieldPoints_array,
                            uint32_t fieldPoint_size);


/// @brief Add point fields to point cloud from the given device
/// @param cloud_msg The point cloud that should receive point field
/// @param deviceName Name of the device to add 
/// @return Operation status
bool SetPointFieldsFromDevice(sensor_msgs__msg__PointCloud2 * cloud_msg,
                            const char * deviceName);

/// @brief Create PointCloud2 message from selected device
/// @param cloud_msg PointCloud2 message instance pointer that will be created from the known device name
/// @param deviceName Device name that will populate PointCloud2 message
/// @param height Height PointCLoud2 length
/// @param width Width PointCloud2 length
/// @param is_bigendian Data ordering (big or little endian)
/// @return Operation status (true success, false failure)
bool CreatePointCloud2FromDevice(sensor_msgs__msg__PointCloud2 * cloud_msg,
                            const char * deviceName, uint32_t height, uint32_t width, bool is_bigendian);


/// @brief Return the size of a datatype (which is an enum of sensor_msgs__msg__PointField__) in bytes
/// @param datatype one of the enums of sensor_msgs::msg::PointField::
/// @return Datatype size
size_t sizeOfPointField(uint8_t datatype_u8);

#ifdef __cplusplus
}
#endif

#endif /* __APP_PC2_ITERATOR_H */
