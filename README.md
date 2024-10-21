<p align="center">
<img src=".images/pc2Gen_logo.jpg" width=300 height=300>
</p>

# Description

From expensive [LiDAR](https://en.wikipedia.org/wiki/Lidar) to entry-level [Time-Of-Flight (TOF)](https://en.wikipedia.org/wiki/Time-of-flight_camera) cameras, point clouds data and its ROS2 structured [sensor_msgs/PointCloud2](https://docs.ros.org/en/ros2_packages/rolling/api/sensor_msgs/interfaces/msg/PointCloud2.html) message are nowadays a need for environnement sensing. Due to sensor diversity, understanding and configuring highly customable [sensor_msgs/PointCloud2](https://docs.ros.org/en/ros2_packages/rolling/api/sensor_msgs/interfaces/msg/PointCloud2.html) could be headache and time consuming.

**Pc2Gen aims to lower time and code complexity of [sensor_msgs/PointCloud2](https://docs.ros.org/en/ros2_packages/rolling/api/sensor_msgs/interfaces/msg/PointCloud2.html) integration** within [ROS2](https://docs.ros.org/en/rolling/index.html) or [micro-ros](https://micro.ros.org) project by generating, from input descriptions, the relative C code allowing direct message creation and manipulation.


> [!NOTE]  
> Pc2Gen has been thought for both heavy and resourced constrained targets.

# Get Started

Four steps are needed to generate your C custom PointCloud2 message :
1. If not already informed, add your [pointFields](https://docs.ros.org/en/ros2_packages/rolling/api/sensor_msgs/interfaces/msg/PointField.html) elements to the [pointField_def.yaml](asset/pointField_def.yaml) file.
2.  Describe your message in the [device_def.yaml](asset/device_def.yaml) file
3.  Run pc2Gen python script
4.  Add generated files to your project and call instanciation function

# Dependencies

pc2Gen relies on following packages :

* [Python 3.7](https://www.python.org/downloads) or above version
* The python yaml extention
```bash
pip install pyyaml
```
To compile and run generated example, you also need installed on your machine :
* [ROS2](https://docs.ros.org/en/rolling/Releases.html) and relatives compiling tools
* Optionnaly, [micro-ros](https://micro.ros.org/docs/tutorials/core/first_application_linux) if you which to target embedded platforms

# Usage

Let's create a basic X-Y-Z [sensor_msgs/PointCloud2](https://docs.ros.org/en/ros2_packages/rolling/api/sensor_msgs/interfaces/msg/PointCloud2.html) message :

Clone this repository to your workspace :
```bash
git clone https://github.com/fofolevrai/pc2Gen.git && cd pc2Gen
```
**1.  Describe the PointCloud2 message**

In this root directory, create a file `xyz_device.yaml` which discribes the ordered informed data that will populate by your sensor within your PointCloud2 message.

```yaml
devices:
 - name: "xyz_device"
   pointFields:
    - name: "x"
    - name: "y"
    - name: "z"
```
This configures PointCloud2 message. Each 3D pixel will represent (in order) X, Y, and Z spacial information.

**2. Generate relative C code**

Open a terminal pointing to the root directory and run Pc2Gen :

```bash
python3 pc2Gen.py
```
C files `point_cloud2_iterator.h` and `point_cloud2_iterator.c` should be generated within the root directory :
```bash
Files generated with success :
        * point_cloud2_iterator.h
        * point_cloud2_iterator.c
```
This files contain the PointCloud2 message and helper functions.

> [!TIP]  
> For more complex usage, please refer to the [dedicated page](doc/DEFINITION.md).

## Integrate pc2Gen generated code to your project

Generated files contain helper functions to instanciate and manipulate your PointCloud2 message.

**1. Instanciation**

To instanciate your PointCloud2 message, call the function `CreatePointCloud2FromDevice` as follow :

```C
  /* PointCloud2 declaration */
  sensor_msgs__msg__PointCloud2 cloud;

  /* Mock (sensor characteristics) */
  uint32_t height_size = 8;     // Nbr column pixels
  uint32_t width_size = 8;      // Nbr line pixels
  bool is_bigendian = false;    // Data bytes ordering
  
  /* PointCloud2 instanciation */
  bool success = CreatePointCloud2FromDevice(&cloud, "xyz_device", height_size, width_size, is_bigendian);
```
On success return, function `CreatePointCloud2FromDevice` allocates needed memory space and populates fields regarding the given information from both `pointField_def.yaml` and `device_def.yaml` files.

**2. Data feeding**

Feed the message with your sensor data :

```C
//  cloud.data.data is type pointer as described
//  in the 'pointField_def.yaml' file
(float) * my_xyz_sensor_data;

/*
*   Fetch X-Y-Z data from your sensor ...
*/

//  Link data cloud pointer to your fetched data
cloud.data.data = my_xyz_sensor_data;
```
> [!CAUTION]  
> * Do respect the data ordering as described in the [device_def.yaml](asset/device_def.yaml) file.
> * Do respect the data type as informed in the [pointField_def.yaml](asset/pointField_def.yaml) file

## Compile your code

This step consists of ROS/ROS2 [package](https://docs.ros.org/en/rolling/Tutorials/Beginner-Client-Libraries/Creating-Your-First-ROS2-Package.html) creation.

## Examples

**Intel realsense D4xx**

realsense_d4xx_face_render

<p align="center">
<img src=".images/realsense_d4xx_face_render.jpg" width=300 height=300>
</p>

**ST VL53Lx**

*TBD*

# Test

*TBD*

# Issue

If you find issue(s), please report to the [dedicated tumb](https://github.com/fofolevrai/pc2Gen/issues)
# Contribute

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Authors

* [@fofolevrai](https://github.com/fofolevrai)



## License

This project is provided under the [BSD-3](https://opensource.org/license/bsd-3-clause) License - see the [LICENCE.md](LICENCE.md) file for details


