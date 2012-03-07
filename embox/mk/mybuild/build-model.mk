# Generated by Xpand using M2Make template.

# Model of 'build' package.

ifndef __mybuild_build_model_mk
__mybuild_build_model_mk := 1

include mk/mybuild/myfile-model.mk

include mk/model/model_impl.mk

#
# Model object 'Build'.
#
# The following features are defined:
#   - reference 'modules'
#
define class-BuildBuild
	$(eobject Build_Build,
		BuildBuild,,)

	# Property 'modules... : BuildModuleInstance'.
	$(eobject-reference Build_Build_modules,
		modules,BuildModuleInstance,configuration,changeable many containment)

	$(map moduleInstanceStore... : BuildModuleInstance)

	# PROTECTED REGION ID(Build) ENABLED START
	# Args:
	#  1. Mybuild instance
	$(method loadFromMybuild,
		$(set modules, 
			$(for mybuild <- $1,
				configResSet <- $(get mybuild->configResourceSet),
				configResource <- $(get configResSet->resources),
				cfgFileContent <- $(get configResource->contentsRoot),
				cfgConfiguration <- $(firstword $(get cfgFileContent->configurations)), #FIXME
				cfgInclude <- $(get cfgConfiguration->includes),
				module <- $(get cfgInclude->module),
				
				$(invoke moduleInstance,$(module)))))

	
	# Args:
	#  1. MyModule instance
	#  2. (Optional) Configuration
	$(method moduleInstance,
		$(for mod <- $1,
			$(or $(get moduleInstanceStore/$(mod)),
				$(firstword $(set moduleInstanceStore/$(mod),
						$(new BuildModuleInstance,$(mod),
							$(if $(value 2),$2)))
					$(foreach d,$(get mod->depends),
						$(invoke moduleInstance,$d))))))
	$(method listInstances,
		$(for i <- modules,
			$(get i->name)))

	$(if $(value 1), 
		$(invoke loadFromMybuild,$1))

	# PROTECTED REGION END
endef

#
# Model object 'ModuleInstance'.
#
# The following features are defined:
#   - reference 'configuration'
#   - reference 'type'
#   - reference 'depends'
#   - reference 'options'
#
# Args:
#  1. (Optional) Module type
#  2. (Optional) Configuration
#  3. (Optional) Depends  
#  4. (Optional) Options  
define class-BuildModuleInstance
	$(eobject Build_ModuleInstance,
		BuildModuleInstance,,)

	# Property 'configuration : BuildBuild'.
	$(eobject-reference Build_ModuleInstance_configuration,
		configuration,BuildBuild,modules,changeable container)

	# Property 'type : MyModule'.
	$(eobject-reference Build_ModuleInstance_type,
		type,MyModule,,changeable)

	# Property 'depends... : BuildModuleInstance'.
	$(eobject-reference Build_ModuleInstance_depends,
		depends,BuildModuleInstance,,changeable many)

	# Property 'options... : BuildOptionInstance'.
	$(eobject-reference Build_ModuleInstance_options,
		options,BuildOptionInstance,module,changeable many containment)

	# PROTECTED REGION ID(ModuleInstance) ENABLED START
	
	$(if $(value 1),
		$(set type,$1))

	$(if $(value 2),
		$(set configuration,$2))

	$(if $(value 3),
		$(set depends,$3))

	$(if $(value 4),
		$(set options,$4))
		
	# PROTECTED REGION END
endef

#
# Model object 'OptionInstance'.
#
# The following features are defined:
#   - reference 'module'
#   - attribute 'value'
#
define class-BuildOptionInstance
	$(eobject Build_OptionInstance,
		BuildOptionInstance,,)

	# Property 'module : BuildModuleInstance'.
	$(eobject-reference Build_OptionInstance_module,
		module,BuildModuleInstance,options,changeable container)

	# Property 'value'.
	$(eobject-attribute Build_OptionInstance_value,
		value,changeable)

	# PROTECTED REGION ID(OptionInstance) ENABLED START
#	# TODO Add custom implementation here and remove this comment.
	# PROTECTED REGION END
endef

$(def_all)

endif # __mybuild_build_model_mk

