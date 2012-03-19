ifndef __mk_ugly_mk
__mk_ugly_mk := 1

include mk/conf/roots.mk
include mk/conf/runlevel.mk

#
# Headers.
#

include mk/mybuild/check.mk
include mk/mybuild/mybuild.mk

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
      ,$(\n)$(\h)include __impl_x($(header))$(\n)))

#endif /* __MOD_HEADER__$(subst .,__,$1) */

endef


#
# Image.
#

SRC_TO_OBJ = \
	$(call filter-patsubst, \
		$(abspath $(ROOT_DIR))%.c $(abspath $(ROOT_DIR))%.S,$(OBJ_DIR)%.o, \
		$(filter-out %.lds.S,$(abspath $1)))

NEW_SRC_TO_OBJ = \
	$(call filter-patsubst, \
		$(abspath $(ROOT_DIR))%.c $(abspath $(ROOT_DIR))%.S,$(OBJ_DIR)%.o, \
			$(abspath $1))

DEF_C_BUILD_RULE = \
	$(call ___DEF_C_BUILD_RULE,$(call NEW_SRC_TO_OBJ,$1),$(OBJ_DIR)/%.o,$(ROOT_DIR)/%.c)

BUILD_RULE_TEMPLATE = \
	$1 += $$(strip $$1) $(\n)\
	$$1 : $$2 : $$3 \
	$(\n)$(\t)$2

define ___DEF_C_BUILD_RULE
	$(eval \
		$(call BUILD_RULE_TEMPLATE,
			OBJS_BUILD,
			$$(CC) $$(CFLAGS) $$(CPPFLAGS) $$(CCFLAGS) -o $$@ -c $$<))
endef

define ___DEF_S_BUILD_RULE
	$(eval \
		$(call BUILD_RULE_TEMPLATE,
			OBJS_BUILD,
			$$(CC) $$(CFLAGS) $$(CPPFLAGS) $$(CCFLAGS) -o $$@ -c $$<))
endef

LIB_FILE = \
	$(foreach 1,$1,$(LIB_DIR)/$(get $1.qualifiedName).a)

# param $1 is module obj
module_get_files = \
	$(foreach s,$(get $1.sources),$(get s->fileFullName))
module_get_sources = \
	$(filter %.c %.S,$(module_get_files))
module_get_headers = \
	$(filter %.h,$(module_get_files))
module_get_objects = \
	$(call SRC_TO_OBJ,$(module_get_sources))

filter_abstract_modules = \
	$(strip $(foreach m,$1,$(if $(get m->isAbstract),$m)))
filter_static_modules = \
	$(strip $(foreach m,$1,$(if $(get m->isStatic),$m)))

ROOTFS_LABEL := mybuild.lang.InitFS

define module_get_rootfs 
	$(for fileMember <- $(get $1.sources),
		annot <- $(get fileMember->annotations),
		annotType <- $(get annot->type),
		$(if $(eq $(ROOTFS_LABEL),$(get $(get annot->type).qualifiedName)),
			$(get fileMember->fileFullName)))
endef

LABEL-IncludePath := mybuild.lang.IncludePath
LABEL-DefineMacro := mybuild.lang.DefineMacro

# 1. Module.
define define_mod_obj_rules
	${eval 
		$(for mod<-$1,
				src<-$(get mod->sources),
				obj<-$(call SRC_TO_OBJ,$(get src->fileFullName)),
				$(for annot <- $(get src->annotations),
						annotType <- $(get annot->type),
						annotName <- $(get annotType->qualifiedName),
						annotBind <- $(get annot->bindings),
						opt <- $(get annotBind->option),
						optName <- $(get opt->name),
						optValue <- $(get $(get annotBind->optionValue).value),
						$(if $(and \
								$(eq $(annotName),$(LABEL-IncludePath)),
								$(eq $(optName),value)),
						$(obj) : override CCFLAGS += -I'$(optValue)'$(\n)) 
						$(if $(and \
								$(eq $(annotName),$(LABEL-DefineMacro)),
								$(eq $(optName),value)),
						$(obj) : override CCFLAGS += -D'$(optValue)'$(\n))) 
				$(obj) : override CPPFLAGS += \
					-D__EMBUILD_MOD__='$(subst .,__,$(get mod->qualifiedName))'$(\n))}
