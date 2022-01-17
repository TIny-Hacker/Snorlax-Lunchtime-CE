# ----------------------------
# Makefile Options
# ----------------------------

NAME = SNORLAX
ICON = icon.png
DESCRIPTION = "Snorlax's Lunchtime for the CE"
COMPRESSED = YES
ARCHIVED = YES

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)

