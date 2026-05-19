###############################################################################
##
## Project Specific Makefile definitions that affect the Class B signature.
##
## This file is maintained by the owner of the project.
##


################################################################################
##
## Select the project type(s) for the project.
## Valid values are:
##      !!BASE_PROJ_TYPES!!
##
PROJ_TYPE:=!!PROJ_TYPE!!


################################################################################
##
## Select the microcontroller used on the project.
## Valid values are:
##      !!SUPPORTED_MICROS!!
##
MICRO:=!!MICRO!!


################################################################################
##
## Set the compiler optimization level for your project.
##
## Standard options:
##  -On     No optimization (best debug support)
##  -Ol     Low optimization (default)
##  -Om     Medium optimization
##  -Oh     High & balanced optimization
##  -Ohs    High optimization favoring speed (sacrificing size)
##  -Ohz    High optimization favoring size (sacrificing speed)
##
## Additional options:
##  With any optimization level:
##      --enable_restrict       Enables the Standard C keyword restrict. This option can be useful
##                              for improving analysis precision during optimization.
##      --no_inlining           Disable function inlining.
##
##  With Medium optimization (-Om) or better:
##      --no_clustering         Disable static clustering optimizations.
##      --no_code_motion        Disable code motion optimizations.
##      --no_cse                Disable common subexpression elimination.
##      --no_shattering         RX Only -- Disable variable shattering. The compiler uses this
##                              feature to break up auto variables, which increases the
##                              performance of the generated code.
##
##  With High optimizations (-Oh, -Ohs, or -Ohz):
##      --no_scheduling         Disable the instruction scheduler.
##      --no_tbaa               Disable type-based alias analysis.
##      --no_unroll             Disable loop unrolling.
##
##  With High optimization favoring speed (-Ohs):
##      --no_cross_call         RX Only -- Disable the cross-call optimizations.
##      --no_size_constraints   Relax the normal restrictions for code size expansion when
##                              optimizing for high speed.
##
##  Settings from default files for the legacy build environment:
##      For RX Micros:
##          OPTIMIZATION:=-Om --no_clustering --no_code_motion --no_cross_call --no_cse --no_fpu --no_inline --no_unroll
##      For ARM Micros:
##          OPTIMIZATION:=-On
##
OPTIMIZATION:=-Om

################################################################################
##
## Set an alternate compiler optimization level for a subset of C files in your project.
##
## This optional feature allows to use a different optimization level than OPTIMIZATION for some C source files that require it.
## To enable it, define:
## - ALT_OPTIMIZATION with the optimization level required - see OPTIMIZATION
## - ALT_OPTIMIZATION_FILES with the list of source file names (with .c suffix) to be compiled with ALT_OPTIMIZATION
##
## ALT_OPTIMIZATION:=-Ohs
## ALT_OPTIMIZATION_FILES:=ModuleFile1.c ModuleFile2.c

################################################################################
##
## Set the build tool versions for the project.
##
## You can use "make list-tool-versions" to see available versions.
##
IAR_ARM_VERSION:=v7.70
IAR_RX_VERSION:=v2.90
EMBOS_ARM_VERSION:=v4.26
EMBOS_RX_VERSION:=v4.24
