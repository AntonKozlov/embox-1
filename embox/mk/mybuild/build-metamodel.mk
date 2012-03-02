# Generated by Xpand using M2Make template.

# Meta model for 'build' package.

ifndef __mybuild_build_metamodel_mk
__mybuild_build_metamodel_mk := 1

include mk/mybuild/myfile-metamodel.mk

include mk/model/metamodel_impl.mk

# Create meta objects.
Build := \
	$(call eMetaModelCreate,Build)

Build_Build := \
	$(call eMetaClassCreate,$(Build),Build_Build)
Build_Build_modules := \
	$(call eMetaReferenceCreate,$(Build_Build),Build_Build_modules)

Build_ModuleInstance := \
	$(call eMetaClassCreate,$(Build),Build_ModuleInstance)
Build_ModuleInstance_configuration := \
	$(call eMetaReferenceCreate,$(Build_ModuleInstance),Build_ModuleInstance_configuration)
Build_ModuleInstance_type := \
	$(call eMetaReferenceCreate,$(Build_ModuleInstance),Build_ModuleInstance_type)
Build_ModuleInstance_depends := \
	$(call eMetaReferenceCreate,$(Build_ModuleInstance),Build_ModuleInstance_depends)
Build_ModuleInstance_options := \
	$(call eMetaReferenceCreate,$(Build_ModuleInstance),Build_ModuleInstance_options)

Build_OptionInstance := \
	$(call eMetaClassCreate,$(Build),Build_OptionInstance)
Build_OptionInstance_module := \
	$(call eMetaReferenceCreate,$(Build_OptionInstance),Build_OptionInstance_module)
Build_OptionInstance_value := \
	$(call eMetaAttributeCreate,$(Build_OptionInstance),Build_OptionInstance_value)

# Initializes the objects and relations between them.
define __build_init
	$(call eMetaModelInit,$(Build),build,build)

	$(call eMetaClassInit,$(Build_Build),Build,,)
	$(call eMetaReferenceInit,$(Build_Build_modules),modules,$(Build_ModuleInstance),$(Build_ModuleInstance_configuration),changeable many containment)

	$(call eMetaClassInit,$(Build_ModuleInstance),ModuleInstance,,)
	$(call eMetaReferenceInit,$(Build_ModuleInstance_configuration),configuration,$(Build_Build),$(Build_Build_modules),changeable container)
	$(call eMetaReferenceInit,$(Build_ModuleInstance_type),type,$(MyFile_Module),,changeable)
	$(call eMetaReferenceInit,$(Build_ModuleInstance_depends),depends,$(Build_ModuleInstance),,changeable many)
	$(call eMetaReferenceInit,$(Build_ModuleInstance_options),options,$(Build_OptionInstance),$(Build_OptionInstance_module),changeable many containment)

	$(call eMetaClassInit,$(Build_OptionInstance),OptionInstance,,)
	$(call eMetaReferenceInit,$(Build_OptionInstance_module),module,$(Build_ModuleInstance),$(Build_ModuleInstance_options),changeable container)
	$(call eMetaAttributeInit,$(Build_OptionInstance_value),value,changeable)

endef # __build_init

# Binds objects to instance classes and features to properties.
define __build_bind
	$(call eMetaClassBind,$(Build_Build),BuildBuild)
	$(call eMetaFeatureBind,$(Build_Build_modules),modules)

	$(call eMetaClassBind,$(Build_ModuleInstance),BuildModuleInstance)
	$(call eMetaFeatureBind,$(Build_ModuleInstance_configuration),configuration)
	$(call eMetaFeatureBind,$(Build_ModuleInstance_type),type)
	$(call eMetaFeatureBind,$(Build_ModuleInstance_depends),depends)
	$(call eMetaFeatureBind,$(Build_ModuleInstance_options),options)

	$(call eMetaClassBind,$(Build_OptionInstance),BuildOptionInstance)
	$(call eMetaFeatureBind,$(Build_OptionInstance_module),module)
	$(call eMetaFeatureBind,$(Build_OptionInstance_value),value)

endef # __build_bind

$(def_all)

$(call __build_init)
$(call __build_bind)

$(call eMetaModelFreeze,$(Build))

endif # __mybuild_build_metamodel_mk

