#include "stdafx.h"
#include "QTAliasCBU.h"
#include "QTDIPCall.h"
#include "QTClass.h"
#include "QTApplut.h"

#define EXPORTING_DLL

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#define TBL_PATH		"QTPATHAPLICATIVOS"


/////////////////////////////////////////////////////////////////////////////
// AliasCBU operations
/////////////////////////////////////////////////////////////////////////////

//Obtiene el usuario y password para enviarlos en la cabecera del XML para la consulta del CBU
bool Recupero_UserID_Psw(CString Ambiente, CString &sUsuario, CString &sPassWord)
{	
	CString sSemilla  = "";
	CString	sServer   = "";
	CString sPath     = "";
	CString sPathField = "";
	
	sServer="MD" + CString(Ambiente);
	QTGetCTEntry(TBL_PATH, QT_COMBO_GET_DECODE, sServer, sPath );
	sPath.TrimRight();
	sPathField = sPath+"\\MIDLWPASS.PWD";
	
	ObtenerUserPass(sPathField, sSemilla, sUsuario, sPassWord);
	CompletaEspacios(sSemilla,16);
	DesencriptarDatos(sSemilla, sUsuario,  sPassWord);

	return(sUsuario!="" && sPassWord!="");
}

//Obtiene el usuario y password
bool ObtenerUserPass(CString sPathFile, CString &sSemilla, CString &sUsuario, CString &sPassWord)
{	
	FILE *OpenArchivo;
	char buffer[600] = "";
	bool bReturn = false;
	int  lenSemilla = 0;

	if( (OpenArchivo = fopen( sPathFile, "r" )) != NULL )
	{
		if (fgets(buffer, 600, OpenArchivo) != NULL)  
						sSemilla=buffer;
		if (fgets(buffer, 600, OpenArchivo) != NULL)  
							sUsuario=buffer;
		if (fgets(buffer, 600, OpenArchivo) != NULL)  
							sPassWord=buffer;		
		
		sSemilla = sSemilla.Mid(12,16);
		sSemilla.TrimRight();
		lenSemilla = sSemilla.GetLength();

		if (sSemilla.Mid(lenSemilla-1,1) == "\"")
			sSemilla=sSemilla.Mid(0,lenSemilla-1);

		sUsuario = sUsuario.Mid(12,13);	
		sPassWord = sPassWord.Mid(12,8);

		bReturn = true;
	}
	return bReturn;
}

//Necesario para la funciones de desencriptar el usuario y password
void CompletaEspacios(CString &CampoData, int MaxLen)
{
	MaxLen = MaxLen - (CampoData.GetLength());
	for (int iCont=0;iCont<MaxLen;iCont++)
		CampoData=CampoData+" ";
}

//Funcion para desencriptar los datos del usuario y password que devuelve MIDD
int DesencriptarDatos(CString sSemilla, CString &sUsuario, CString &sPassWord)
{	
	HINSTANCE hDLL;              
	LPFNDLLFUNC lpfnDllFunc;    

	char *pUser="";
	char *pPass="";
	char *pSemi="";
	char *pData="";
	int iRet=0;

    pData = (char *) malloc (100 +1);
	pUser = (char *) malloc (100 +1);
	pPass = (char *) malloc (100 +1);
	pSemi = (char *) malloc (100 +1);

	memset (pData,0x0,100 +1 );
	memset (pUser,0x0,100 +1 );
	memset (pPass,0x0,100 +1 );
	memset (pSemi,0x0,100 +1 );
	
	hDLL = NULL;
	hDLL = LoadLibrary("encpass.dll");

	if (hDLL != NULL)
	{
		 lpfnDllFunc = (LPFNDLLFUNC)GetProcAddress(hDLL,"desencriptar");
		
		 if (!lpfnDllFunc)
		 	iRet=-999;		 
		 else
		 {
			 //strcpy(pUser, sUsuario );
			 strcpy(pPass, sPassWord);
			 strcpy(pSemi, sSemilla);
			 //lpfnDllFunc(pUser,pData,pSemi);
			 //sUsuario = pData;
			 lpfnDllFunc(pPass,pData,pSemi);
			 sPassWord = pData;
		 }

		FreeLibrary(hDLL);
	}		 
	else	
		iRet=-5001; //no se encuentra la .dll en el servidor
	
	return iRet;
}

