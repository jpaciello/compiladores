def traducir(tabla):
    xml = ""
    xml_final = ""
    elemento_de_lista = False
    es_clave = False
    reemplazar = False 
    llave_principal = True

    for elemento in tabla:
        if(llave_principal and elemento["tipoLexema"] == "LITERAL_CADENA"):
            xml_final = "<" + elemento["token"] + ">" + "\n[INTERNO]\n" + "</" + elemento["token"] + ">"
            llave_principal = False
        elif(elemento["token"] == "{"):
            es_clave = True
        elif(elemento["token"] == "["):
            elemento_de_lista = True
        elif(elemento["token"] == "{" and elemento_de_lista):
            xml += "<item>"
        elif(elemento["tipoLexema"] in ["LITERAL_CADENA", "LITERAL_NUM", "PR_TRUE", "PR_FALSE"] and not reemplazar and not llave_principal):
            xml += "<" + elemento["token"] + ">" + "[VALUE]" + "</" + elemento["token"] + ">"
            reemplazar = True
        elif(elemento["tipoLexema"] in ["LITERAL_CADENA", "LITERAL_NUM", "PR_TRUE", "PR_FALSE"] and reemplazar):
            xml = xml.replace("[VALUE]", elemento["token"])
            reemplazar = False
        elif(elemento["token"] == "}" and elemento_de_lista):
            xml += "</item>"
            elemento_de_lista = False
        elif(elemento["token"] == "]"):
            xml += "</item>"
            elemento_de_lista = False
        else:
            pass
    
    return xml_final.replace("[INTERNO]", xml)