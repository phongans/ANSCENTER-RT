# OVIDA Edge AI Video Analytics for any hardware.

![ovida](doc/ovida.png)

# Layer Architecture
![architecture](doc/RT-LAYER.png)

# Show case
## Connect 
![ovida](doc/conect-camara.png)

## Camera view
![ovida](doc/cameraview.png)

## Detector
![ovida](doc/detector.png)

## Tracker
![ovida](doc/tracker.png)

## Analysis setup

# Introducing  OVIDA-RT.
OVIDA-RT is our AI software stack that comes pre-installed with dozens of video analytics and computer vision solutions. It's easy to configure and customize to your use-case, even if you're not a data scientist or developer.

# All of our solutions for one price
For a single low price you have access to all of our AI solutions now, and in the future.

# Deployment options for OVIDA's algorithms.
OVIDA provides many deployment options for our computer vision algorithms, including hardware, cloud, and docker. Work with us to define your ideal outcomes, and our solutions team will provide a full package, ready for release or integration.

## Windows
### Visual Studio 2013
#### Environment
The build procedure has been tested and verified using:  
- Windows 7 Ultimate Service Pack 1 64-bit
- Visual Studio 2013 Ultimate Update 5  
- CMake 3.2.3: http://www.cmake.org/download/  
- OpenCV 3.0.0: http://opencv.org/downloads.html  
- Qt 5.5.0 (msvc2013_64): http://www.qt.io/download/

#### Preparation
Firstly, ensure the following are added to the Windows ```PATH``` variable (modified as required):  
- CMake executable: ```C:/Program Files (x86)/CMake/bin```  
- Qt libraries (DLLs): ```C:/Qt/Qt5.5.0/5.5/msvc2013_64/bin```  
- OpenCV libraries (DLLs): ```C:/Users/Nick/Desktop/opencv/build/x64/vc12```


# Reference architecture
Projects/products/research using this application as a base:

- Control Testbed, Monash University, Australia: [Video](https://www.youtube.com/watch?v=f_Ax93E_f1M)
- MultiCV: A computer vision software infrastructure [Github](https://github.com/citiZenStef/MultiCV)
- Realtime Video Magnification [Github](https://github.com/tschnz/Live-Video-Magnification)

# Features

- Interactive Qt-based GUI (Qt 6).
- Utilizes the new OpenCV 4.x C++ API.
- (NEW) Multi-camera support: Process frames from multiple cameras simultaneously.
- (NEW) Stream synchronization: Multiple streams can be processed in sync on a frame-by-frame basis.
- (NEW) Shared image buffer: Allows threads access to image buffers from other streams.
- (NEW) Scale to fit frame: Frame can optionally be scaled to fit the entire window (i.e. when a ROI is set or a low resolution is being used).
- (NEW) Resolution can optionally be set (via OpenCV call) when connecting to the camera.
- (NEW) Application can now handle any camera resolution (window size will auto adjust on connect).
- (NEW) Application window can now be resized freely (image will scale accordingly).
- Small memory footprint (typically <30MB when one camera is connected).
- Dedicated thread to capture frames from camera.
- Dedicated thread to perform image processing on captured frames.
- Thread priorities can be chosen at run-time.
- Includes function to perform the required conversion from the OpenCV Mat format to Qt's QImage format.
- Ability to choose image buffer size and camera device number at run-time.
- Option to drop frame if image/frame buffer is full.
- Ability to clear image buffer while the capture and processing threads are running.
- Ability to set the "region of interest" (ROI) interactively with the aid of a mouse.
- Graphical representation of image buffer status (% full).
- Capture rate and processing rate (in FPS) display.
- (NEW) Number of frames captured and processed display.
- Camera information (device number and resolution) display.
- Region of interest (ROI) information display.
- Mouse cursor position display (including pixel position display).
- Several in-built (and user-settable) OpenCV functions to assess multithreading performance:
- Detector with YOLO
- Tracker with OCSORT

