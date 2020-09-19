@ECHO OFF

FOR %%L in (Enterprise Professional Community) DO (
  IF EXIST "%VU_VSDEV%" (
    SET VU_TMP="%VU_VSDEV%"
  )
)

SET VU_VSDEV=%VU_TMP:"=%