//Funcion para pasar de minusculas a MAYUSCULAS
void MinToMay(char string[])
{
	int i = 0;
	int desp = 'a'-'A';
	for (i=0;string[i]!='\0';++i)
	{
		if(string[i] >= 'a'&&string[i]<='z')
		{
			string[i] = string[i]-desp;
		}
	}
}

//Funcion PRINCIPAL para consultar el Alias o el CBU dependiendo el caso de consulta
CString Consultar(CString sAlias, CString sCBU, CString sAmbiente)
{   	
	CString sUsuario = "";
	CString sPassWord = "";

	//Controlo la definicion del ambiente
	if(sAmbiente == "")
		//No se definio el ambiente
		return "Err9";  

	//No continuo con la consulta si tengo el Alias y el CBU
	if(sAlias != "" && sCBU != "")
		//Ya estan todos los datos o estan por error
		return "Err8";

	//No continuo con la consulta si no uno de los 2 datos (Alias o CBU) 
	if(sAlias == "" && sCBU == "")
		//No tengo dato necesario
		return "Err8";

	//Estructuras
	_SEND_COE	cSend;
	_RETURN_COE	cReturn;
	
	//Variables
	int ind=0;
	int	iRet=0;
	int indice = 0;
	int longitudRes = 0;
	int longitudUrl = 0; 
	int ResultadoOK = 0;
	int EncontroCBU = 0;
	int EncontroAlias = 0;	
	
	CString sDominio = "CO" + sAmbiente;;
	CString sUrl = "";
	CString sRespuesta = "";
	CString sBuscar = "";
	CString sCanal = "tusucursal";
	CString	port = "";
	CString ssl = "";
	CString dataXML = "";
	CString criterioXML = "";
	CString url = "";	

	//Recupera el user y pass de MIDD
	//Paso por parametro el ambiente
	Recupero_UserID_Psw(sAmbiente, sUsuario, sPassWord);
		
	//dataXML = paso el contenido XML necesario para realizar la consulta
	
	//Segun el parametro enviado el criterio va a hacer...
	if(sAlias != "")
		criterioXML = "<alias>"+sAlias+"</alias>";
	else if(sCBU != "")
		criterioXML = "<cbu>"+sCBU+"</cbu>";

	dataXML="<bsvreq><header><canal>"+sCanal+"</canal><usuario>"+sUsuario+"</usuario><password>"+sPassWord+"</password><requerimiento>alias_cbu_cons</requerimiento></header><data>"+criterioXML+"</data></bsvreq>";
	
	//Obtengo de la tabla la url donde consultas
	QTGetCTEntry(TBL_PATH, QT_COMBO_GET_DECODE, sDominio, sUrl);
	sUrl.TrimRight();
	
	longitudUrl = sUrl.GetLength();
	longitudUrl = longitudUrl - 6;

	url = sUrl.Mid(0, longitudUrl);

	longitudUrl =  sUrl.GetLength();
	url	= url.Mid(7, longitudUrl);

	//Obtengo el puerto 
	port = sUrl.Right(5);	

	char respuesta[10000];
	char cbu[100]; 
	char alias[100];

	memset(&respuesta,	0x00,	sizeof(respuesta));
	memset(&cbu,		0x00,	sizeof(cbu));
	memset(&alias,		0x00,	sizeof(alias));
	memset(&cSend,		0x00,	sizeof(cSend));
	memset(&cReturn,	0x00,	sizeof(cReturn));

	//Parametrizacion del Servicio
	strcpy(cSend.cMethod,"POST");
	strcpy(cSend.cURL, url); 
	strcpy(cSend.cFrmData,dataXML);
	strcpy(cSend.cObjectNameURI, ""); 
	strcpy(cSend.cHeaders,"Content-Type: text/xml; encoding=utf-8");
	strcpy(cSend.cIsSSL,ssl);    
	cSend.PortNumber=atoi(port); 
	cSend.cFrmDataLength=dataXML.GetLength();
	
	//Hago el POST HTTP con el XML
	iRet= MakeHTTPRequestCOE(&cSend,&cReturn);
	
	//Proceso la respuesta
	strcpy(respuesta, cReturn.Respuesta);
	MinToMay(respuesta);	
	sRespuesta = respuesta;	
	longitudRes = sRespuesta.GetLength(); 
	
	//Compruebo el resultado 
	sBuscar = "OK";
	ResultadoOK = sRespuesta.Find(sBuscar);
	
	if(ResultadoOK != -1)
	{
		if(sAlias != "")
		{
			//Posibles resultados al buscar
			sBuscar = "ALIAS MAL FORMULADO";
			EncontroAlias = sRespuesta.Find(sBuscar);	
			
			if(EncontroAlias != -1)
				return "Err1";
			
			sBuscar = "ALIAS NO EXISTE";
			EncontroAlias = sRespuesta.Find(sBuscar);

			if(EncontroAlias != -1)
				return "Err2";			

			sBuscar = "EL ALIAS SE ENCUENTRA ELIMINADO";
			EncontroAlias = sRespuesta.Find(sBuscar);

			if(EncontroAlias != -1)
				return "Err2";				

			//Compruebo si encuentra el ALIAS
			sBuscar = "ALIAS ENCONTRADO";
			EncontroAlias = sRespuesta.Find(sBuscar);

			//Si lo encuentra
			if(EncontroAlias != -1)
			{
				//Busca el CBU en la respuesta
				sBuscar = "CUENTA_CBU";
				EncontroCBU = sRespuesta.Find(sBuscar) + 11;

				//Si lo encuentra
				if(EncontroCBU != -1)
				{
					//Armo el CBU
					for(int i=EncontroCBU; i<=longitudRes; i++)
					{
						if(respuesta[i] != '<')	
						{
							cbu[indice] = respuesta[i];				
							indice++;
						}
						else
							break;
					}			
					//Devuelve el CBU
					return cbu;
				}
				else
					//NO encontro el CBU
					return "Err3";								
			}
		}

		if(sCBU != "")
		{
			//No encuentra el CBU o es erroneo
			sBuscar = "CBU INCORRECTO";
			EncontroCBU = sRespuesta.Find(sBuscar);

			if(EncontroCBU != -1)
				return "Err4";

			sBuscar = "NO TIENE ALIAS ASIGNADO";
			EncontroAlias = sRespuesta.Find(sBuscar);

			if(EncontroAlias != -1)
				return "Err2";

			sBuscar = "<ALIAS_VALOR>NULL</ALIAS_VALOR>";
			EncontroAlias = sRespuesta.Find(sBuscar);

			if(EncontroAlias != -1)
				return "Err6";

			//Busca el Alias en la respuesta
			sBuscar = "ALIAS_VALOR_ORIGINAL";
			EncontroAlias = sRespuesta.Find(sBuscar) + 21;

			//Si lo encuentra
			if(EncontroAlias != -1)
			{
				//Armo el Alias
				for(int i=EncontroAlias; i<=longitudRes; i++)
				{
					if(respuesta[i] != '<')	
					{
						alias[indice] = respuesta[i];				
						indice++;
					}
					else
						break;
				}			
				//Devuelve el ALIAS
				return alias;
			}
			else
				//NO encontro el ALIAS
				return "Err5";			
		} 
		
		//NO encontro el Alias o el CBU
		return "Err6";
	} 
	else
		//Resultado incorrecto - ¿el servicio esta funcionando?
		return "Err7";
}

//Funcion para saber si la respuesta es correcta 
bool RevisarRespuesta(CString sRespuesta)
{
	int longitudResp = sRespuesta.GetLength();
		
	if(longitudResp <= 4 && sRespuesta.Find("Err") != -1)
		return false;
	
	return true;
}

//Funcion para obtener un string con la descripcion del error en base a un codigo de error
CString ObtenerError(CString sErr)
{
	CString error = "";

	if(sErr == "Err1")
		error = "ALIAS INCORRECTO";
	else if(sErr == "Err2")
		error = "ALIAS INEXISTENTE";
	else if(sErr == "Err3")
		error = "CBU NO ENCONTRADO";
	else if(sErr == "Err4")
		error = "CBU INCORRECTO";
	else if(sErr == "Err5")
		error = "ALIAS NO ENCONTRADO";
	else if(sErr == "Err6")
		error = "RESULTADO NO ENCONTRADO";
	else if(sErr == "Err7")
		error = "VERIFICAR EL ESTADO DEL SERVICIO";
	else if(sErr == "Err8")
		error = "DATOS INCORRECTOS";
	else if(sErr == "Err9")
		error = "AMBIENTE INCORRECTO";
	else 
		error = "ERROR DESCONOCIDO";

	return error;
}