
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

EMBOX_TARGET_CC =$(shell which $(EMBOX_CROSS_COMPILE)gcc)
EMBOX_TARGET_CXX=$(shell which $(EMBOX_CROSS_COMPILE)g++)

ifeq (,$(EMBOX_TARGET_CC))
$(error Cannot find C compiler: $(EMBOX_CROSS_COMPILE)gcc)
endif
ifeq (,$(EMBOX_TARGET_CXX))
$(error Cannot find C++ compiler: $(EMBOX_CROSS_COMPILE)g++)
endif

AT=

DLMALLOC_DIR       = $(ROOT_DIR)/build/dlmalloc

EMBOX_IMPORTED_CPPFLAGS += -I$(abspath .)/include
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
#EMBOX_IMPORTED_CPPFLAGS += -include $(abspath .)/busybox_embox_compat.h
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS) 

EMBOX_IMPORTED_CFLAGS   += $(filter -g,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS)))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))

EMBOX_IMPORTED_MAKEFLAGS += $(filter -j,$(EMBOX_MAKEFLAGS))
ifneq ($(filter -j,$(EMBOX_MAKEFLAGS)),)
EMBOX_IMPORTED_MAKEFLAGS += $(shell nproc)
endif
# no wonder the following doesn't work
# EMBOX_IMPORTED_MAKEFLAGS += $(filter --jobserver-fds=%,$(EMBOX_MAKEFLAGS))

EMBOX_DERIVED_CFLAGS    += $(EMBOX_IMPORTED_CFLAGS) 
EMBOX_DERIVED_CXXFLAGS  += $(EMBOX_IMPORTED_CXXFLAGS) 
EMBOX_DERIVED_MAKEFLAGS += $(EMBOX_IMPORTED_MAKEFLAGS) 

EMBOX_DERIVED_LDFLAGS   += -Wl,--relocatable -nostdlib -m32
all:

.PHONY: all

DLMALLOC_SRC := malloc.c
DLMALLOC_LIB := $(DLMALLOC_DIR)/malloc.o
DLMALLOC_CFLAGS := -DHAVE_MMAP=1 -DHAVE_MORECORE=0 -DLACKS_TIME_H

$(DLMALLOC_DIR):
	$(AT)mkdir -p $@

$(DLMALLOC_LIB) : $(DLMALLOC_SRC) $(DLMALLOC_DIR)
	$(EMBOX_TARGET_CC) $(EMBOX_DERIVED_CFLAGS) $(DLMALLOC_CFLAGS) -c -o $@ $<

all: $(DLMALLOC_LIB)	

