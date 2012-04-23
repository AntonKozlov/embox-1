# $Id$
#
# Dependency Injection model generator. 
#
# Author: Eldar Abusalimov
#

ifndef _codegen_di_mk_
_codegen_di_mk_ := 1

DEPSINJECT_SRC := $(SRCGEN_DIR)/depsinject.c
DEPSINJECT_OBJ := $(OBJ_DIR)/depsinject.o

ifeq (0,1)
generate_header := \
  /* Auto-generated EMBuild Dependency Injection model file. Do not edit. */\n

generate_includes := \n\#include <framework/mod/embuild.h>\n

__printf_escape = '$(subst ",\",$1)'
endif

$(DEPSINJECT_SRC) : mk/codegen-di.mk $(configfiles_model_mk)
	$(MAKE) depsinject=1 -f mk/script/depsinject.mk > $@

$(DEPSINJECT_OBJ) : $(AUTOCONF_DIR)/config.h
$(DEPSINJECT_OBJ) : $(DEPSINJECT_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -std=gnu99 -D__EMBUILD__ -o $@ -c $<

-include $(DEPSINJECT_OBJ:.o=.d)

endif
