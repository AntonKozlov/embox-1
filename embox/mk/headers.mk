
include mk/mybuild/resource.mk
include mk/core/define.mk
include mk/mybuild/check.mk

# By module get header
HEADERS_BUILD := \
  $(patsubst %,$(OBJ_DIR)/mods/%.h,$(subst .,/,$(basename $(APIS_BUILD))))

# By header get module
__header_mod = \
  $(filter $(subst /,.,$(patsubst $(abspath $(OBJ_DIR))/mods/%.h,%,$(abspath $@))).%,$(APIS_BUILD))

__header_gen = \
  $(subst $(\n),\n,$(call __header_template,$(__header_mod)))

define get_subs
	$(filter $(get $1.subTypes),$(suffix $(MODS_ENABLE_OBJ)))
endef

$(call def,get_subs)

# 1. Header module name
define __header_template
/* Autogenerated file. Do not edit. */

#ifndef __MOD_HEADER__$(subst .,__,$1)
#define __MOD_HEADER__$(subst .,__,$1)
$(foreach impl,$(call get_subs,$1),$(\n)// impl: \
  $(impl)$(foreach header,$(strip $(patsubst $(abspath $(SRC_DIR))/%,%,
                 $(abspath $(call module_get_headers,$(impl))))) \
      ,$(\n)$(\h)include __impl_x($(header))))

#endif /* __MOD_HEADER__$(subst .,__,$1) */

endef

$(HEADERS_BUILD): mk/image.mk $(AUTOCONF_DIR)/mods.mk
	@$(MKDIR) $(@D) && printf "%b" '$(__header_gen)' > $@
