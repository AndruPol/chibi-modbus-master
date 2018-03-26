MODBUS     = FreeModbus/modbus
MODBUSPORT = FreeModbus/port

MODBUSPORTSRC = ${MODBUSPORT}/portother.c \
		${MODBUSPORT}/porttimer.c \
		${MODBUSPORT}/porttimer_m.c \
		${MODBUSPORT}/portserial.c \
		${MODBUSPORT}/portserial_m.c \
		${MODBUSPORT}/portevent.c \
		${MODBUSPORT}/portevent_m.c

#		${MODBUSPORT}/user_mb_app.c \	

MODBUSSRC = ${MODBUS}/mb.c \
		${MODBUS}/mb_m.c \
		${MODBUSPORT}/user_mb_app_m.c \
		${MODBUS}/functions/mbfunccoils.c \
		${MODBUS}/functions/mbfuncdiag.c \
		${MODBUS}/functions/mbfunccoils_m.c \
		${MODBUS}/functions/mbfuncdisc.c \
		${MODBUS}/functions/mbfuncdisc_m.c \
		${MODBUS}/functions/mbfuncholding.c \
		${MODBUS}/functions/mbfuncholding_m.c \
		${MODBUS}/functions/mbfuncinput.c \
		${MODBUS}/functions/mbfuncinput_m.c \
		${MODBUS}/functions/mbfuncother.c \
		${MODBUS}/functions/mbutils.c \
		${MODBUS}/rtu/mbrtu.c \
		${MODBUS}/rtu/mbrtu_m.c \
		${MODBUS}/rtu/mbcrc.c \
		${MODBUS}/ascii/mbascii.c \
		${MODBUS}/tcp/mbtcp.c \
		${MODBUSPORTSRC}

MODBUSPORTINC = ${MODBUSPORT}

MODBUSINC =	${MODBUS}/include \
		${MODBUSPORTINC}