export BUILD_SCRIPT = $(PWD)/../../submodules/ruckus/system_vivado.mk

# Define target output
target: bit

# Define Firmware Version Number
export PRJ_VERSION = 0x00000001

export PROJECT = xF13P_top
export BOARD = xF13P

#Define target part
export PRJ_PART = xcvu13p-flga2577-2-e

export PARALLEL_SYNTH = 24
export GZIP_BUILD_IMAGE = 1

export TOP_DIR = $(abspath $(PWD)/../..)
  
# !Caution! Timing failuire IGnore flag. Allows bitfile generation even if timing not met. 
export TIG = true

# Use top level makefile
include $(BUILD_SCRIPT)
