################################################################################
# Whirlpool project Makefile
#
# This file should be the same for all Whirlpool Nucleus SDK projects.
# Changes that are recommended for all projects should be submitted for
# 	approval to be made standard for all projects.
# Changes that are specific to a single project should be implemented through
# 	the makefiles in the config directory.
#
# Copyright (c) 2020 Whirlpool Corporation. All rights reserved -- CONFIDENTIAL.
#


################################################################################
##
## Define folder names
##      Paths must be relative to the project's root directory
##      Directory separators must be a backslash for DOS commands
##      Directory separators must be a forward slash for Makefile targets
##

# Define the folder where cross-category configuration files are located.
DIRBLD:=tools

# Define the folder where project-specific configuration files are located.
DIRCFG:=config

# Define the folder where loading rules are stored.
DIRLDG:=loading

# Define the folder used to store PC-lint output files.
DIRLNT:=linted

# Define the folder used to store generated list files.
DIRLST:=listings

# Define the folder used to store generated object files.
# This value must match the 'object_directory' value specified in the
#   regenerate.json file.
DIROBJ:=obj

# Define the folder used to store results of project builds.
DIROUT:=exe

# Define the project's root folder.
# The make utility must be run from the project's root folder.
DIRPRJ:=$(subst /,\,$(CURDIR))

# Define the folder where the build parameters are placed.
DIRPRM:=$(DIRCFG)\build
DIRPRM_:=$(subst \,/,$(DIRPRM))

# Define the folder where the state information and old S19 files are placed.
DIRPRV:=$(DIRCFG)\last
DIRPRV_:=$(subst \,/,$(DIRPRV))

# Define the folder where the source files are located.
# This value must match one of the elements of the 'source_folders' array
#   value specified in the regenerate.json file.
DIRSRC:=source

# Define the folder used to temporarily store other generated files.
# This value must match the 'temporary_directory' value specified in the
#   regenerate.json file.
DIRTMP:=$(DIRCFG)\tmp
DIRTMP_:=$(subst \,/,$(DIRTMP))

# Define the folder that holds the SRSystemConfig_prm.h file.
DIRSRSYSCFG:=$(DIRSRC)\ProjectSpecific\Common\all\SRConfig
# Define the folder that holds the SystemConfig_prm.h file.
DIRSYSCFG:=$(DIRSRC)\ProjectSpecific\Common\all\Config

DIRDRVCFG=$(DIRSRC)\ProjectSpecific\Driver
DIRDRVCFG_:=$(subst \,/,$(DIRDRVCFG))


################################################################################
##
## Detect characteristics of the SDK build environment before proceeding.
##

