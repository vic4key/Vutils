@ECHO OFF

FOR %%L in (Enterprise Professional Community) DO (
	IF EXIST "%VU_VSDEV%" (
		SET VU_TMP="%VU_VSDEV%"
		GOTO L_FOUND
	)
)

IF NOT EXIST "%VU_VSDEV%" (
	FOR %%L in (Unknown) DO (
		SET VU_TMP="%VU_VSDEV%"
	)
)

:L_FOUND

SET VU_VSDEV=%VU_TMP:"=%