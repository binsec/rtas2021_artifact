CXX      =g++
RM       =rm -f

BASE=../../../../

CPPFLAGS =-I$(BASE)
CXXFLAGS =-O0 -g3 -Wall -Werror
LDFLAGS  =-pthread

VPATH=$(BASE)

SRCS     =unisim/util/hypapp/hypapp.cc miniweb.cc

BUILD    =build
OBJS     =$(patsubst %.cc,$(BUILD)/%.o,$(SRCS))
DEPS     =$(patsubst %.o,%.d,$(OBJS))

EXE=miniwebserver

.PHONY: all
all: $(EXE)


$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJS)

$(OBJS):$(BUILD)/%.o:%.cc
	@mkdir -p `dirname $@`
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -o $@ -c $<

clean:
	$(RM) -R $(BUILD)
	$(RM) $(EXE) $(EXE).exe


-include $(DEPS)

