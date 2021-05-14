# Makefile rules for compiling and static library
# supported external variables:
# - NAME - component/target binary name
# - LOCAL_SRCS - list of source files relative to current makefile
# - SRCS       - list of source files relative to project root
# - LOCAL_HEADERS - headers to be installed (relative to current makefile)
# - HEADERS       - headers to be installed (relative to project root)
#
# - DEPS - list of components from current repo to be completed before starting this one
#
# - LOCAL_CFLAGS  - additional CFLAGS for current component compilation
#



# directory with current Makefile - relative to the repository root
# filter-out all Makefiles outside of TOPDIR
CLIENT_MAKES := $(filter $(TOPDIR)/%,$(abspath $(MAKEFILE_LIST)))
LOCAL_DIR := $(patsubst $(TOPDIR)/%,%,$(dir $(lastword $(CLIENT_MAKES))))

SRCS += $(addprefix $(LOCAL_DIR), $(LOCAL_SRCS))
HEADERS += $(addprefix $(LOCAL_DIR), $(LOCAL_HEADERS))

# linking prerequisites
OBJS.$(NAME) := $(patsubst %.c,$(PREFIX_O)%.o,$(SRCS))

# compilation prerequisites - component order-only dependency
$(OBJS.$(NAME)): | $(DEPS)

# potentially custom CFLAGS/LDFLAGS for compilation and linking
$(OBJS.$(NAME)): CFLAGS:=$(CFLAGS) $(LOCAL_CFLAGS)

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

# allow header-only components
LIBNAME :=
ifneq ($(strip $(SRCS)),)
  LIBNAME := $(PREFIX_A)$(NAME).a
endif

# rule for linking static lib
# NOTE: if applied globally, we could remove the $(ARCH) variable and put explicit commands here
$(PREFIX_A)$(NAME).a: $(OBJS.$(NAME))
	$(ARCH)

# create component phony targets
.PHONY: $(NAME) $(NAME)-headers $(NAME)-clean

$(NAME)-headers: $(INSTALLED_HEADERS.$(NAME))

$(NAME): $(NAME)-headers $(LIBNAME)

$(NAME)-clean:
	@echo "cleaning $(NAME)"
	@rm -rf $(OBJS.$(NAME)) $(DEPS.$(NAME)) $(INSTALLED_HEADERS.$(NAME)) $(PREFIX_A)$(NAME).a

# no installation for static libs
$(NAME)-install: $(NAME) ;


# necessary for NAME variable to be correctly set in recepies
$(NAME) $(NAME)-clean: NAME:=$(NAME)

# cleaning vars to avoid strange errors
NAME :=
LOCAL_SRCS :=
LOCAL_DIR :=
LOCAL_HEADERS :=
DEPS :=
SRCS :=
HEADERS :=
LOCAL_CFLAGS :=
