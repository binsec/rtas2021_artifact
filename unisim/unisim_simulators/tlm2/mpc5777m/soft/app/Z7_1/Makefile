CC=$(PWD)/../../libsys/bin/Z7_1/bin/powerpc-eabivle-gcc
LDFLAGS=-static
CFLAGS=-Og -g -Wall -fdebug-prefix-map=$(shell readlink -f $$PWD/../../app)=app -fdebug-prefix-map=$(shell readlink -f $$PWD/../../libsys)=libsys
RM=rm -f

OUTPUT=boot.elf
SOURCES=main.c

OBJS=$(patsubst %.c,%.o,$(SOURCES))
DEPS=$(patsubst %.o,%.d,$(OBJS))

.PHONY: all
all: $(OUTPUT)

.PHONY: clean
clean:
	$(RM) $(OUTPUT) $(OBJS) $(DEPS)

$(OUTPUT): $(OBJS) $(PWD)/../../libsys/lib/libsys.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

-include $(DEPS)
