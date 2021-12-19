BIN_TARGETS := \
utmps-utmpd \
utmps-wtmpd \
utmps-write

LIBEXEC_TARGETS :=

LIB_DEFS := UTMPS=utmps

ifneq ($(DO_LIBC_INCLUDES),)
EXTRA_INCLUDES := src/include/utmpx.h
endif
