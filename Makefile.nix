# makfile configuration
TARGET          = libeui
SOURCES         = \
	examples/sdl2/main.c \
	examples/sdl2/driver_sdl2.c \
	src/eui_button.c \
	src/eui_container.c \
	src/eui_object.c \
	src/eui_string.c \
	src/eui_view.c \
	src/eui_label.c \
	src/eui_touch.c \
	src/eui_image.c \
	src/libeui.c \
	examples/fonts/fontcm16b.c \
	examples/fonts/fontcm30.c \
	examples/fonts/fontfixed6x8.c

MAKE            = make
CC              = gcc
OBJDUMP         = objdump

INCLUDES        = -Isrc
DEFINES         = -DLIBEUI_DRIVER_SDL2 -D_REENTRANT

CFLAGS          = $(INCLUDES) $(DEFINES) -O0 -g -std=c99 -Wall -Wno-unused-variable -Wno-unused-function
LDFLAGS         = -lSDL2

BUILDDIR        = ./.build.nix


# define object files from sources
OBJECTS = $(SOURCES:%.c=$(BUILDDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo
	@echo "Linking $@..."
	@$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete."
	@echo

-include $(OBJECTS:.o=.d)

$(BUILDDIR)/%.o : %.c
	@echo "Compiling $<..."
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET)
