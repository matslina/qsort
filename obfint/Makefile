
SOURCES=$(wildcard bfint*.c)
OBJS := $(patsubst %.c, %, $(SOURCES))

all: $(OBJS)

$(OBJS): $(SOURCES)
	gcc $(@).c -o $@

clean:
	touch $(OBJS)
	rm $(OBJS)
