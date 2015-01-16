##
# Make documentation: http://www.gnu.org/software/make/manual/
#



##
# Global variables section.
#
BUG_ADDRESS = kurijlj@gmail.com
TARGETS = cqlock



##
# Define global flags here.
#
STD_FLAGS = -O0 -Wall -std=gnu99
STD_DBG_FLAGS = -g -O0 -Wall -std=gnu99
GLIB2CFLAGS = $(shell pkg-config --cflags glib-2.0)



##
# Define global libraries here.
#
LIBGLIB2 = $(shell pkg-config --libs glib-2.0)



##
# Print some help information.
#
.ONESHELL:
help:
	@echo -e "Usage: LANG=<LANGUAGE> RELEASE=<true> make [option]\n"
	@echo -e "Supported options:"
	@echo -e "\thelp\t\t\tPrint this help list"
	@echo -e "\tall\t\t\tMake all executables"
	@echo -e "\tprepmsgs\t\tPrepare .po files for translation"
	@echo -e "\ttranslations\t\tMake .mo translations"
	@echo -e "\tclean\t\t\tRemove .o and *~ files"
	@echo -e "\tcleanall\t\tRemove all executables, .o, *~, .ppc"
	@echo -e "\t\t\t\tand .pot files"
	@echo -e "\tcqlock\t\t\tMake cqlock executable"
	@echo -e "\tcqlock.po\t\tPrepare cqlock.po file for translation"
	@echo -e "\tcqlock.mo\t\tMake cqlock.mo translation\n"
	@echo -e "Report bugs and suggestions to <$(BUG_ADDRESS)>\n"



##
# Make all executables.
#
all: $(TARGETS)



##
# Prepare all messages for translation.
#
prepmsgs: $(foreach TARGET,$(TARGETS),$(TARGET).po)



##
# Prepare all messages for translation.
#
translations: $(foreach TARGET,$(TARGETS),$(TARGET).mo)



##
# cqlock section.
#
cqlock : SOURCES = cqlock.c
cqlock : TARGET = $(SOURCES:%.c=%)
cqlock : OBJS = $(TARGET).o
cqlock : LIBS = $(LIBGLIB2)
cqlock : EXTRA_FLAGS = $(GLIB2CFLAGS)
cqlock : cqlock.o
	gcc -o $(TARGET) $(OBJS) $(LIBS)

cqlock.o : cqlock.c
ifeq ($(RELEASE), true)
	gcc $(STD_FLAGS) $(EXTRA_FLAGS) -c $(SOURCES)
else
	gcc $(STD_DBG_FLAGS) $(EXTRA_FLAGS) -c $(SOURCES)
endif

# cqlock localisation section.
cqlock.mo : SOURCE = cqlock.po
cqlock.mo : TARGET = $(SOURCE:%.po=%).mo
.ONESHELL:
cqlock.mo : cqlock.po
ifeq ($(strip $(LANG)),)
	mkdir -p ./$(LANG)/LC_MESSAGES
	msgfmt -c -v -o $(TARGET) $(SOURCE)
	mv -v ./$(TARGET) ./$(LANG)/LC_MESSAGES/
else
	mkdir -p ./sr_RS@latin/LC_MESSAGES
	msgfmt -c -v -o $(TARGET) $(SOURCE)
	mv -v ./$(TARGET) ./sr_RS@latin/LC_MESSAGES/
endif

cqlock.po : SOURCE = cqlock.pot
cqlock.po : TARGET = $(SOURCE:%.pot=%).po
cqlock.po : cqlock.pot
ifeq ($(strip $(LANG)),)
	msginit -l $(LANG) -o $(TARGET) -i $(SOURCE)
else
	msginit -l sr_RS@latin -o $(TARGET) -i $(SOURCE)
endif

cqlock.pot : SOURCE = cqlock.ppc
cqlock.pot : DOMAIN = $(SOURCE:%.ppc=%)
cqlock.pot : VERSION = 1.0
cqlock.pot : TARGET = $(DOMAIN).pot
cqlock.pot : cqlock.ppc
	xgettext -o $(TARGET) --package-name="$(DOMAIN)" \
		--package-version="$(VERSION)" \
		--msgid-bugs-address="$(BUG_ADDRESS)" $(SOURCE)

cqlock.ppc : SOURCE = cqlock.c
cqlock.ppc : TARGET = $(SOURCE:%.c=%).ppc
cqlock.ppc : EXTRA_FLAGS = $(GLIB2CFLAGS)
cqlock.ppc : cqlock.c
	cpp $(EXTRA_FLAGS) $(SOURCE) $(TARGET)



##
# Cleanup section.
#
clean :
	rm -v ./*.o ./.*~ ./*~

cleanall :
	rm -v $(foreach TARGET,$(TARGETS),./$(TARGET)) ./*.o ./.*~ ./*~ ./*.ppc ./*.pot
