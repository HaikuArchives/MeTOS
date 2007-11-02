# makefile pour les Addons

MAKEFILEPATH = ./source/MakeFiles
CPU = x86

ADDONSNAME = \
	ColorsPrefs \
	MeTOSPrefs \
	ToolPrefs

default .DEFAULT :
	-@for f in $(ADDONSNAME) ; \
	do \
		$(MAKE) -C $(MAKEFILEPATH) -f $$f $@; \
	done
	-@for f in $(ADDONSNAME) ; \
	do \
		mv ./$(MAKEFILEPATH)/obj.$(CPU)/$$f ./$$f ;\
	done