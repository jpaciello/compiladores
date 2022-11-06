from modules.lector import Archivo
from modules.regex import Regex
from modules.syntax import analizador
from modules.traductor import traducir

if(__name__ == '__main__'):
    try:
        lexer = Regex()
        archivo = Archivo()
        input = archivo.leer()
        lexema = lexer.getLexema(input)
        archivo.escribir(lexema)
        print("Archivo output.txt generado.")
        traducido = traducir(lexer.tablaTraduccion)
        archivoTraducido = Archivo(salida="output.xml")
        archivoTraducido.escribir(traducido)
    except Exception as error:
        print(error)
        raise error
    else:
        analizador(lexema)