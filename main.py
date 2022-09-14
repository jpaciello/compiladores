from modules.lector import Archivo
from modules.regex import Regex

if(__name__ == '__main__'):
    try:
        lexer = Regex()
        archivo = Archivo()
        codigo = archivo.leer()
        lexema = lexer.getLexema(codigo)
        archivo.escribir(lexema)
        print("Archivo output.txt generado.")
    except Exception as error:
        print(error)