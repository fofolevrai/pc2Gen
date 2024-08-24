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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <std_msgs/msg/string.h>

#include "rosidl_runtime_c/primitives_sequence_functions.h"
#include <std_msgs/msg/u_int8.h>
#include <std_msgs/msg/header.h>
#include <sensor_msgs/msg/point_field.h>
#include <sensor_msgs/msg/point_cloud2.h>

#include "rcutils/allocator.h"
#include "point_cloud2_iterator.h"

/// @brief Commun device
const pointCloud2Generator_t g_customDevice[] = _DEVICE_INITIALIZER_;

/// @brief Commun fields 
const sensor_msgs__msg__PointField communPointFields[] = _POINTFIELD_INITIALIZER_;

/// @brief Return the size of a datatype (which is an enum of sensor_msgs__msg__PointField__) in bytes
/// @param datatype one of the enums of sensor_msgs::msg::PointField::
/// @return Datatype size
size_t sizeOfPointField(uint8_t datatype_u8)
{
    switch (datatype_u8)
    {
    case sensor_msgs__msg__PointField__INT8:
    case sensor_msgs__msg__PointField__UINT8:
    {
        return (int8_t) sizeof(uint8_t);
    }
    break;
    case sensor_msgs__msg__PointField__INT16:
    case sensor_msgs__msg__PointField__UINT16:
    {
        return (int8_t) sizeof(uint16_t);
    }
    break;
    case sensor_msgs__msg__PointField__INT32:
    case sensor_msgs__msg__PointField__UINT32:
    case sensor_msgs__msg__PointField__FLOAT32:
    {
        return (int8_t) sizeof(uint32_t);
    }
    break;
    case sensor_msgs__msg__PointField__FLOAT64:
    {
        return (int8_t) sizeof(uint64_t);
    }
    break;
    default:
    {
        //  Wrong field type argv passed
        fprintf(stderr, "PointField of type %d does not exist", datatype_u8);
        return 0;
    }
    break;
    }

    //  Should never be reached
    return 0;
}

bool
sensor_msgs__msg__PointField__Sequence__add(
  const sensor_msgs__msg__PointField__Sequence * input,
  sensor_msgs__msg__PointField__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__msg__PointField);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__msg__PointField * data =
      (sensor_msgs__msg__PointField *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__msg__PointField__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__msg__PointField__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  
  for (size_t i = output->size, j = 0; i < input->size; ++i, ++j) {
    if (!sensor_msgs__msg__PointField__copy(
        &(input->data[j]), &(output->data[i])))
    {
      return false;
    }
  }
  output->size = input->size;

  return true;
}

/// @brief A a Point field to point cloud
/// @param field_to_add The point field to add to the PointCloud2 message
/// @param pc_msg The PointCloud2 message
/// @param offset offset pointer of that element
/// @return Operation status
bool addPointFieldtoPointCloud2(const sensor_msgs__msg__PointField field_to_add, sensor_msgs__msg__PointCloud2 * pc_msg, uint32_t * offset)
{
    //  Check args
    if((NULL == pc_msg)
        || (NULL == offset)
        || (NULL == field_to_add.name.data)
        || (0 == field_to_add.name.capacity)
        || (field_to_add.name.size >= field_to_add.name.capacity))
    {
        return false;
    }

    sensor_msgs__msg__PointField__Sequence pf_seq = {
        .data = &field_to_add,
        .size = pc_msg->fields.size + 1,
        .capacity = pc_msg->fields.capacity + 1
    };

    if(!sensor_msgs__msg__PointField__Sequence__add(&pf_seq, &(pc_msg->fields)))
    {
        return false;
    }

    //  Update cloud point step
    pc_msg->point_step += (pc_msg->fields.data[pc_msg->fields.size - 1].count * sizeOfPointField(pc_msg->fields.data[pc_msg->fields.size - 1].datatype));

    return true;
}

/**
 * @brief Private function that adds a PointField to the "fields" member of a PointCloud2
 * @param cloud_msg the PointCloud2 to add a field to
 * @param name the name of the field
 * @param count the number of elements in the PointField
 * @param datatype the datatype of the elements
 * @param offset offset pointer of that element
 * @return Operation status
 */
bool addPointFieldFromArgs(sensor_msgs__msg__PointCloud2 *cloud_msg,
                                     const char * name, uint32_t count,
                                     uint8_t datatype)
{
    if((NULL == cloud_msg)
        || (NULL == name))
    {
        return false;
    }

    sensor_msgs__msg__PointField field_to_add = {
        .name = {.data = name, .size= 55, .capacity = sizeof(name)},
        .count = count,
        .datatype = datatype,
        .offset = cloud_msg->point_step};
    
    if(!addPointFieldtoPointCloud2(field_to_add, cloud_msg, &(cloud_msg->point_step)))
    {
        return false;
    }

    return true;
}


/// @brief Return the number of elements (points) present in the cloud message
/// @param cloud_msg The PointCloud2 message pointer to fetch size from
/// @return The number of point cloud elements present into the 'cloud_msg' frame
size_t GetSize(sensor_msgs__msg__PointCloud2 *cloud_msg)
{
    if (NULL == cloud_msg)
    {
        //  Issue
        fprintf(stderr, "Null argument given");
        return 0;
    }

    if(!cloud_msg->point_step)
    {
        //  Issue
        fprintf(stderr, "Pointcloud2 'point_step' field is nil");
        return 0;
    }

    if((NULL == cloud_msg->data.data)
        || (!cloud_msg->data.size))
    {
        //  Issue
        fprintf(stderr, "No data in point cloud msg");
        return 0;
    }

    if(cloud_msg->data.size % cloud_msg->point_step)
    {
        //  Issue
        fprintf(stderr, "PointCloud2 data size could not be a 'point_step' fraction");
        return 0;
    }

    return cloud_msg->data.size / cloud_msg->point_step;
}

