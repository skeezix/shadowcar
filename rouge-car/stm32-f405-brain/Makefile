
# refering to libopencm3-examples Makefiles/ld-script
LCM3EX_BASE=/home/skeezix/archive/devo/toolkits/stm32/libopencm3-examples/examples/stm32/f4/

BINARY = rouge
APPOBJS = lib_serial.o MPU6050.o lib_mpu6050_i2c.o skeelib.o rouge_cm3.o lib_pwm_motor.o balancer.o

LDSCRIPT = ${LCM3EX_BASE}/stm32f4-discovery/stm32f4-discovery.ld

# copied libopencm3 Makefile.include into a local copy, hacked it up a touch
#include ${LCM3EX_BASE}/Makefile.include
include Makefile.libopencm3
