TPM tested in SDK 2.0 - OK (S32 did not have TPM driver).
FTM tested in SDK 2.0 - there are errors in SDK 2.0:
  - Function FTM_SetupPwm (fsl_ftm.c) does not set High-true pulses (clear Output on match) channel mode correctly. It sets MSA bit instead of ELSB in the FTMx_CnSC register (wrong shifting). Fixed in new version of SDK.
  - Function FTM_SetupOutputCompare does not set offset (the offset value is written to a buffer, but not propagated into FTM register). When you move setting CnV register after CnSC in the function, everything is OK.
  
Note: Input capture is not implemented in Timer.