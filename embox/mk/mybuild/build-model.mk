# Generated by Xpand using M2Make template.

# Model of 'build' package.

ifndef __mybuild_build_model_mk
__mybuild_build_model_mk := 1

include mk/model/model_impl.mk

#
# Model object 'Build'.
#
# The following features are defined:
#   - reference 'configuration'
#   - reference 'modules'
#
define class-Build
	# Extends 'EObject' class (implicitly).
	$(eobject Build_Build,
		Build,,)

	# Property 'configuration : CfgConfiguration'.
	# Property 'configuration_link : ELink'.
	$(eobject-reference Build_Build_configuration,
		configuration,CfgConfiguration,,changeable linkable)

	# Property 'modules... : ModuleInstance'.
	$(eobject-reference Build_Build_modules,
		modules,ModuleInstance,build,changeable many containment)

	# PROTECTED REGION ID(Build_Build) ENABLED START
	$(property-field issueReceiver : IssueReceiver)
	
	$(map moduleInstanceByName : BuildModuleInstance)

	# PROTECTED REGION END
endef

#
# Model object 'ModuleInstance'.
#
# The following features are defined:
#   - reference 'build'
#   - reference 'type'
#   - reference 'dependent'
#   - reference 'depends'
#   - reference 'contains'
#   - reference 'contained'
#   - reference 'options'
#   - reference 'afterDepends'
#   - reference 'includeMember'
#
define incrFieldSetter
	$(if $(filter +,$3),
		$(for arg <- $2,
			$(if $(filter $(arg),$(get-field $1)),
				,
				$(set-field+ $1,$(arg)))),
		$(set-field depends,$2))
endef
	
define class-ModuleInstance
	# Extends 'EObject' class (implicitly).
	$(eobject Build_ModuleInstance,
		ModuleInstance,,)

	# Property 'build : Build'.
	$(eobject-reference Build_ModuleInstance_build,
		build,Build,modules,changeable container)

	# Property 'type : MyModuleType'.
	$(eobject-reference Build_ModuleInstance_type,
		type,MyModuleType,,changeable)

	# Property 'dependent... : ModuleInstance'.
	$(eobject-reference Build_ModuleInstance_dependent,
		dependent,ModuleInstance,depends,changeable many)

	# Reference 'depends' [0..*]: bidirectional, derived.
	$(property depends... : ModuleInstance)
	# PROTECTED REGION ID(Build_ModuleInstance_depends) ENABLED START
	
	$(field depends... : ModuleInstance)

	$(getter depends,
		$(get-field depends))

	$(setter depends,
		$(if $(filter +,$2),
			$(for arg <- $1,
				$(if $(filter $(arg),$(get-field depends)),
					,
					$(set-field+ depends,$(arg)))),
			$(set-field depends,$1)))

	# PROTECTED REGION END

	# Reference 'contains' [0..*]: bidirectional, derived.
	$(property contains... : ModuleInstance)
	# PROTECTED REGION ID(Build_ModuleInstance_contains) ENABLED START
	$(field contains... : ModuleInstance)

	$(getter contains,
		$(get-field contains))

	$(setter contains,
		$(if $(filter +,$2),
			$(for arg <- $1,
				$(if $(filter $(arg),$(get-field contains)),
					,
					$(set-field+ contains,$(arg)))),
			$(set-field contains,$1)))

	# PROTECTED REGION END

	# Property 'contained : ModuleInstance'.
	$(eobject-reference Build_ModuleInstance_contained,
		contained,ModuleInstance,contains,changeable)

	# Property 'options... : OptionInstance'.
	$(eobject-reference Build_ModuleInstance_options,
		options,OptionInstance,module,changeable many containment)

	# Property 'afterDepends... : ModuleInstance'.
	$(eobject-reference Build_ModuleInstance_afterDepends,
		afterDepends,ModuleInstance,,changeable many)

	# Property 'includeMember : CfgInclude'.
	# Property 'includeMember_link : ELink'.
	$(eobject-reference Build_ModuleInstance_includeMember,
		includeMember,CfgInclude,,changeable linkable)

	# PROTECTED REGION ID(Build_ModuleInstance) ENABLED START
	# PROTECTED REGION END
endef

#
# Model object 'OptionInstance'.
#
# The following features are defined:
#   - reference 'module'
#   - reference 'option'
#   - reference 'value'
#
define class-OptionInstance
	# Extends 'EObject' class (implicitly).
	$(eobject Build_OptionInstance,
		OptionInstance,,)

	# Property 'module : ModuleInstance'.
	$(eobject-reference Build_OptionInstance_module,
		module,ModuleInstance,options,changeable container)

	# Property 'option : MyOption'.
	# Property 'option_link : ELink'.
	$(eobject-reference Build_OptionInstance_option,
		option,MyOption,,changeable linkable)

	# Property 'value : MyLiteral'.
	$(eobject-reference Build_OptionInstance_value,
		value,MyLiteral,,changeable)

	# PROTECTED REGION ID(Build_OptionInstance) ENABLED START
#	# TODO Add custom implementation here and remove this comment.
	# PROTECTED REGION END
endef

# PROTECTED REGION ID(Build) ENABLED START
# TODO Add custom implementation here and remove this comment.
# PROTECTED REGION END

$(def_all)

endif # __mybuild_build_model_mk

