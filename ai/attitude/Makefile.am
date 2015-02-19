## Process this file with automake to produce Makefile.in

aimodule_LTLIBRARIES = fc_ai_attitude.la

AM_CPPFLAGS = \
-I$(FCBLDPATH)/gen_headers/ \
-I$(FCSRCPATH)/utility \
-I$(FCSRCPATH)/common \
-I$(FCSRCPATH)/ai \
-I$(FCSRCPATH)/server \
-I$(FCSRCPATH)/server/advisors \
-I$(FCSRCPATH)/ai/default

fc_ai_attitude_la_SOURCES = \
utils.h \
utils.c \
leaderrules.c \
leaderrules.h \
aivariant.h \
aivariant.c \
attitudeai.c \
attitudeai.h

fc_ai_attitude_la_LDFLAGS = -module
fc_ai_attitude_la_LIBADD = -lm
