# $Id$
#
#   Date: Mar 6, 2012
# Author: Anton Kozlov
#

ifndef __mybuild_config_linker_mk
__mybuild_config_linker_mk:= 1

include mk/model/linkage.mk

define class-ConfigLinker
	$(super Linker)

	$(method getResourceImportNormalizers,
		$(for root <- $(get 1->contentsRoot),
			$(with $(get root->name),
				$(if $1,$1.*))))

endef

$(def_all)

endif # __mybuild_config_linker_mk
