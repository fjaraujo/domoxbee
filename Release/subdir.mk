################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ServerSocket.cpp \
../Socket.cpp \
../atcon.cpp \
../datacon.cpp \
../domoxbee.cpp \
../iocon.cpp \
../main.cpp \
../rematcon.cpp \
../remotenode.cpp \
../xbeep.cpp 

OBJS += \
./ServerSocket.o \
./Socket.o \
./atcon.o \
./datacon.o \
./domoxbee.o \
./iocon.o \
./main.o \
./rematcon.o \
./remotenode.o \
./xbeep.o 

CPP_DEPS += \
./ServerSocket.d \
./Socket.d \
./atcon.d \
./datacon.d \
./domoxbee.d \
./iocon.d \
./main.d \
./rematcon.d \
./remotenode.d \
./xbeep.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -pthread -I"/home/fjaraujo/git/domoxbee/06.domoxbee" -I/home/fjaraujo/install/libxbee3-read-only -O0 -Wall -c -fmessage-length=0  -Wno-reorder -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


