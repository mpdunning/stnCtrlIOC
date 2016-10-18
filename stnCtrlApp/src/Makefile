TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = stnCtrl
DBD += stnCtrl.dbd

stnCtrl_DBD += base.dbd
stnCtrl_DBD += asSupport.dbd
stnCtrl_DBD += stnCtrlSup.dbd

stnCtrl_LIBS += autosave 

stnCtrl_SRCS += stnCtrl_registerRecordDeviceDriver.cpp
stnCtrl_SRCS += stnCtrlPadASub.c
stnCtrl_SRCS += stnCtrlPacASub.c

stnCtrl_SRCS_DEFAULT += main.cpp
stnCtrl_SRCS_vxWorks += -nil-

stnCtrl_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

