################################################################################
##
## Micro-specific Makefile definitions for the STM32F0x micro.
##


################################################################################
##
## Compiler flags.
## Additional compiler command line options are defined by $(CFLAGS) and $(OPTIMIZATION)
##
MICRO_CFLAGS:=--cpu Cortex-M0+


################################################################################
##
## Assembler flags.
## Additional assembler command line options are defined by $(AFLAGS)
##
MICRO_AFLAGS:=--cpu Cortex-M0+ -S


################################################################################
##
## Linker flags.
##
MICRO_LINK:=--semihosting $(if $(IS_IAP),--redirect Micro__BootTest=main --redirect PendSV_Handler=Micro__Trap --redirect EmbOSBasic__SystickHandler=SRMain__TimerISR)
MICRO_FLASH:=MICRO_FLASH
MICRO_ENTRY:=$(if $(IS_IAP),main,Micro__BootTest)


################################################################################
##
## Define the byte value used to fill the unused memory.
##
FILL_BYTE:=$(if $(IS_IAP),0xAC,0xFF)

