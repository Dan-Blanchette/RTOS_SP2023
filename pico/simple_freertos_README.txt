Running Simple FreeRTOS program:

Take the folder FreeRTOS-Kernel and put it under your pico folder.

Take the folder simple_usb and put it under your pico folder. 

In CMakeLists.txt, change the line:

set(PICO_SDK_FREERTOS_SOURCE /home/ros2box/pico/FreeRTOS-Kernel)

to match the file path to your folder, probably /home/your-user-name/pico/FreeRTOS-Kernel

Now you should be able to configure and build your simple_usb project. 

For future projects with FreeRTOS, you should be able to use the CMakeLists.txt file as a guide. Make sure your PICO_SDK_FREERTOS_SOURCE file has the correct filepath. 
