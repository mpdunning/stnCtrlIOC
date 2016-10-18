#!../../bin/linux-x86_64/stnCtrl

< envPaths

cd ${TOP}

dbLoadDatabase "dbd/stnCtrl.dbd"
stnCtrl_registerRecordDeviceDriver pdbbase

epicsEnvSet("LOC", "ESB")
epicsEnvSet("N",   "3")
epicsEnvSet("P",   "$(LOC):STN$(N)")
epicsEnvSet("STN", "$(LOC):XBSTN$(N)")
epicsEnvSet("KLY", "K5")
epicsEnvSet("PAD", "0$(N)1")

save_restoreSet_status_prefix( "")
save_restoreSet_IncompleteSetsOk( 1)
save_restoreSet_DatedBackupFiles( 1)
set_savefile_path( "/nfs/slac/g/testfac/esb/$(IOC)","autosave")
set_pass0_restoreFile( "stnCtrl.sav")
set_pass1_restoreFile( "stnCtrl.sav")

dbLoadRecords("db/stnCtrl.db","P=$(P),STN=$(STN),KLY=$(KLY),PAD=$(PAD),IOC=$(IOC)")

#cd ${AUTOSAVE}
#dbLoadRecords( "db/save_restoreStatus.db","P=$(P)")

cd ${TOP}/iocBoot/${IOC}
iocInit

create_monitor_set( "stnCtrl.req",30,"P=$(P)")

epicsThreadSleep(2)
dbpf $(P):PULSEMODE.PROC 1
