# $Id$
#
# Embox root Makefile.
#
# This file have just to setup proper make flags and to invoke mk/main.mk
# which will perform the real work.
#
# Author: Eldar Abusalimov
#

#
# Directories.
#

export ROOT_DIR       := .

export CONF_DIR       := $(ROOT_DIR)/conf
export TEMPLATES_DIR  := $(ROOT_DIR)/templates

export SRC_DIR        := $(ROOT_DIR)/src
export DOC_DIR        := $(ROOT_DIR)/doc

export BUILD_DIR      := $(ROOT_DIR)/build

export BIN_DIR        := $(BUILD_DIR)/bin
export OBJ_DIR        := $(BUILD_DIR)/obj
export LIB_DIR        := $(BUILD_DIR)/lib
export SRCGEN_DIR     := $(BUILD_DIR)/src-gen
export MKGEN_DIR      := $(BUILD_DIR)/mk-gen
export AUTOCONF_DIR   := $(BUILD_DIR)/conf-gen
export DOT_DIR        := $(DOC_DIR)
export DOCS_OUT_DIR   := $(DOC_DIR)

export CACHE_DIR      := mk/.cache

export ANNOTATION_HANDLERS := mk/mybuild/annotation_handlers

#
# Tools.
#

export RM     := rm -f
export CP     := cp
export MV     := mv
export PRINTF := printf
export MKDIR  := mkdir -p
export LN     := ln -s

# Check Make version (we need at least GNU Make 3.81). Fortunately,
# '.FEATURES' built-in variable has been introduced exactly in GNU Make 3.81.
ifneq ($(origin .FEATURES),default)
$(error Unsupported Make version. \
	Mybuild does not work properly with GNU Make $(MAKE_VERSION), \
	please use GNU Make 3.81 or above.)
endif

# Disable everything, turn on undefined variables check.
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables
MAKEFLAGS += --no-print-directory
MAKEFLAGS += --warn-undefined-variables

# Fixup for case when prompt contains dollar signs.
# Avoids bogus 'undefined variable' warnings.
export PS1 :=

.DEFAULT_GOAL := all

# Force multiple targets listed in the command line to run independently,
# even if -j option is enabled. This allows 'make -j clean all' to run
# properly, at the same time executing each target in parallel.
.NOTPARALLEL :

.PHONY : $(sort all $(MAKECMDGOALS))
$(sort all $(MAKECMDGOALS)) :
	@$(MAKE) -C $(dir $(lastword $(MAKEFILE_LIST))) -f mk/main.mk $@

