EOF = ''
token = ''
position = 0
errors = []
cadena_caracteres = ''
caracter = ''

def analizador(data):
    global cadena_caracteres
    global errors

    cadena_caracteres = data

    getToken()
    json()

    if(len(errors) < 1) :
        print('Anàlisis de sintaxis realizado correctamente.')
    else: 
        print('Errores ocurridos en a ejecuciòn: ')
        print(errors)


def getToken():
    global caracter
    global cadena_caracteres
    global position
    global token

    while(position < len(cadena_caracteres)):
        if(caracter != 'SKIP'):
            token += caracter
        caracter = cadena_caracteres[position]
        position += 1

        if(caracter == 'ESPACIO' or caracter == 'NEW_LINE'):
            break


def match(token_esperado):
    global token
    if(token == token_esperado):
        getToken()

        if(token == ''):
            getToken()
    else: 
        raise Exception('Se esperaba: ' + token_esperado)


def json():
    element()


def element():
    if(token == 'L_LLAVE'):
        element()
    elif (token == 'L_CORCHETE'):
        array()

def array():
    global errors
    global token 
    try:
        match('L_CORCHETE')
        if(token == 'R_CORCHETE'):
            match('R_CORCHETE')
        else:
            element_list()
            match('R_CORCHETE')
        
    except Exception as e:
        errors.append(str(e))
        while(token != 'R_CORCHETE'):
            getToken()
        match('R_CORCHETE')


def element_list():
    element()
    APrima()


def APrima():
    global token
    if(token == 'COMA'):
        match('COMA')
        element()
        APrima()


def object():
    global errors
    global token
    try:
        match('L_LLAVE')
        if(token == 'R_LLAVE'):
            match('R_LLAVE')
        else:
            attribute_list()
            match('R_LLAVE')
    except Exception as e:
        errors.append(str(e))
        while(token != 'R_LLAVE'):
            getToken()
        match('R_LLAVE')


def attribute_list():
    global errors
    try:
        attribute()
        BPrima()
    except Exception as e:
        errors.append(str(e))
        raise 'Ha ocurrido un error: ' + str(e)


def BPrima():
    global token
    if(token == 'COMA'):
        match('COMA')
        attribute()
        BPrima()

def attribute():
    global errors
    try:
        attribute_name()
        match('DOS_PUNTOS')
        attribute_value()
    except Exception as e:
        errors.append(str(e))
        raise e


def attribute_name():
    global errors
    try:
        match('LITERAL_CADENA')
    except Exception as e:
        errors.append(str(e))
        raise Exception('Se esperaba un tipo: STRING')


def attribute_value():
    global token
    if(token == 'L_CORCHETE' or token == 'L_LLAVE'):
        element()
    elif (token == 'LITERAL_CADENA'):
        match('LITERAL_CADENA')
    elif (token == 'LITERAL_NUM'):
        match('LITERAL_NUM')
    elif(token == 'PR_TRUE'):
        match('PR_TRUE')
    elif(token == 'PR_FALSE'):
        match('PR_FALSE')
    elif(token == 'PR_NULL'):
        match('PR_NULL')
    else:
        raise Exception(
            'Error en el tipo de dato recibido ' +
            'se esperaba uno de los siguientes tipos de datos: ' + 
            'TRUE, FALSE, STRING, NULL, NUMBER.'
        )