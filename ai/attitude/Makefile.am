## Process this file with automake to produce Makefile.in
#FC_HOME = ~/Programs/freeciv-2.4.2/
#aimoduledir=$(libdir)/fcai
aimodule_LTLIBRARIES = fc_ai_attitude.la

#ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(top_srcdir)/utility \
-I$(top_srcdir)/common \
-I$(top_srcdir)/ai \
-I$(top_srcdir)/server \
-I$(top_srcdir)/server/advisors \
-I$(top_srcdir)/ai/default

fc_ai_attitude_la_SOURCES = \
attitudeai.c \
attitudeai.h \
aivariant.h \
aivariant.c \
leaderrules.c \
leaderrules.h \
utils.h \
utils.c

fc_ai_attitude_la_LDFLAGS = -module
fc_ai_attitude_la_LIBADD = -lm

