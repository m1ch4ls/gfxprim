TOPDIR=.
SUBDIRS=libs tests pylib demos
include include.mk

#
# Make sure tests are build after library and 
# rebuild library before entering test just
# to be extra safe.
#
.PHONY: build

tests: build libs

build:
	@$(MAKE) --no-print-directory -C build clean
	@$(MAKE) --no-print-directory -C build

clean:
ifdef VERBOSE
	$(MAKE) -C build clean
else
	@echo "/build"
	@$(MAKE) --no-print-directory -C build clean
endif

HEADER_LOC=/usr/include/
LIB_LOC=/usr/lib/

install:
ifdef VERBOSE
	./install.sh ""
else
	@./install.sh ""
endif

tar:
	$(MAKE) clean
	cd .. && tar cf gfxprim-`date +%Y-%b-%d-%HH%MM`.tar gfxprim
