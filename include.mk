PM_DIR	:= $(dir $(lastword $(MAKEFILE_LIST)))

PM_SOURCES_RELATIVE :=  \
ParameterManager.cxx    \
SettingsIO.cxx          \
SettingsUser.cxx

PM_SOURCES := $(addprefix $(PM_DIR), $(PM_SOURCES_RELATIVE))
PM_INCLUDES := $(PM_DIR)

undefine PM_SOURCES_RELATIVE
undefine PM_INCLUDES_RELATIVE