# Detect the SDK environment.
SDK_PROJECT_DETECTED:=$(if $(wildcard $(DIRSRC)/Micros/*),YES,NO)
ifeq ($(SDK_PROJECT_DETECTED),NO)
  $(error No SDK project was detected. This build environment only works with SDK projects.)
endif

# Detect an SDK workspace (as opposed to a project workspace that is based on the SDK).
PROJ_FOLDER:=$(notdir $(DIRPRJ))
PROJ_FOLDER_PARTS:=$(subst -, ,$(PROJ_FOLDER))
IS_SDK:=$(if $(and $(filter Nucleus,$(word 1,$(PROJ_FOLDER_PARTS))),\
                   $(filter SDK%,$(word 3,$(PROJ_FOLDER_PARTS)))),SDK)

# Detect the IAP environment.
IS_IAP:=$(if $(wildcard $(DIRSRC)/XCategory/Application/IAPMgr/*),_IAP_)

# Determine the Nucleus Tools path and if Nucleus-Tools-SDK project is present there.
ifndef NUCLEUS_TOOLS_PATH
  NUCLEUS_TOOLS_PATH:=..\Nucleus-Tools-SDK
endif

ifeq ($(wildcard $(subst \,/,$(NUCLEUS_TOOLS_PATH))),)
  $(error The Nucleus-Tools-SDK not found in $(NUCLEUS_TOOLS_PATH); check if project is loaded or NUCLEUS_TOOLS_PATH variable is correct)
else
  NUCLEUS_TOOLS_PATH:=$(subst /,\,$(NUCLEUS_TOOLS_PATH))
endif

# Define the option to include/exclude the interrupt vector table in the build.
DBG_OPTION:=$(if $(IS_IAP),,-D ENABLE_JUMP_TO_APPLICATION_WITHOUT_IAP_FIRMWARE)

# Define which micros are supported by the SDK.
#	1.Parse the top level folders names
#	2.Parse the sub folders names
#	3.Make the list of parent micro names those are having variants
#	4.Combile the list of all micros with variants
#	5.Filter out the parent micro names from supported micros
DIRMICROS:=$(sort $(patsubst $(DIRSRC)/Micros/%/Build/MicroCore.mk,%,$(wildcard $(DIRSRC)/Micros/*/Build/MicroCore.mk)))
SUPPORTED_VARIANTS:=$(foreach FOLDER,$(DIRMICROS),\
					  $(sort $(patsubst $(DIRSRC)/Micros/$(FOLDER)/Build/%/MicroCore.mk,$(FOLDER)-%,$(wildcard $(DIRSRC)/Micros/$(FOLDER)/Build/*/MicroCore.mk))))
MICRO_GROUPS:=$(foreach FOLDER,$(SUPPORTED_VARIANTS),\
				$(word 1,$(subst -, ,$(FOLDER))))

SUPPORTED_MICROS:=$(filter-out $(MICRO_GROUPS),$(DIRMICROS) $(SUPPORTED_VARIANTS))

# Define which project types are supported by the SDK.
BASE_PROJ_TYPES:=$(if $(IS_IAP),IAP,ACU HMI EXPACU EXPHMI)
SUPPORTED_PROJ_TYPES:=$(if $(IS_IAP),IAP,ACU HMI ACU+HMI EXPACU EXPHMI ACU+EXPHMI HMI+EXPACU EXPACU+EXPHMI)


################################################################################
##
## Define filenames
##      Paths must be relative to the project's root directory
##      Directory separators must be a backslash for DOS commands
##      Directory separators must be a forward slash for Makefile targets
##

# Define the file that contains the search paths for the compiler and PC-lint.
FILE_INCLUDES:=$(DIRTMP)\include_path.lnt
FILE_INCLUDES_:=$(subst \,/,$(FILE_INCLUDES))

# Define the file that contains the list of object files to link during the build.
FILE_LINK_LIST:=$(DIRTMP)\link_list.txt
FILE_LINK_LIST_:=$(subst \,/,$(FILE_LINK_LIST))

# Define the setting file to link during the debug build.
ifdef IS_IAP
FILE_LINK_SF:=
FILE_LINK_SF_:=
else
FILE_LINK_SF:=$(DIRTMP)\settingfile.bin
FILE_LINK_SF_:=$(subst \,/,$(FILE_LINK_SF))
endif

# Define the filename for the configuration file used by the linker.
FILE_LINKER_CONFIG=$(MICRO)_config.icf

# Define the path and filename of the file whose content is placed at the start
# of link_list.txt file.
FILE_LINKER_HEADER=$(MICRO)_link.txt

# Define the file that contains PC-lint configuration information.
FILE_LINT_CONFIG:=$(DIRBLD)\Lint\std.lnt

# Define the file that contains PC-lint configuration information.
# This configuration must be separate from other PC-lint configuration.
# Jenkins modifies the PC-lint output format to generate PC-lint reports.
FILE_LINT_FORMAT_CONFIG:=$(DIRBLD)\Lint\format.lnt

# Define lint output files generated by PC-lint.
FILE_LINT_PROJECT:=$(DIRLNT)\_project.lnt

# Define the file that contains the micro used on the last build.
FILE_MICRO:=$(DIRPRV)\Micro.mk

# Define the file that contains the project type used on the last build.
FILE_PROJ_TYPE:=$(DIRPRV)\ProjectType.mk

# Define the file that contains the embOS version used on the last build.
FILE_EMBOS_PATH:=$(DIRPRV)\EmbOS.mk

# Define the file that contains the IAR version used on the last build.
FILE_IAR_PATH:=$(DIRPRV)\IAR.mk

# Define a filename suffix used when generating debug files.
SUFFIX_DEBUG:=_Debug


################################################################################
##
## Include sub-makefiles.
##

# Include sub-makefiles
-include $(DIRCFG)/project_config.mk
-include $(DIRTMP_)/paths.mk
-include $(DIRTMP_)/sources.mk
include $(DIRBLD)/custom_functions.mk
include $(DIRBLD)/common_sr_config.mk
-include $(DIRCFG)/project_sr_config.mk
-include $(DIRCFG)/sw05.mk

################################################################################
##
## Generate definitions derived from included makefiles.
##

# Verify that FILE_MERGE_FVT_SF does not have a '/' or a '\'.
ifdef FILE_MERGE_FVT_SF
  ifneq ($(or $(findstring /,$(FILE_MERGE_FVT_SF)),$(findstring \,$(FILE_MERGE_FVT_SF))),)
    $(error FILE_MERGE_FVT_SF must not include a path)
  endif
endif
# Create a list of lint files that can be generated.
ifdef CFILES
  LNTFILES:=$(CFILES:c=lnt)
else
  LNTFILES=$(SPACE)
endif

ifndef S37FILES
  S37FILES=$(SPACE)
endif

# Create a list of obj files that should be generated.
ifdef CFILES
  OBJECTS:=$(sort $(CFILES:c=obj) $(S37FILES:s37=obj))
else
  OBJECTS=$(SPACE)
endif

# Define the makefile VPATH.
ifdef PATHS
  VPATH:=$(subst \,/,$(DIROBJ) $(PATHS) $(DIROUT))
endif

# Define missing definitions.
ifndef GNU_FORMAT
  GNU_FORMAT=NO
endif
ifndef ALT_OPTIMIZATION
  ALT_OPTIMIZATION=$(SPACE)
endif
ifndef ALT_OPTIMIZATION_FILES
  ALT_OPTIMIZATION_FILES=$(SPACE)
endif

ifndef SW05_WARNING
  SW05_WARNING:=
endif

################################################################################
##
## Specify where to find standard build environment tools.
##      Paths must be relative to the project's root folder.
##      Directory separators must be a backslash for DOS commands
##      Directory separators must be a forward slash for Makefile targets
##

# Execution call for the PC-lint utility
LINT_EXE:=$(DIRBLD)\Lint\lint-nt.exe

# Execution call for the LogMessagesCreator utility
LOGMESSAGESCREATOR:=$(DIRBLD)\Log\LogMessagesCreator.exe

# Execution call for the LogPreprocessor utility
LOGPREPROCESSOR:=$(DIRBLD)\Log\LogPreprocessor.exe

# Batch file to either use the specified setting file or find the newest setting
# file in a directory.
FIND_FILE_BAT:=$(DIRBLD)\getDebugSf.bat

# Batch file to implement merging actions with Project s19 , FVT setting file and IAP s19 file
MERGE_RELEASE_BAT:=$(DIRBLD)\MergeRelease.bat

# Batch file to create S19 Setting File using the SF binary file
SF_S19_BAT:=$(DIRBLD)\CreateS19SettingFile.bat

# Execution call for the sw05 test executer
SW05_TEST_EXE:=$(NUCLEUS_TOOLS_PATH)\sw05\sw05_test.exe

# Define the Microsoft command shell for executing commands.
SHELL:=$(ComSpec)
ifeq ($(SHELL),)
  SHELL:=cmd.exe
endif


################################################################################
##
## Determine the project name that will be used for the build.
##

# Determine the project name if none has been specified.
ifndef PROJ_NAME
  PROJ_NAME:=$(PROJ_FOLDER)
endif
ifdef IS_SDK
  # Text used in the generation of the project_config.mk file.
  PROJ_NAME_PREFIX:=ProjectName
else
  # Use the first part of the project name as a prefix for generating project_config.mk
  PROJ_NAME_PARTS:=$(subst _, ,$(PROJ_NAME))
  PROJ_NAME_PREFIX:=$(word 1,$(PROJ_NAME_PARTS))
  # Extract a project type out of the project name to use as a default project type.
  PROJ_NAME_TYPE:=$(filter $(word 2,$(PROJ_NAME_PARTS)),$(BASE_PROJ_TYPES))
endif


################################################################################
##
## Define the micro family to use for this project.
##

# Set LAST_MICRO to the micro that was selected on the last build.
LAST_MICRO:=
-include $(FILE_MICRO)
# Verify that the micro is supported.
ifdef MICRO
  	MICRO:=$(filter $(MICRO),$(SUPPORTED_MICROS))
endif

# If no micro is defined, try to set it anyway.
#   Option 1: Use the last micro if available
#   Option 2: Use the micro target from the make command line
#   Option 3: Leave it blank
ifndef MICRO
  ifdef LAST_MICRO
	MICRO:=$(filter $(LAST_MICRO),$(SUPPORTED_MICROS))
  endif
endif
ifndef MICRO
  MICRO:=$(word 1,$(filter $(MAKECMDGOALS),$(SUPPORTED_MICROS)))
endif

MICRO_VARIANT:=$(filter $(MICRO),$(SUPPORTED_VARIANTS))

#Identify the supported micro is varient, if yes then set build folder path accordingly
ifeq ($(MICRO),$(MICRO_VARIANT))
	DIRMICRO:=$(word 1,$(subst -, ,$(MICRO)))
	DIRVARIANT:=$(word 2,$(subst -, ,$(MICRO)))
	DIRMICROBUILD:=$(DIRMICRO)\Build\$(DIRVARIANT)
else
	DIRMICRO:=$(MICRO)
	DIRMICROBUILD:=$(DIRMICRO)\Build
endif

DIRMICROBUILD_:=$(subst \,/,$(DIRMICROBUILD))

# Define FORCED_MICRO for use in situations where MICRO must be defined.
ifdef MICRO
  FORCED_MICRO:=$(MICRO)
else
  FORCED_MICRO:=$(word 1, $(SUPPORTED_MICROS))
endif


################################################################################
##
## Define the compiler and embOS library to use for this project.
##

# Set LAST_DIRCX to the compiler that was selected on the last build.
LAST_DIRCX:=
-include $(FILE_IAR_PATH)
# Set LAST_EMBOS_PATH to the embOS version that was selected on the last build.
LAST_EMBOS_PATH:=
-include $(FILE_EMBOS_PATH)
# Compiler is based on the MICRO definition.
-include $(DIRSRC)/Micros/$(DIRMICROBUILD_)/MicroCore.mk
# Define the folders where the micro tools can be found.
DIRCX:=$(NUCLEUS_TOOLS_PATH)\iar\$(MICRO_CORE_PATH_NAME)\$(IAR_$(MICRO_CORE)_VERSION)\cx
DIRLIB:=$(NUCLEUS_TOOLS_PATH)\iar\$(MICRO_CORE_PATH_NAME)\$(IAR_$(MICRO_CORE)_VERSION)\lib
# Get IAR Group and set embOS folder to use
-include $(DIRCX)/IAR.mk
EMBOS_PATH:=$(NUCLEUS_TOOLS_PATH)\embOS\$(MICRO_CORE_PATH_NAME)\$(EMBOS_$(MICRO_CORE)_VERSION)
-include $(DIRCFG)/$(MICRO).mk


################################################################################
##
## Define the project type to use for this project.
##      Guarantee the order of elements in the PROJ_TYPE definition.
##

# Set LAST_PROJ_TYPE to the project type that was selected on the last build.
LAST_PROJ_TYPE:=
-include $(FILE_PROJ_TYPE)
# Sort the base project types to be in a pre-determined order.
ifdef PROJ_TYPE
  PROJ_TYPE:=$(filter $(PROJ_TYPE),$(BASE_PROJ_TYPES))
endif
# If no project type is defined, set it anyway.
#   Option 1: Use the last project type if available
#   Option 2: Use the project type based on the project name
#   Option 3: Use the first supported project type
ifndef PROJ_TYPE
  ifdef LAST_PROJ_TYPE
    PROJ_TYPE:=$(filter $(LAST_PROJ_TYPE),$(BASE_PROJ_TYPES))
  endif
endif
ifndef PROJ_TYPE
  ifdef PROJ_NAME_TYPE
    PROJ_TYPE:=$(PROJ_NAME_TYPE)
  endif
endif
ifndef PROJ_TYPE
  PROJ_TYPE:=$(if $(IS_IAP),IAP,ACU HMI)
endif
# Define the list of project types specified as targets on the command line.
# These targets are combined as one and used to update the project_config.mk.
PARAM_PROJ_TYPE:=$(filter $(MAKECMDGOALS),$(BASE_PROJ_TYPES))


################################################################################
##
## Define the project's source directories.
##

ifdef PROJ_TYPE
  # SDK projects only compile firmware in selected folders.
  ARCH_SCOPES:=XCategory Category ProjectSpecific
  ARCH_LAYERS:=Application Common Hbl Service
  ARCH_DIRS:=$(call func_combine,$(DIRSRC),$(call func_combine,$(ARCH_SCOPES),$(ARCH_LAYERS)))
  ifeq ($(PROJ_TYPE),ACU)
    SOURCE_DIRS:=$(wildcard $(call func_combine,$(ARCH_DIRS),all acu acuhmi acuexpacu))
  endif
  ifeq ($(PROJ_TYPE),HMI)
    SOURCE_DIRS:=$(wildcard $(call func_combine,$(ARCH_DIRS),all hmi acuhmi hmiexphmi))
  endif
  ifeq ($(PROJ_TYPE),EXPACU)
    SOURCE_DIRS:=$(wildcard $(call func_combine,$(ARCH_DIRS),all expacu acuexpacu))
  endif
  ifeq ($(PROJ_TYPE),EXPHMI)
    SOURCE_DIRS:=$(wildcard $(call func_combine,$(ARCH_DIRS),all exphmi hmiexphmi))
  endif
  ifeq ($(PROJ_TYPE),ACU EXPHMI)
    SOURCE_DIRS:=$(wildcard $(call func_combine,$(ARCH_DIRS),all acu exphmi acuhmi acuexpacu hmiexphmi))
  endif
  ifeq ($(PROJ_TYPE),HMI EXPACU)
    SOURCE_DIRS:=$(wildcard $(call func_combine,$(ARCH_DIRS),all hmi expacu acuhmi acuexpacu hmiexphmi))
  endif
  ifeq ($(PROJ_TYPE),EXPACU EXPHMI)
    SOURCE_DIRS:=$(wildcard $(call func_combine,$(ARCH_DIRS),all expacu exphmi acuexpacu hmiexphmi))
  endif
    ifeq ($(PROJ_TYPE),ACU HMI)
    SOURCE_DIRS:=$(wildcard $(call func_combine,$(ARCH_DIRS),all acu hmi acuhmi acuexpacu hmiexphmi))
  endif
  ifeq ($(PROJ_TYPE),IAP)
    SOURCE_DIRS:=$(wildcard $(ARCH_DIRS))
  endif
  ifdef MICRO_CORE
    SOURCE_DIRS+=$(DIRSRC)/XCategory/Driver/APIs\
      $(EMBOS_PATH)/source\
      $(DIRSRC)/Micros/$(DIRMICRO)/Driver\
      $(DIRDRVCFG)/$(MICRO)
  endif
else
  # Non-SDK projects compile all the firmware in the DIRSRC directory.
  SOURCE_DIRS:=$(DIRSRC)
endif


################################################################################
##
## Define primary build targets that are not filenames
##

all :: regenerate $(if $(IS_IAP),,debug-sf)
	@$(MAKE) -f $(DIRBLD)/Makefile $(if $(JOBS),-j$(JOBS)) objects
	@$(MAKE) -f $(DIRBLD)/Makefile $(PROJ_NAME)$(SUFFIX_DEBUG).s19
	@$(MAKE) -f $(DIRBLD)/Makefile merge-release
ifdef SCRIPT_AFTER_BUILD
	$(SCRIPT_AFTER_BUILD) $(SCRIPT_AFTER_BUILD_PARAMS)
endif
	@echo $(PROJ_NAME) and $(PROJ_NAME)$(SUFFIX_DEBUG) Builds Complete.

clean :: | $(DIRPRV_)
	@$(call func_clean)

ifdef IS_SDK
clobber :: clean
	@$(call func_remove_dir,$(DIRPRM))
	@$(call func_remove_dir,$(DIRPRV))
	@del /q $(DIRCFG)\*.*
	@echo   Removed configuration files in the $(DIRCFG) directory
	@$(call func_remove_dir,$(DIRSRC)\Micros\embOS)
	@$(call func_remove_dir,$(DIRDRVCFG))
	@if exist $(DIRSRC)\ProjectSpecific\Common\all\SRConfig\ClassB_Signature.h del /q $(DIRSRC)\ProjectSpecific\Common\all\SRConfig\ClassB_Signature.h
ifndef IS_IAP
	@$(DIRBLD)\updateSysConfig.bat "ACU HMI" $(DIRSYSCFG) $(DIRSRSYSCFG) $(DIRBLD)
endif
endif

debug :: regenerate $(if $(IS_IAP),,debug-sf)
	@$(MAKE) -f $(DIRBLD)/Makefile $(if $(JOBS),-j$(JOBS)) objects
	@$(MAKE) -f $(DIRBLD)/Makefile $(PROJ_NAME)$(SUFFIX_DEBUG).s19
ifdef SCRIPT_AFTER_BUILD
	$(SCRIPT_AFTER_BUILD) $(SCRIPT_AFTER_BUILD_PARAMS)
endif
	@echo $(PROJ_NAME)$(SUFFIX_DEBUG) Build Complete.

release :: regenerate
	@$(MAKE) -f $(DIRBLD)/Makefile SW05_WARNING=$(SW05_WARNING) $(if $(JOBS),-j$(JOBS)) objects
	@$(MAKE) -f $(DIRBLD)/Makefile merge-release
ifdef SCRIPT_AFTER_BUILD
	$(SCRIPT_AFTER_BUILD) $(SCRIPT_AFTER_BUILD_PARAMS)
endif
	@echo $(PROJ_NAME) Build Complete.

ifndef IS_SDK
sw05-% ::
	@$(info)
		@$(SW05_TEST_EXE) $(word 2,$(subst -, ,$@)) $(NODE) $(MILESTONE) $(wordlist 3,99,$(subst -, ,$@)) 
endif

################################################################################
##
## Define other build targets that are not filenames
##

ifdef IS_SDK
auto-build-combos ::
	@$(call func_edit_config,JOBS,1)
# Build every project type with the MKE1x-MKE1xZ microcontroller
	@$(foreach p,$(SUPPORTED_PROJ_TYPES),\
	   $(if $(wildcard $(DIRPRV_)/Build_$p_MKE1x-MKE1xZ),,\
	     $(call func_build,$p,MKE1x-MKE1xZ)))
# Build the ACU and HMI project types with every other microcontroller
	@$(foreach m,$(filter-out MKE1x-MKE1xZ,$(SUPPORTED_MICROS)),\
	   $(foreach p,$(wordlist 1,2,$(SUPPORTED_PROJ_TYPES)),\
	     $(if $(wildcard $(DIRPRV_)/Build_$p_$m),,\
	       $(call func_build,$p,$m))))
	@$(MAKE) -f $(DIRBLD)/Makefile clobber
endif

ClassB_Signature.h :: $(DIRDRVCFG_)/$(MICRO)
	@$(info )
	@$(DIRBLD)\SRRevision.exe \
	    -s $(SOURCE_DIRS)

ifndef IS_IAP
debug-sf :: | $(DIRTMP)
ifdef FILE_DEBUG_SF
	@$(FIND_FILE_BAT) $(patsubst %\,%,$(subst /,\,$(FILE_DEBUG_SF))) $(FILE_LINK_SF)
else
	@$(FIND_FILE_BAT) $(FILE_LINK_SF)
endif
endif

double-click :: help
	@$(info )
	@pause

help ::
	@type $(DIRBLD)\Help_Makefile_SDK.txt

lint ::
ifndef CFILES
	@$(MAKE) -f $(DIRBLD)/Makefile regenerate
endif
	@$(MAKE) -f $(DIRBLD)/Makefile lint-no-regen

lint-no-regen :: $(FILE_INCLUDES_) | $(DIRLNT)
	@echo    Linting project...
	@$(LINT_EXE) -os($(FILE_LINT_PROJECT)) -i$(DIRLIB) $(FILE_INCLUDES) $(FILE_LINT_CONFIG) $(FILE_LINT_FORMAT_CONFIG) -zero $(CFILES)
	@type $(FILE_LINT_PROJECT) $(if $(filter $(GNU_FORMAT),YES),| $(DIRBLD)\regex.exe -f $(DIRBLD)\lint2gcc_err.regex)

# List all terms defined by the Makefile.
list-definitions ::
	@$(foreach v,$(sort $(.VARIABLES)),$(if $(filter file,$(origin $v)),$(info $v=$(value $v))))
	@:

list-micros ::
	@$(if $(wildcard $(subst \,/,$(NUCLEUS_TOOLS_PATH))),,echo No micros are supported: "$(NUCLEUS_TOOLS_PATH)" was not found. & exit 1)
	@echo Valid values for MICRO:
	@for %%a in ($(sort $(SUPPORTED_MICROS))) do echo     %%a
	@:

list-proj-types ::
	@$(call func_list_proj_types)

list-tool-versions ::
	@$(if $(wildcard $(subst \,/,$(NUCLEUS_TOOLS_PATH))),,echo No tools were installed: "$(NUCLEUS_TOOLS_PATH)" was not found. & exit 1)
	@echo For ARM microcontrollers:
	@echo     Valid values for IAR_ARM_VERSION:
	@for /f %%i in ('dir /ad /b /on $(NUCLEUS_TOOLS_PATH)\iar\arm') do @echo         %%i
	@echo     Valid values for EMBOS_ARM_VERSION:
	@for /f %%i in ('dir /ad /b /on $(NUCLEUS_TOOLS_PATH)\embOS\arm') do @echo         %%i
	@echo For RX microcontrollers:
	@echo     Valid values for IAR_RX_VERSION:
	@for /f %%i in ('dir /ad /b /on $(NUCLEUS_TOOLS_PATH)\iar\rx') do @echo         %%i
	@echo     Valid values for EMBOS_RX_VERSION:
	@for /f %%i in ('dir /ad /b /on $(NUCLEUS_TOOLS_PATH)\embOS\rx') do @echo         %%i

log-preprocessor :: $(DIRDRVCFG_)/$(MICRO) | $(DIROUT)
	@$(info )
	@$(LOGMESSAGESCREATOR) $(DIRSRC)
	@$(LOGPREPROCESSOR)\
	    -m $(DIROUT)\$(PROJ_NAME)_Messages.xml\
	    -s $(call func_join,;,$(SOURCE_DIRS))

ifndef IS_IAP
ifdef FILE_MERGE_FVT_SF
merge-release :: $(PROJ_NAME)_with_FVT.s19 $(PROJ_NAME)_with_IAP_FVT.s19
else
merge-release :: $(PROJ_NAME).s19
	@echo Merge with FVT was skipped since FILE_MERGE_FVT_SF was not specified.
endif
else
merge-release :: $(PROJ_NAME).s19
endif

objects :: $(FILE_INCLUDES_) $(FILE_LINK_LIST_) $(OBJECTS)

regenerate :: switch-embos-ver switch-iar-ver switch-micro switch-proj-type $(DIRDRVCFG_)/$(MICRO) log-preprocessor ClassB_Signature.h | .jazzignore $(DIRCFG)/.jazzignore $(DIROUT) $(DIRTMP)
ifdef SCRIPT_BEFORE_BUILD
	$(SCRIPT_BEFORE_BUILD) $(SCRIPT_BEFORE_BUILD_PARAMS)
endif
	@$(info )
	@$(DIRBLD)\regenerate.exe\
	    -i $(DIRCX)/inc\
	    -s $(SOURCE_DIRS)

# Update project_config.mk with a new micro selection.
$(SUPPORTED_MICROS) :: | $(DIRCFG)
	@$(call func_edit_config,MICRO,$@)
	@echo Updated $(DIRCFG)\project_config.mk with "MICRO:=$@"
	@$(MAKE) -f $(DIRBLD)\Makefile switch-micro

# Update project_config.mk with a new project type selection.
$(BASE_PROJ_TYPES) :: | $(DIRCFG)
	@$(eval REQ_PROJ_TYPE:=$(call func_join,+,$(PARAM_PROJ_TYPE)))
	@$(if $(filter $(REQ_PROJ_TYPE),$(call func_join,+,$(PROJ_TYPE))),,\
	   $(if $(filter $@,$(word 1,$(PARAM_PROJ_TYPE))),\
	     $(if $(filter $(REQ_PROJ_TYPE),$(SUPPORTED_PROJ_TYPES)),\
	       $(call func_set_proj_type,$(PARAM_PROJ_TYPE)),\
	       @echo The "$(PARAM_PROJ_TYPE)" project type is not supported.\
	         & $(call func_list_proj_types))))

switch-embos-ver :: $(DIRSRC)/Micros/embOS | $(DIRPRV_)
ifdef MICRO_CORE
	@if not exist $(EMBOS_PATH)\$(COMPILER_GROUP)\
	    echo EmbOS $(MICRO_CORE) version $(EMBOS_$(MICRO_CORE)_VERSION) is not supported by IAR $(IAR_$(MICRO_CORE)_VERSION) compiler selected. \
	  & echo Use one of these valid EMBOS_$(MICRO_CORE)_VERSION versions in $(DIRCFG)/project_sr_config.mk: \
	  & for /f %%i in ('dir /b /ad /on $(NUCLEUS_TOOLS_PATH)\embOS\$(MICRO_CORE_PATH_NAME)') do if exist $(NUCLEUS_TOOLS_PATH)\embOS\$(MICRO_CORE_PATH_NAME)\%%i\$(COMPILER_GROUP) echo     %%i
	@if not exist $(EMBOS_PATH)\$(COMPILER_GROUP) exit 1
  ifneq ($(LAST_EMBOS_PATH),$(MICRO_CORE_PATH_NAME)\$(EMBOS_$(MICRO_CORE)_VERSION))
	@$(call func_clean)
	@echo LAST_EMBOS_PATH:=$(MICRO_CORE_PATH_NAME)\$(EMBOS_$(MICRO_CORE)_VERSION)>$(FILE_EMBOS_PATH)
	@echo     Switched $(PROJ_NAME) to use embOS $(MICRO_CORE) version $(EMBOS_$(MICRO_CORE)_VERSION)
  endif
endif

switch-iar-ver :: | $(DIRPRV_)
ifdef MICRO_CORE
  ifndef COMPILER_GROUP
	@echo IAR $(IAR_$(MICRO_CORE)_VERSION) compiler version selected is not valid. \
	  & echo Use one of these vaild IAR_$(MICRO_CORE)_VERSION versions in $(DIRCFG)/project_sr_config.mk: \
	  & for /f %%i in ('dir /b /ad /on $(NUCLEUS_TOOLS_PATH)\iar\$(MICRO_CORE_PATH_NAME)') do echo     %%i
	@exit 1
  endif
  ifneq ($(LAST_DIRCX),$(DIRCX))
	@$(call func_clean)
	@echo LAST_DIRCX:=$(DIRCX)>$(FILE_IAR_PATH)
	@echo     Switched $(PROJ_NAME) to build using IAR $(MICRO_CORE) version $(IAR_$(MICRO_CORE)_VERSION)
  endif
endif

switch-micro :: $(DIRDRVCFG_)/$(MICRO) | $(DIRPRV_)
ifdef MICRO
  ifneq ($(LAST_MICRO),$(MICRO))
	@$(call func_clean)
	@echo LAST_MICRO:=$(MICRO)>$(FILE_MICRO)
	@echo     Switched $(PROJ_NAME) to use the $(MICRO) microcontroller
  endif
endif

switch-proj-type :: | $(DIRPRV_)
  ifneq ($(LAST_PROJ_TYPE),$(PROJ_TYPE))
	@$(call func_clean)
	@echo LAST_PROJ_TYPE:=$(PROJ_TYPE)>$(FILE_PROJ_TYPE)
	@$(DIRBLD)\updateSysConfig.bat "$(PROJ_TYPE)" $(DIRSYSCFG) $(DIRSRSYSCFG) $(DIRBLD)
	@echo     Switched $(PROJ_NAME) to build the $(PROJ_TYPE) project type
  endif


################################################################################
##
## Define file and folder name build targets
##

.jazzignore :
	@copy /y $(DIRBLD)\Templates\.jazzignore .jazzignore>nul

$(DIRCFG) $(DIRLNT) $(DIRLST) $(DIROBJ) $(DIROUT) $(DIRPRM_) $(DIRPRV_) $(DIRSRC) $(DIRTMP) :
	@$(eval DIR:=$(subst /,\,$@))
	@if not exist $(DIR) mkdir $(DIR)\
	  & echo     Created the $(DIR) directory...

$(DIRCFG)/.jazzignore : | $(DIRCFG)
	@copy /y $(DIRBLD)\Templates\config.jazzignore $(DIRCFG)\.jazzignore>nul

$(DIRCFG)/project_config.mk $(DIRCFG)/project_sr_config.mk : | $(DIRCFG)
	@type $(DIRBLD)\Templates\$(@F)\
	 | $(DIRBLD)\regex.exe s/!!PROJ_NAME!!/$(PROJ_NAME)/g \
	                       s/!!PROJ_NAME_PREFIX!!/$(PROJ_NAME_PREFIX)/g \
	                       s/!!BASE_PROJ_TYPES!!/$(BASE_PROJ_TYPES)/g \
	                       s/!!PROJ_TYPE!!/$(PROJ_TYPE)/g \
	                       s/!!SUPPORTED_MICROS!!/$(SUPPORTED_MICROS)/g \
	                       s/!!MICRO!!/$(FORCED_MICRO)/g \
	 > $@
	@echo     Created initial $@...

$(DIRCFG)/sw05.mk : | $(DIRCFG)
	@copy $(DIRBLD)\Templates\$(@F) $(DIRCFG)\sw05.mk 1>nul
	@echo     Created initial $@...

$(DIRCFG)/$(FILE_LINKER_CONFIG) : | $(DIRCFG)
ifdef MICRO
	@copy /y $(DIRSRC)\Micros\$(DIRMICROBUILD)\$(IS_IAP)config.icf $(DIRCFG)\$(FILE_LINKER_CONFIG)>nul
	@echo     Created $(FILE_LINKER_CONFIG)
endif

$(DIRCFG)/$(FILE_LINKER_HEADER) : | $(DIRCFG)
ifdef MICRO
	@copy /y $(DIRSRC)\Micros\$(DIRMICROBUILD)\$(IS_IAP)linker_header.txt $(DIRCFG)\$(FILE_LINKER_HEADER)>nul
	@echo     Created $(FILE_LINKER_HEADER)
endif

$(DIRCFG)/$(MICRO).mk : | $(DIRCFG)
ifdef MICRO
	@copy /y $(DIRSRC)\Micros\$(DIRMICROBUILD)\micro.mk $(DIRCFG)\$(MICRO).mk>nul
	@echo     Created $(DIRCFG)/$(MICRO).mk
endif

$(DIRDRVCFG_)/$(MICRO) :
ifdef MICRO
	@robocopy $(DIRSRC)\Micros\$(DIRMICRO)\Config $(DIRDRVCFG)\$(MICRO) /mir /ndl /nfl /njh /njs >nul\
	 & exit 0
	@echo     Copied $(MICRO) driver configuration files to $(DIRDRVCFG)\$(MICRO)
	@$(DIRBLD)\updateUcPrm.bat "$(MICRO)" $(DIRDRVCFG) $(DIRBLD)
endif

$(DIRSRC)/Micros/embOS :
ifdef MICRO_CORE
	@robocopy $(EMBOS_PATH)\source \
	    $(DIRSRC)\Micros\embOS /mir /ndl /nfl /njh /njs >nul\
	 & exit 0
	@echo     Copied embOS files to $(DIRSRC)\Micros\embOS for eclipse to reference
endif

$(FILE_INCLUDES_) :
ifdef PATHS
	@for %%i in ($(PATHS)) do echo -I%%i\>>$(FILE_INCLUDES)
	@echo     Created $(FILE_INCLUDES)
endif

$(FILE_LINK_LIST_) : $(DIRCFG)/$(FILE_LINKER_HEADER)
ifdef OBJECTS
	@copy $(DIRCFG)\$(FILE_LINKER_HEADER) $(FILE_LINK_LIST)>nul
	@for %%o in ($(filter-out MicroExtInterruptJumpTable.obj,$(OBJECTS))) do echo obj\%%o>>$(FILE_LINK_LIST)
	@echo     Created $(FILE_LINK_LIST)
else
	@echo No OBJECTS. Cannot build $(FILE_LINK_LIST).
endif

ifdef FILE_MERGE_FVT_SF
$(DIRTMP_)/$(FILE_MERGE_FVT_SF).s19 : $(DIROUT)/$(PROJ_NAME).map | $(DIRTMP_)
	@$(SF_S19_BAT) $(DIROUT)\$(PROJ_NAME).map  $(DIRBLD)  $(DIRPRM)\$(FILE_MERGE_FVT_SF) $(DIRPRM) $(DIRTMP)
endif

$(PROJ_NAME).map : $(PROJ_NAME).out

$(PROJ_NAME).out : $(FILE_LINK_LIST_) $(DIRCFG)/$(FILE_LINKER_CONFIG) $(OBJECTS) | $(DIROUT)
	@$(info )
	@echo Generating $@...
	@$(DIRCX)\$(LINK) $(MICRO_LINK)\
	    --config $(DIRCFG)\$(FILE_LINKER_CONFIG)\
	    --entry $(MICRO_ENTRY)\
	     -f $(FILE_LINK_LIST)\
	     -L $(EMBOS_PATH)\$(COMPILER_GROUP)\
	    --map $(DIROUT)\$(PROJ_NAME).map\
	     -o $(DIROUT)\$(PROJ_NAME).out.tmp\
	    --place_holder $(MICRO_FLASH)_CRC,4,.MICRO_CRC_region,4
	@$(DIRCX)\$(ELFTOOL)\
	    --checksum $(MICRO_FLASH)_CRC:2,crc16,0x00;$(MICRO_FLASH)_START-$(MICRO_FLASH)_END\
	    --fill $(FILL_BYTE);$(MICRO_FLASH)_START-$(MICRO_FLASH)_END\
	    --verbose\
	    $(DIROUT)\$(PROJ_NAME).out.tmp $(DIROUT)\$(PROJ_NAME).out
	@del /f /q $(DIROUT)\$(PROJ_NAME).out.tmp

$(PROJ_NAME)$(SUFFIX_DEBUG).out : $(FILE_LINK_SF_) $(FILE_LINK_LIST_) $(DIRCFG)/$(FILE_LINKER_CONFIG) $(OBJECTS) | $(DIROUT) $(DIRTMP)
	@$(info )
	@echo Generating $@...
	@$(DIRCX)\$(LINK) $(MICRO_LINK)\
	    --config $(DIRCFG)\$(FILE_LINKER_CONFIG)\
	    --entry $(MICRO_ENTRY)\
	     -f $(FILE_LINK_LIST) $(DIROBJ)\MicroExtInterruptJumpTable.obj\
	     -L $(EMBOS_PATH)\$(COMPILER_GROUP)\
	    --map $(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).map\
	     -o $(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).out.tmp\
	     $(if $(FILE_LINK_SF),\
	       --image_input $(FILE_LINK_SF)$(COMMA)__Debug_SF$(COMMA).settingfile*$(COMMA)4\
	       --keep __Debug_SF\
	      )\
	    --place_holder $(MICRO_FLASH)_CRC,4,.MICRO_CRC_region,4
	@$(DIRCX)\$(ELFTOOL)\
	    --checksum $(MICRO_FLASH)_CRC:2,crc16,0x00;$(MICRO_FLASH)_START-$(MICRO_FLASH)_END\
	    --fill $(FILL_BYTE);$(MICRO_FLASH)_START-$(MICRO_FLASH)_END\
	    --verbose\
	    $(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).out.tmp $(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).out
	@del /f /q $(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).out.tmp

$(PROJ_NAME).s19 : $(PROJ_NAME).out | $(DIRPRV_)
	@$(info )
	@echo Generating $@...
	@$(call func_backup,$(DIROUT)\$(PROJ_NAME).s19)
	@$(DIRCX)\$(ELFTOOL) --srec $(DIROUT)\$(PROJ_NAME).out $(DIROUT)\$(PROJ_NAME).s19
	@if exist $(DIRPRV)\$(PROJ_NAME).s19\
	    fc $(DIRPRV)\$(PROJ_NAME).s19 $(DIROUT)\$(PROJ_NAME).s19 >nul\
	 && echo No S19 Differences Encountered\
	  & exit 0

ifndef IS_IAP
ifdef FILE_MERGE_FVT_SF
$(PROJ_NAME)_with_FVT.s19 : $(PROJ_NAME).s19 $(DIRTMP_)/$(FILE_MERGE_FVT_SF).s19
	@$(MERGE_RELEASE_BAT) $(DIROUT) $(DIRBLD) $(PROJ_NAME) $(FILE_MERGE_FVT_SF) $(FILL_BYTE) $(DIRTMP)
else
	@echo Merge with FVT was skipped since FILE_MERGE_FVT_SF was not specified.
endif
endif

ifndef IS_IAP
ifdef FILE_MERGE_FVT_SF
$(PROJ_NAME)_with_IAP_FVT.s19 : $(PROJ_NAME).s19 $(DIRTMP_)/$(FILE_MERGE_FVT_SF).s19
ifdef FILE_MERGE_IAP
	@$(MERGE_RELEASE_BAT) $(DIROUT) $(DIRBLD) $(PROJ_NAME) $(FILE_MERGE_FVT_SF) $(FILL_BYTE) $(DIRTMP) $(FILE_MERGE_IAP)
else
	@echo Merge with IAP and FVT was skipped because FILE_MERGE_IAP was not specified.
endif
else
	@echo Merge with IAP and FVT was skipped because FILE_MERGE_FVT_SF was not specified.
endif
endif

$(PROJ_NAME)$(SUFFIX_DEBUG).s19 : $(PROJ_NAME)$(SUFFIX_DEBUG).out | $(DIRPRV_)
	@$(info )
	@echo Generating $@...
	@$(call func_backup,$(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).s19)
	@$(DIRCX)\$(ELFTOOL) --srec $(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).out $(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).s19
	@if exist $(DIRPRV)\$(PROJ_NAME)$(SUFFIX_DEBUG).s19\
	    fc $(DIRPRV)\$(PROJ_NAME)$(SUFFIX_DEBUG).s19 $(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).s19 >nul\
	 && echo No S19 Differences Encountered\
	  & exit 0


################################################################################
##
## Intrinsic rules.
##

%.obj : %.c $(FILE_INCLUDES_) | $(DIRLST) $(DIROBJ)
	@echo  Compiling $*.c...
	@$(eval CUR_OPTIMIZATION:=$(OPTIMIZATION))
	@$(if $(filter $(ALT_OPTIMIZATION_FILES),$(<F)),$(eval CUR_OPTIMIZATION:=$(ALT_OPTIMIZATION)))
	@$(DIRCX)\$(CC) $(MICRO_CFLAGS) $(CUR_OPTIMIZATION) $(CFLAGS) $(DBG_OPTION)\
	    --no_wrap_diagnostics\
	    -f $(FILE_INCLUDES)\
	    -l $(DIRLST)\$*.lst\
	    -o $(DIROBJ)\$@\
	     $<\
	     $(if $(filter $(SW05_WARNING),YES),| $(NUCLEUS_TOOLS_PATH)\sw05\sw05_warning_parser.exe)\
	     $(if $(filter $(GNU_FORMAT),YES),| $(DIRBLD)\regex.exe -f $(DIRBLD)\iar2gcc_err.regex)

%.obj : %.s37 $(FILE_INCLUDES_) | $(DIRLST) $(DIROBJ)
	@echo Assembling $*.s37...
	@$(DIRCX)\$(AS) $(MICRO_AFLAGS) $(AFLAGS)\
	    -f $(FILE_INCLUDES)\
	    -l $(DIRLST)\$*.lst\
	    -o $(DIROBJ)\$@\
	     $<\
	     $(if $(filter $(GNU_FORMAT),YES),| $(DIRBLD)\regex.exe -f $(DIRBLD)\iar2gcc_err.regex)

%.lnt : %.c $(FILE_INCLUDES_) | $(DIRLNT)
	@$(LINT_EXE) -os($(DIRLNT)\$*.lnt) -i$(DIRLIB) $(FILE_INCLUDES) $(FILE_LINT_CONFIG) $(FILE_LINT_FORMAT_CONFIG) -b -u $< & if not ERRORLEVEL 1 del /f/q $(DIRLNT)\$*.lnt
	@if not exist $(DIRLNT)\$*.lnt echo     Linted $*.c -- No messages& copy /y nul $(DIRLNT)\$*.lnt>nul
	@if exist $(DIRLNT)\$*.lnt echo     Linted $*.c...& type $(DIRLNT)\$*.lnt $(if $(filter $(GNU_FORMAT),YES),| $(DIRBLD)\regex.exe -f $(DIRBLD)\lint2gcc_err.regex)


# Definitions that avoid simple problems.

.DEFAULT_GOAL := double-click
.SUFFIXES :
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-print-directory
MAKEFLAGS += --warn-undefined-variables
