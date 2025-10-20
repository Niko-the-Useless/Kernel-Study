savedcmd_sysUptime.mod := printf '%s\n'   sysUptime.o | awk '!x[$$0]++ { print("./"$$0) }' > sysUptime.mod
