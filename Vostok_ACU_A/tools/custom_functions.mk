################################################################################
# Define custom makefile functions

COMMA:=,
SPACE:=
SPACE+=

# Backup a file to the save directory.
#   $1 = file to backup
define func_backup
  @if exist $1 echo Backing up $(notdir $1) file... & move /y $1 $(DIRPRV)\$(notdir $1) >nul
endef

# Build the project using the specified parameters
#	$1 = project to use for the build. Base types separated by '+'.
#	$2 = micro to use for the build
define func_build
  $(eval PT:=$(subst +, ,$1))
  @echo ________________________________________________________________________________\
   & echo.\
   & echo.\
   & echo                 Project Type: $1\
   & echo                        Micro: $2\
   & echo.\
   & echo ________________________________________________________________________________
  @$(MAKE) -f $(DIRBLD)/Makefile $2 $(PT)
  @$(MAKE) -f $(DIRBLD)/Makefile GNU_FORMAT=NO debug
  @echo Successfully built $(PT) project type with the $2 micro>$(DIRPRV)\Build_$1_$2
endef

# Delete generated files and folders.
#   No parameters
define func_clean
  @$(call func_backup,$(DIROUT)\$(PROJ_NAME).s19)
  @$(call func_backup,$(DIROUT)\$(PROJ_NAME)$(SUFFIX_DEBUG).s19)
  @$(call func_remove_dir,$(DIROUT))
  @$(call func_remove_dir,$(DIRLNT))
  @$(call func_remove_dir,$(DIRLST))
  @$(call func_remove_dir,$(DIROBJ))
  @$(call func_remove_dir,$(DIRTMP))
endef

# Combine each element of the first list with each element of the second list.
#	$1 = first list
#	$2 = second list
define func_combine
  $(foreach item1,$1,$(foreach item2,$2,$(item1)/$(item2)))
endef

# Update a field in the project_config.mk file.
#	$1 = label for the field
#	$2 = new value for the label
define func_edit_config
  @ren $(DIRCFG)\project_sr_config.mk project_sr_config.bak
  @type $(DIRCFG)\project_sr_config.bak \
    | $(DIRBLD)\regex.exe "s/^^\s*$1\s*:?=.*/$1:=$2/m" \
    > $(DIRCFG)\project_sr_config.mk
  @del /f /q $(DIRCFG)\project_sr_config.bak
  @ren $(DIRCFG)\project_config.mk project_config.bak
  @type $(DIRCFG)\project_config.bak \
    | $(DIRBLD)\regex.exe "s/^^\s*$1\s*:?=.*/$1:=$2/m" \
    > $(DIRCFG)\project_config.mk
  @del /f /q $(DIRCFG)\project_config.bak
endef

# Use separator $1 to join all the elements in list $2.
#	$1 = List separator
#	$2 = List of elements
define func_join
$(subst $(SPACE),$1,$(strip $2))
endef

# List the valid project types.
#   No parameters
define func_list_proj_types
  @echo Valid values for PROJ_TYPE:
  @for %%i in ($(subst +, ,$(patsubst %,"%",$(SUPPORTED_PROJ_TYPES)))) do echo     %%~i
endef

# Delete a directory and all it's contents.
# Function duplicates the command as a work-around for a Windows defect.
#   $1 = path to the directory.
define func_remove_dir
  @if exist $1 echo   Removing the $1 directory... & rmdir /q /s $1 >nul & exit 0
  @if exist $1 rmdir /q /s $1 >nul & exit 0
endef

# Set the PROJ_TYPE parameter in the project_config.mk file.
#   $1 = The new value for PROJ_TYPE
define func_set_proj_type
  @$(call func_edit_config,PROJ_TYPE,$1)
  @echo Updated $(DIRCFG)\project_sr_config.mk with "PROJ_TYPE:=$1"
  @$(MAKE) -f $(DIRBLD)\Makefile switch-proj-type
endef
