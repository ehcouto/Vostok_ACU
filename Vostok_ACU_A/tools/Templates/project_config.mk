################################################################################
## Project Specific Makefile definitions.
##
## This file is maintained by the owner of the project.
##


################################################################################
##
## Change the name of the generated files in the exe directory.
## By default, the system uses the project folder as the project name.
## You can override that value by entering a value here.
##
PROJ_NAME:=!!PROJ_NAME!!


################################################################################
##
## Select the IAP .S19 to merge with the release .S19 file.
## If the relevant files are found
##      the release .S19 + FILE_MERGE_IAP + FILE_MERGE_FVT_SF will be merged
##      to create !!PROJ_NAME!!_with_IAP_FVT.s19
## The FILE_MERGE_IAP file must be placed in the config/build folder.
##
#FILE_MERGE_IAP:=!!PROJ_NAME_PREFIX!!_IAP_A.s19


################################################################################
##
## Specify the *.bin setting file to merge in the release build.
## If the relevant files are found
##      the release .S19 + FILE_MERGE_IAP + FILE_MERGE_FVT_SF will be merged
##      to create !!PROJ_NAME!!_with_IAP_FVT.s19
## If the relevant files are found
##      the release .S19 + FILE_MERGE_FVT_SF will be merged
##      to create !!PROJ_NAME!!_with_FVT.s19
## The FILE_MERGE_FVT_SF file must be placed in the config/build folder.
## Specify only the file name.The file extension has not to be specified
## Don't add the path where the file is located and do not add the extension file name
##
#FILE_MERGE_FVT_SF:=!!PROJ_NAME_PREFIX!!_FVT


################################################################################
##
## Specify the *.bin setting file to link into the debug build.
## There are three different ways to use this definition:
##      1. Specify the absolute path and filename for the setting file to use.
##      2. Specify the relative path and filename for the setting file to use.
##         The path should be relative to the project folder.
##      3. Specify a folder to search. The newest *.bin file will be used.
## If the setting file *.bin file is found, the linker will include the setting
##      file image in the debug *.out and debug *.s19 output files.
##
#FILE_DEBUG_SF:=C:/GESE_Category_Integration/Application


################################################################################
##
## Specify IAR and PC-Lint console output format.
##      YES = Convert IAR and PC-Lint console output to GNU format
##      NO = Show native IAR console output in console window
##           Show format specified in Format.lnt on console window.
##
## Why should I set GNU_FORMAT to YES?
##  PROS:
##    - Double-clicking on a warning or error in eclipse will automatically jump
##      to the file and line where the warning or error was detected.
##  CONS:
##    - Lengthens the compile time by about 20%.
##
## Refer to the link below to configure Eclipse to jump to the warning or error
## without slowing the compile time.
##    - https://confluence.whirlpoolcorp.com/x/Dq5gBQ
##
GNU_FORMAT:=YES


################################################################################
##
## Script or batch file to run before the build.
## Include a path in the definition if necessary.
## The working directory will be the project's root directory.
## Consider how the script should be handled on server builds.
## It is recommended that you place the script in the config folder.
## You may optionally specify parameters for the script.
##
SCRIPT_BEFORE_BUILD:=
SCRIPT_BEFORE_BUILD_PARAMS=


################################################################################
##
## Script or batch file to run after the build.
## Include a path in the definition if necessary.
## The working directory will be the project's root directory.
## Consider how the script should be handled on server builds.
## It is recommended that you place the script in the config folder.
## You may optionally specify parameters for the script.
##
SCRIPT_AFTER_BUILD:=
SCRIPT_AFTER_BUILD_PARAMS=


################################################################################
##
## Select the number of jobs that you wish to use while compiling.
## Using multiple jobs will split the work across multiple threads.
## This is especially recommended for multi-core processors.
##
##  JOBS:=1                         No parallel processing
##  JOBS:=4                         Execute up to 4 jobs in parallel
##  JOBS:=8                         Execute up to 8 jobs in parallel
##  JOBS:=$(NUMBER_OF_PROCESSORS)   Execute a job for each processor
##
JOBS:=$(NUMBER_OF_PROCESSORS)

