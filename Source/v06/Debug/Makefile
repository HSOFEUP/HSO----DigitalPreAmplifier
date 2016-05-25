################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../adc_analog.c \
../eeprom.c \
../interruptvectors.c \
../lcd.c \
../my_utils.c \
../SHT11.c \
../SHT25.c \
../SPI.c \
../TSL.c \
../tsl2561.c \
../twi.c \
../usart.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
adc_analog.o \
eeprom.o \
interruptvectors.o \
lcd.o \
my_utils.o \
SHT11.o \
SHT25.o \
SPI.o \
TSL.o \
tsl2561.o \
twi.o \
usart.o

OBJS_AS_ARGS +=  \
adc_analog.o \
eeprom.o \
interruptvectors.o \
lcd.o \
my_utils.o \
SHT11.o \
SHT25.o \
SPI.o \
TSL.o \
tsl2561.o \
twi.o \
usart.o

C_DEPS +=  \
adc_analog.d \
eeprom.d \
interruptvectors.d \
lcd.d \
my_utils.d \
SHT11.d \
SHT25.d \
SPI.d \
TSL.d \
tsl2561.d \
twi.d \
usart.d

C_DEPS_AS_ARGS +=  \
adc_analog.d \
eeprom.d \
interruptvectors.d \
lcd.d \
my_utils.d \
SHT11.d \
SHT25.d \
SPI.d \
TSL.d \
tsl2561.d \
twi.d \
usart.d

OUTPUT_FILE_PATH +=TSL.elf

OUTPUT_FILE_PATH_AS_ARGS +=TSL.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler

























./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)F:\Installed_Programs\Electronics\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega128a -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.8.1
	$(QUOTE)F:\Installed_Programs\Electronics\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="TSL.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega128a  
	@echo Finished building target: $@
	"F:\Installed_Programs\Electronics\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "TSL.elf" "TSL.hex"
	"F:\Installed_Programs\Electronics\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "TSL.elf" "TSL.eep" || exit 0
	"F:\Installed_Programs\Electronics\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "TSL.elf" > "TSL.lss"
	"F:\Installed_Programs\Electronics\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "TSL.elf" "TSL.srec"
	"F:\Installed_Programs\Electronics\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-size.exe" "TSL.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "TSL.elf" "TSL.a" "TSL.hex" "TSL.lss" "TSL.eep" "TSL.map" "TSL.srec" "TSL.usersignatures"
	