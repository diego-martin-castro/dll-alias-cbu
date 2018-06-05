##############################################################

INSTRUCCIONES DE USO:

1- En Project Settings del proyecto donde se lo vaya a incluir -> ir a la solapa: Link ->Object/Library modules: -> Agregar la librería qtaliascbu.lib

2- En el .h de la clase donde se utilice la DLL incluir el .h QTAliasCBU.H

FUNCIONES DISPONIBLES:

Llamar las funciones disponibles dentro de la librería

Consultar(CString sAlias, CString sCBU, CString Ambiente);
Función principal que devuelve un CString con el resultado, si hay un error devuelve un CString vacío;

Parametros necesarios: sAlias, sCBU y sAmbiente. Dejar "" el dato que se quiera obtener.

Si se pasa un Alias:

sAlias = "#####";

sCBU = "";

sAmbiente = "DESA";

Si se pasa un CBU:

sAlias = "";

sCBU = "######";

sAmbiente = "DESA";

RevisarRespuesta(CString sRespuesta)
Función para comprobar si la consulta fue correcta o hubo algún error. TRUE si es correcta o FALSE si hubo un error.

ObtenerError(CString sErr)
Función para obtener un string con la descripcion del error en base a un codigo de error

##############################################################