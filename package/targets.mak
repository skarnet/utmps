BIN_TARGETS := \
utmps-utmpd \
utmps-wtmpd \
utmps-write

LIBEXEC_TARGETS :=

LIB_DEFS := UTMPS=utmps
UTMPS_DESCRIPTION := The utmps library, a secure utmp implementation

ifneq ($(DO_LIBC_INCLUDES),)
EXTRA_INCLUDES := src/include/utmpx.h
ifneq ($(DO_UTMP),)
EXTRA_INCLUDES += src/include/utmp.h
endif
endif
