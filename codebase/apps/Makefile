#
# Recursive make - makes the subdirectory code
#

include $(RAP_MAKE_INC_DIR)/rap_make_macros

TARGETS =  $(GENERAL_TARGETS) $(DOC_TARGETS) $(INSTALL_TARGETS)

SUB_DIRS = \
	tdrp \
	procmap \
	spdb_utils \
	mdv_utils

$(TARGETS): FORCE
	$(MAKE) $(SUB_DIRS) TARGET=$@

$(SUB_DIRS): FORCE
	cd $@; $(MAKE) $(TARGET)

FORCE:







