# makefile pour les Addons

MAKEFILEPATH = ./source/MakeFiles/
CPU = x86

ADDONSNAME = \
	VApplication \
	VBox \
	VButton \
	VChannelSlider \
	VCheckBox \
	VColorControl \
	VGButton \
	VListView \
	VMenu \
	VMenuBar \
	VMenuField \
	VMenuItem \
	VOutlineListView \
	VPopUpMenu \
	VProject \
	VRadioButton \
	VScrollView \
	VSlider \
	VStatusBar \
	VStringView \
	VTabView \
	VTextControl \
	VTextView \
	VView \
	VWindow

default .DEFAULT :
	-@for f in $(ADDONSNAME) ; \
	do \
		$(MAKE) -C $(MAKEFILEPATH) -f $$f $@; \
	done
	-@for f in $(ADDONSNAME) ; \
	do \
		mv ./$(MAKEFILEPATH)/obj.$(CPU)/$$f ./$$f ;\
	done