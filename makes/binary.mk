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
# - LOCAL_INSTALL_PATH - custom rootfs dir for the binary to be installed (if not provided - DEFAULT_INSTALL_PATH)


# directory with current Makefile - relative to the repository root
# filter-out all Makefiles outside of TOPDIR
CLIENT_MAKES := $(filter $(TOPDIR)/%,$(abspath $(MAKEFILE_LIST)))
LOCAL_DIR := $(patsubst $(TOPDIR)/%,%,$(dir $(lastword $(CLIENT_MAKES))))

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
ifneq ($(filter-out $(PREFIX_H)%, $(INSTALLED_HEADERS.$(NAME))),)
  $(error $(NAME): Installing headers outside of PREFIX_H, check Your makefile: $(INSTALLED_HEADERS.$(NAME)))
endif

$(PREFIX_H)%.h: $(LOCAL_DIR)%.h
	$(HEADER)

# rule for linking binary
# NOTE: if applied globally, we could remove the $(LINK) variable and put explicit commands here
# NOTE: disabled $(PHOENIXLIB) direct dependency until it can be removed from $(LIBS)
$(PREFIX_PROG)$(NAME): $(OBJS.$(NAME)) $(RESOLVED_LIBS) # $(PHOENIXLIB)
	$(LINK)

# create component phony targets
.PHONY: $(NAME) $(NAME)-headers $(NAME)-clean $(NAME)-install

$(NAME)-headers: $(INSTALLED_HEADERS.$(NAME))

$(NAME): $(NAME)-headers $(PREFIX_PROG_STRIPPED)$(NAME)

$(NAME)-clean: $(patsubst %,%-clean,$(DEPS))
	@echo "cleaning $(NAME)"
	@rm -rf $(OBJS.$(NAME)) $(DEPS.$(NAME)) $(PREFIX_PROG)$(NAME) $(PREFIX_PROG_STRIPPED)$(NAME) $(INSTALLED_HEADERS.$(NAME))

# install into the root filesystem
LOCAL_INSTALL_PATH := $(or $(LOCAL_INSTALL_PATH),$(DEFAULT_INSTALL_PATH))

$(NAME)-install: $(NAME) $(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH)/$(NAME)
$(PREFIX_ROOTFS)$(LOCAL_INSTALL_PATH)/$(NAME): $(PREFIX_PROG_STRIPPED)$(NAME)
	$(INSTALL_FS)

# necessary for NAME variable to be correctly set in recepies
$(NAME) $(NAME)-clean: NAME:=$(NAME)

ALL_COMPONENTS += $(NAME)

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
LOCAL_INSTALL_PATH :=
