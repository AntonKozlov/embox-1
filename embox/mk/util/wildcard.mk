# $Id$
#
# Wildcard derivatives.
#
# Author: Eldar Abusalimov
#

ifndef __util_wildcard_mk
__util_wildcard_mk := 1

include mk/core/common.mk
include mk/core/string.mk

##
# Builtin function: r-wildcard
#   $(r-wildcard pattern...)
#
# Extended version of wildcard that understands "double asterisk" pattern (**).
#
# Params:
#   1. Patterns containing wildcard characters (possibly including **).
# Return:
#   Wildcard expansion of the patterns.
# Note:
#   It does not handle properly more than one '**' tokens in single pattern.
r-wildcard = \
	$(sort $(foreach p,$1,$(call __r_wildcard,$(subst **,* *,$p))))
builtin_func-r-wildcard = $(builtin_to_function_inline)

# Called for a single pattern with '**' replaced by '* *'.
__r_wildcard = \
	$(if $(call singleword,$1), \
		$(wildcard $1), \
		$(if $(call doubleword,$1), \
			$(foreach b,$(firstword $1), \
			$(foreach e,$(word 2,$1), \
			$(foreach d,$(if $(findstring /,$e), \
					$(firstword $(subst /,/ ,$e)),*), \
				$(__r_wildcard_double)))), \
			$(error \
				Handling more than one '**' tokens is not implemented)))

# Context:
#   b. Path at the beginning, before '**': 'foo/b*'.
#   e. Part after '**':                    '*r/baz'.
#   d. $(e) up to the first '/' (if any):  '*r/' (or '*' if there is no '/').
__r_wildcard_double = \
	$(call __r_wildcard_double_loop,/, \
		$(wildcard $b$(e:*%=%) $(if $(call eq,*/,$d),$b$(e:*/%=%))))

# Start with a single separator, it will grow on each recursive subcall.
#   / -> '/*/' -> '/*/*/' -> ...
# Expand wildcards while 'foo/b*/*/...' gives non-empty result.
#   1. Separator.
#   2. Accumulator.
__r_wildcard_double_loop = \
	$(if $2,$2 )$(if $(wildcard $b$1$d),$(call $0,$1*/,$(wildcard $b$1$e)))

#
# Directory/file versions of wildcard.
#
# Based on: http://stackoverflow.com/q/9442829/545027
#

##
# Function: d-wildcard
#
# Directory-only wildcard. This version of wildcard filters out any files
# leaving only directories.
#
# Params:
#  1. Patterns containing wildcard characters
#
# Returns: Wildcard expansion of the patterns (directories only).
#
# Usage: $(call d-wildcard,pattern...)
#
d-wildcard = \
	$(patsubst %/.,%,$(filter %/.,$(wildcard $(1:%=%/.))))

##
# Function: f-wildcard
#
# File-only wildcard. This version of wildcard leaves only files in the
# expansion result.
#
# Params:
#  1. Patterns containing wildcard characters
#
# Returns: Wildcard expansion of the patterns (files only).
#
# Usage: $(call f-wildcard,pattern...)
#
f-wildcard = \
	$(filter-out $(call d-wildcard,$1),$(wildcard $1))

##
# Function: wildcard_first
#
# Tries to expand each pattern sequentially until getting non-empty expansion.
#
# Params:
#  1. Patterns containing wildcard characters
#
# Returns: The first non-empty wildcard expansion.
#
# Usage: $(call wildcard_first,pattern...)
#
wildcard_first = \
	$(call fold,,$1,__wildcard_first_fold,wildcard)

##
# Function: d-wildcard_first
#
# Directory-only version of wildcard_first.
#
# See: wildcard_first, d-wildcard
#
d-wildcard_first = \
	$(call fold,,$1,__wildcard_first_fold,d-wildcard)

##
# Function: f-wildcard_first
#
# File-only version of wildcard_first.
#
# See: wildcard_first, f-wildcard
#
f-wildcard_first = \
  $(call fold,,$1,__wildcard_first_fold,f-wildcard)

# Params:
#  1. The result of wildcard expansion of previous patterns
#  2. The next pattern to expand (if needed)
#  3. Wildcard function
__wildcard_first_fold = $(or $1,$(call $3,$2))

#
# TODO: all functions below are not used, may be drop them? -- Eldar
#

##
# Function: wildcard_relative
#
# Performs wildcard expansion within the specified base directory.
#
# Params:
#  1. Base directory
#  2. Wildcard patterns relative to the base directory
#
# Returns: Wildcard expansion of the pattern within the base directory.
#
# Usage: $(call wildcard_relative,base,pattern...)
#
# TODO: this will not for absolute paths, may be drop it at all? -- Eldar
#
  wildcard_relative = $(call __wildcard_relative,  wildcard,$1,$2)
d-wildcard_relative = $(call __wildcard_relative,d-wildcard,$1,$2)
f-wildcard_relative = $(call __wildcard_relative,f-wildcard,$1,$2)

# Expand wildcards (first argument) for patterns within the base dir
# and get back to relative names.
__wildcard_relative = \
  $(foreach basedir,$(abspath $2) \
    ,$(patsubst $(basedir)/%,%,$(call $1,$(3:%=$(basedir)/%))))

d-wildcard_relative_first = $(call __wildcard_relative,d-wildcard_first,$1,$2)
f-wildcard_relative_first = $(call __wildcard_relative,f-wildcard_first,$1,$2)
  wildcard_relative_first = $(call __wildcard_relative,  wildcard_first,$1,$2)

endif # __util_wildcard_mk
