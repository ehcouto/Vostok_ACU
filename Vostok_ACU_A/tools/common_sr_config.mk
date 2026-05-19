###############################################################################
## Cross-Project Makefile definitions that affect Class B signature.
##
## This file is maintained by the owner of the build environment.
## If you want to propose a change, please discuss it with the owner.
##


###############################################################################
##
## Define tool and tool options used during the build.
##

# Command for the assembler
AS=iasm$(MICRO_CORE_PATH_NAME)
# Command and options for the assembler
#	Additional assembler options are defined by $(MICRO_AFLAGS)
AFLAGS:=-r

# Command for the assembler
CC=icc$(MICRO_CORE_PATH_NAME)
# Command and options for the C compiler
#	Additional compiler options are defined by $(MICRO_CFLAGS) and $(OPTIMIZATION)
CFLAGS:=--debug -e --only_stdout --silent

# Linker
LINK=ilink$(MICRO_CORE_PATH_NAME)

# ELF output file generator
ELFTOOL=ielftool


###############################################################################
##
## Other definitions
##

