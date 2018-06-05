CString WinInetErrCodToMessage(unsigned long errCode);
__declspec(dllexport) int  DesencriptarDatos(CString sSemilla, CString &sUsuario, CString &sPassWord);
__declspec(dllexport) bool ObtenerUserPass(CString sPathFile, CString &sSemilla, CString &sUsuario, CString &sPassWord);
__declspec(dllexport) bool Recupero_UserID_Psw(CString Ambiente, CString &sUsuario, CString &sPassWord);		
__declspec(dllexport) bool RevisarRespuesta(CString sRespuesta);
__declspec(dllexport) void CompletaEspacios(CString &CampoData, int MaxLen);	
__declspec(dllexport) void MinToMay(char string[]);	
__declspec(dllexport) CString Consultar(CString sAlias, CString sCBU, CString sAmbiente);		
__declspec(dllexport) CString ObtenerError(CString sErr);
typedef  (CALLBACK* LPFNDLLFUNC)(char *, char *,char *);

