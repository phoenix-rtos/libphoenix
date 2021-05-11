# Makefile rules for compiling and linking binary file
# supported external variables:
# - NAME - component/target binary name
# - LOCAL_SRCS - list of source files relative to current makefile
# - SRCS       - list of source files relative to project root
# - LOCAL_HEADERS - headers to be installed (relative to current makefile)
# - HEADERS       - headers to be installed (relative to project root)
# - DEP_LIBS - static libraries from current repo needed to be compiled/installed before this component (shortcut for putting something in LIBS and DEPS)
# - DEPS - list of components from current repo to be completed before starting this one
# - LIBS - names of the static libs to link the binary against (without .a suffix)
# - LOCAL_CFLAGS  - additional CFLAGS for current component compilation
# - LOCAL_LDFLAGS - additional LDFLAGS for current component linking

### FIXME: should be available in phoenix-rtos-build for usage in all projects

# directory with current Makefile - relative to the repository root
CLIENT_MAKES := $(filter-out $(binary.mk) $(static-lib.mk) %.c.d,$(MAKEFILE_LIST))
LOCAL_DIR := $(dir $(lastword $(CLIENT_MAKES)))

SRCS += $(addprefix $(LOCAL_DIR), $(LOCAL_SRCS))
HEADERS += $(addprefix $(LOCAL_DIR), $(LOCAL_HEADERS))

# linking prerequisites
OBJS.$(NAME) := $(patsubst %.c,$(PREFIX_O)%.o,$(SRCS))
RESOLVED_LIBS := $(patsubst %,$(PREFIX_A)%.a, $(DEP_LIBS))
RESOLVED_LIBS += $(patsubst %,$(PREFIX_A)%.a, $(LIBS))

# compilation prerequisites - component order-only dependency
DEPS += $(DEP_LIBS)
$(OBJS.$(NAME)): | $(DEPS)

# potentially custom CFLAGS/LDFLAGS for compilation and linking
$(OBJS.$(NAME)): CFLAGS:=$(CFLAGS) $(LOCAL_CFLAGS)
$(PREFIX_PROG)$(NAME): LDFLAGS:=$(LDFLAGS) $(LOCAL_LDFLAGS)

# dynamically generated dependencies (file-to-file dependencies)
DEPS.$(NAME) := $(patsubst %.c,$(PREFIX_O)%.c.d,$(SRCS))
-include $(DEPS.$(NAME))

# rule for installing headers
INSTALLED_HEADERS.$(NAME) := $(patsubst $(LOCAL_DIR)%.h, $(PREFIX_H)%.h, $(HEADERS))

$(PREFIX_H)%.h: $(LOCAL_DIR)%.h
	$(HEADER)

# rule for linking binary
# NOTE: if applied globally, we could remove the $(LINK) variable and put explicit commands here
$(PREFIX_PROG)$(NAME): $(OBJS.$(NAME)) $(RESOLVED_LIBS) $(PHOENIXLIB)
	$(LINK)

# create component phony targets
.PHONY: $(NAME) $(NAME)-headers $(NAME)-clean $(NAME)-install

$(NAME)-headers: $(INSTALLED_HEADERS.$(NAME))

$(NAME): $(NAME)-headers $(PREFIX_PROG_STRIPPED)$(NAME)

$(NAME)-clean: $(patsubst %,%-clean,$(DEPS))
	@echo "cleaning $(NAME)"
	@rm -rf $(OBJS.$(NAME)) $(DEPS.$(NAME)) $(PREFIX_PROG)$(NAME) $(PREFIX_PROG_STRIPPED)$(NAME) $(INSTALLED_HEADERS.$(NAME))


# necessary for NAME variable to be correctly set in recepies
$(NAME) $(NAME)-clean: NAME:=$(NAME)

# cleaning vars to avoid strange errors
NAME :=
LOCAL_SRCS :=
LOCAL_DIR :=
LOCAL_HEADERS :=
DEP_LIBS :=
DEPS :=
SRCS :=
HEADERS :=
LIBS :=
LOCAL_CFLAGS :=
LOCAL_LDFLAGS :=

# TODO: install into the root filesystem
#$(NAME)-install: $(PREFIX_PROG_STRIPPED)$(NAME)
#	@echo "TODO: install"