/// @brief Resize point cloud size
/// @param cloud_msg The point cloud message to update
/// @param newSize The new data size
/// @return Operation status
bool dataResize(sensor_msgs__msg__PointCloud2 *cloud_msg, size_t newSize)
{
    void * data = NULL;
    size_t total_size, original_size;
    rcutils_allocator_t allocator = rcutils_get_default_allocator();

    //  Check args
    if((NULL == cloud_msg)
        || (0 == cloud_msg->point_step))
    {
        return false;
    }

    original_size = cloud_msg->height * cloud_msg->width;
    total_size = newSize * cloud_msg->point_step;

    //  Shall we need to expend the Point cloud data size
    if(newSize > cloud_msg->data.capacity)
    {
        //  Reallocate cloud data size
        if (NULL == (data = allocator.reallocate(cloud_msg->data.data, total_size, allocator.state)))
        {
            return false;
        }

        cloud_msg->data.data = data;
        cloud_msg->data.capacity = newSize;
    }

    //  Update data size
    cloud_msg->data.size = newSize;

    return true;
}

/// @brief Resize point cloud size
/// @param cloud_msg The point cloud message to update
/// @param width Number of width point into the message
/// @param height Number of height point into the message
/// @return Operation status
bool ResizeFromArgs(sensor_msgs__msg__PointCloud2 *cloud_msg, uint32_t width, uint32_t height)
{
    bool success = true;
    
    //  Check args
    if((NULL == cloud_msg)
        || (0 == cloud_msg->point_step))
    {
        return false;
    }

    success &= dataResize(cloud_msg, (size_t) (width * height));

    cloud_msg->width = width;
    cloud_msg->height = height;
    cloud_msg->row_step = width * cloud_msg->point_step;

    return success;
}

/// @brief Add point fields to point cloud from the given device
/// @param cloud_msg The point cloud that should receive point field
/// @param deviceName Name of the device to add 
/// @return Operation status
bool SetPointFieldsFromDevice(sensor_msgs__msg__PointCloud2 * cloud_msg,
                            const char * deviceName)
{
    //  Point field offset
    uint32_t offset = 0;
    uint16_t counter = 0, k;
    sensor_msgs__msg__PointField * pf_msg = sensor_msgs__msg__PointField__create();

    if((NULL == cloud_msg) || (NULL == deviceName))
    {
        return false;
    }

    //  Check
    if((NULL != cloud_msg->fields.data)
        && (0 < cloud_msg->fields.size)
        && (0 < cloud_msg->fields.capacity)
        && (cloud_msg->fields.capacity < cloud_msg->fields.size))
    {
        offset = cloud_msg->fields.data[cloud_msg->fields.size].offset;
    }

    while(NULL != g_customDevice[counter].deviceName)
    {
        if(0 == strcmp(g_customDevice[counter].deviceName, deviceName))
        {
            for(uint32_t j = 0; j < g_customDevice[counter].nbr_pointFields; j++)
            {
                //  Reset communPointFields's counter
                k = 0;
                while(NULL != &communPointFields[k])
                {
                    if(0 == strcmp(communPointFields[k].name.data, g_customDevice[counter].pointFieldCapacity[j]))
                    {
                        if(!sensor_msgs__msg__PointField__copy(&communPointFields[k], pf_msg))
                        {
                            return false;
                        }
                        pf_msg->offset = offset;

                        if(false == addPointFieldtoPointCloud2((const sensor_msgs__msg__PointField) *pf_msg, cloud_msg, &offset))
                        {
                            //  Issue while adding point field
                            return false;
                        }
                        break;
                    }
                    //  Increase k counter
                    k++;
                }
            }
            break;
        }
        //  Increase counter
        counter++;
    }

    //  Did we find the device name
    if(NULL == g_customDevice[counter].deviceName)
    {
        //  We do interate the entire array
        //  without fetching the corresponding device name
        return false;
    }

    return true;
}

/// @brief Create PointCloud2 message from selected device
/// @param cloud_msg PointCloud2 message instance pointer that will be created from the known device name
/// @param deviceName Device name that will populate PointCloud2 message
/// @param height Height PointCLoud2 length
/// @param width Width PointCloud2 length
/// @param is_bigendian Data ordering (big or little endian)
/// @return Operation status (true success, false failure)
bool CreatePointCloud2FromDevice(sensor_msgs__msg__PointCloud2 * cloud_msg,
                            const char * deviceName, uint32_t height, uint32_t width, bool is_bigendian)
{
    if((NULL == deviceName) || (NULL == cloud_msg))
    {
        return false;
    }

    if(false == SetPointFieldsFromDevice(cloud_msg, deviceName))
    {
        return false;
    }

    //  Populate pointCloud2 fields
    (cloud_msg)->row_step = (cloud_msg)->point_step * width;
    (cloud_msg)->height = height;
    (cloud_msg)->width = width;
    (cloud_msg)->is_bigendian = is_bigendian;

    if(!rosidl_runtime_c__uint8__Sequence__init(&(cloud_msg)->data, (cloud_msg)->row_step * height))
    {
        return false;
    }

    return true;
}
