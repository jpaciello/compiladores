from modules.lector import Archivo
from modules.regex import Regex

if(__name__ == '__main__'):
    try:
        lexer = Regex()
        archivo = Archivo()
        input = archivo.leer()
        lexema = lexer.getLexema(input)
        archivo.escribir(lexema)
        print("Archivo output.txt generado.")
    except Exception as error:
        print(error)