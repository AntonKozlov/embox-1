# $Id: configure.mk 9 2010-03-12 19:00:31Z Eldar.Abusalimov $
#
# Author: Eldar Abusalimov
#

HOSTCC  = gcc
HOSTCPP = $(HOSTCC) -E

build_conf   := $(CONF_DIR)/build.conf
options_conf := $(CONF_DIR)/options.conf
mods_conf    := $(CONF_DIR)/mods.conf
lds_conf     := $(CONF_DIR)/lds.conf

build_mk     := $(AUTOCONF_DIR)/build.mk
mods_mk      := $(AUTOCONF_DIR)/mods.mk
config_h     := $(AUTOCONF_DIR)/config.h
config_lds_h := $(AUTOCONF_DIR)/config.lds.h

CONF_FILES     := $(build_conf) $(options_conf) $(mods_conf) $(lds_conf)
AUTOCONF_FILES := $(build_mk) $(mods_mk) $(config_h) $(config_lds_h)

-include $(build_mk) $(mods_mk)

TARGET ?= embox$(if $(PLATFORM),-$(PLATFORM))

.PHONY: check_config
check_config:
	@test -d $(CONF_DIR) $(CONF_FILES:%=-a -f %) \
		||(echo 'Error: conf directory or files do not exist' \
		&& echo 'Try "make TEMPLATE=<profile> config"' \
		&& echo '    See templates dir for possible profiles' \
		&& exit 1)
ifndef ARCH
	@echo 'Error: ARCH undefined'
	exit 1
endif

$(build_mk)     : DEFS := __BUILD_MK__
$(mods_mk)      : DEFS := __MODS_MK__
$(config_h)     : DEFS := __CONFIG_H__
$(config_lds_h) : DEFS := __CONFIG_LDS_H__

$(build_mk) $(mods_mk) :
	$(HOSTCPP) -Wp, -P -undef -nostdinc -I$(CONF_DIR) $(DEFS:%=-D%) \
	-MMD -MT $@ -MF $@.d $(MK_DIR)/confmacro.S \
		| sed 's/$$N/\n/g' > $@

$(config_h) $(config_lds_h) : 
	$(HOSTCPP) -Wp, -P -undef -nostdinc -I$(CONF_DIR) $(DEFS:%=-D%) \
	-MMD -MT $@ -MF $@.d $(MK_DIR)/confmacro.S \
		| sed 's/$$N/\n/g' | sed 's/$$define/#define/g' > $@

$(AUTOCONF_FILES) : $(MK_DIR)/configure.mk \
  | mkdir # mkdir shouldn't force target to be updated

-include $(AUTOCONF_FILES:%=%.d)

mkdir:
	@test -d $(AUTOCONF_DIR) || mkdir -p $(AUTOCONF_DIR)