endef

# 1. Library module.
define define_lib_rules
	${eval \
		$(call LIB_FILE,$1) : $(call module_get_objects,$1)$(\n)
			$(\t)$(AR) $(ARFLAGS) $$@ $$(^:%= \$$(\n)	%)
	}
endef


#
# Generated dependencies graph.
#

mod_package = $(basename $(mod))
mod_name = $(patsubst .%,%,$(suffix $(mod)))

# The same as DEPS-$(mod), at least for now.
mod_requires = $(DEPS-$(mod))
# This is very inefficient algorithm but it does work.
mod_provides = $(filter-out $(mod),$(foreach m,$(MODS_BUILD), \
  $(if $(filter $(mod),$(call MOD_DEPS_DAG,$(m))),$(m)) \
))

c_mod = $(call c_escape,$(mod))
c_dep = $(call c_escape,$(dep))
c_package = $(call c_escape,$(package))

c_escape = $(subst .,__,$(1))

c_str_escape = \
  \n\t\t"$(subst $(\n),\\\\n"\n\t\t",$(subst $(\t),\\t,$(subst ",\",$1)))"

eol-trim = $(if $(findstring $(\s)\n,$1),$(call $0,$(subst $(\s)\n,\n,$1)),$1)

cond_flags = NYI
#cond_flags =   $(if $(strip $(CFLAGS-$2) $(CPPFLAGS-$2)), \
    $1 $(strip $(CFLAGS-$2) $(CPPFLAGS-$2)) \
  ) \

package_def = \
  \n\n/* Package: $(package) */ \
  \nMOD_PACKAGE_DEF($(c_package), "$(package)");

generate_package_defs = $(call eol-trim,\n/* Package definitions. */\
  $(foreach package,$(sort generic $(basename $(for m <- $(MODS_BUILD), \
        $(get m->qualifiedName)))), \
    $(package_def) \
  ) \
)\n

mod_def = \
  \n\n/* Mod: $(mod) */ \
  \nMOD_DEF($(c_mod), $(call c_escape,$(mod_package)), "$(mod_name)", \
    $(call c_str_escape,$(value BRIEF-$(mod))), \
    $(call c_str_escape,$(value DETAILS-$(mod))));

generate_mod_defs = $(call eol-trim,\n/* Mod definitions. */\
  $(for m <- $(MODS_BUILD) $(LIBS_BUILD), \
        mod <- $(get m->qualifiedName), \
    $(mod_def) \
  ) \
  $(foreach runlevel,0 1 2 3, \
    $(foreach mod,$(addprefix generic.runlevel$(runlevel)_,init fini), \
      $(mod_def) \
    ) \
  ) \
)\n
    
generate_mod_deps = $(strip \n/* Mod deps. */\
  $(for m <- $(MODS_BUILD) $(LIBS_BUILD), \
        mod <- $(get m->qualifiedName), \
    $(for obj_dep <- $(get m->depends), \
          dep <- $(get obj_dep->qualifiedName), \
      \nMOD_DEP_DEF($(c_mod), $(c_dep)); \
    ) \
    $(for obj_dep <- $(filter $(get m->subTypes),$(suffix $(MODS_ENABLE_OBJ))), \
          dep <- $(get obj_dep->qualifiedName), \
      \nMOD_DEP_DEF($(c_mod), $(c_dep)); \
    ) \
    $(if $(value RUNLEVEL-$(mod)), \
      \nMOD_DEP_DEF(generic__runlevel$(RUNLEVEL-$(mod))_init, $(c_mod)); \
      \nMOD_DEP_DEF($(c_mod), generic__runlevel$(RUNLEVEL-$(mod))_fini); \
    ) \
  ) \
)\n




# 1. ResourceSet
define configfiles_do_link
	$(invoke $(new Linker).linkAgainst,$1,
		$(__myfile_resource_set))
endef







$(def_all)


endif # __mk_ugly_mk
