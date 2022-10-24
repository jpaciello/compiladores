# Se importa la libreria re, para poder identificar expresiones regulares.
from typing import NamedTuple
import re

class Token(NamedTuple):
    type: str
    value: str
    line: int
    column: int
    
class Regex:
    """
    Clase Regex de Regular Expresions que inicializa los lexemas a evaluar.
    """
    
    def __init__(self):
        # Se definen los lexemas a evaluar.
        self.compLexicos = [
            ("NEW_LINE", r'\n'),
            ("L_CORCHETE", r'\['),
            ("R_CORCHETE", r']'),
            ("L_LLAVE", r'{'),
            ("R_LLAVE", r'}'),
            ("COMA", r','),
            ("DOS_PUNTOS", r':'),
            ("LITERAL_CADENA", r'(?P<quote>[\'"]).*?(?P=quote)'), 
            ("LITERAL_NUM", r'[0-9]+(\.[0-9]+)?((e|E)(\+|-)?[0-9]+)?'), 
            ("PR_TRUE", r'true|TRUE'),
            ("PR_FALSE", r'false|FALSE'),
            ("PR_NULL", r'null|NULL'),
            ("SKIP", r'[ \t]+'),
            ("ESPACIO", r' ')
        ]
    
    def getLexema (self, input):
        """
        Se recorre el array de tuplas en donde estan definidos
        los lexemas, y por cada token recibido, verifica si su
        expresion regular coincide con el codigo fuente.
        """
        errores = []
        resultado = ""
        tipos = [e[0] for e in self.compLexicos]
        try:
            token_regex = '|'.join('(?P<%s>%s)' % par for par in self.compLexicos)
            numero_linea = 1 
            inicio_linea = 0

            for token in re.finditer(token_regex, input):
                tipoLexema = token.lastgroup
                valorLexema = token.group()
    
                if(tipoLexema == "NEW_LINE"):
                    numero_linea += 1
                    inicio_linea = token.end()
                    resultado += "\n"
                    continue
                elif(tipoLexema == "LITERAL_CADENA"):
                    resultado += "STRING" + " "
                    continue
                elif(tipoLexema == "LITERAL_NUM"):
                    resultado += "NUMBER" + " "
                    continue
                elif(tipoLexema == "SKIP"):
                    resultado += "\t"
                    continue
                elif(tipoLexema in tipos):
                    resultado += tipoLexema + " "
                    continue
                elif (tipoLexema not in tipos):
                    errores.append('Error, ', valorLexema, ' no específicado entre los lexemas encontrado en la linea: ',numero_linea)
            
            if(len(errores) > 0):
                raise('Errores durante la ejecución: ' + '\n'.join(errores))
            else:
                print(resultado)
                return resultado   
        except Exception as error:
            print(error)