
#include "anlex.h"

/************* Variables globales **************/

int consumir; /* 1 indica al analizador lexico que debe devolver
			  el sgte componente lexico, 0 debe devolver el actual */

char cad[5 * TAMLEX]; // string utilizado para cargar mensajes de error
token t;			  // token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente JSON
FILE *archivo2;			// salida de los componentes lexicos
char buff[2 * TAMBUFF]; // Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero = -1;		// Utilizado por el analizador lexico
int fin = 0;			// Utilizado por el analizador lexico
int numLinea = 1;		// Numero de Linea

// array para poder realizar la salida de los componentes lexicos

/**************** Funciones **********************/

// Rutinas del analizador lexico

void error(const char *mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n", numLinea, mensaje);
}

char *buscarComponenteLexico(int compLex)
{
	// se crea el array con los literales de los componentes lexicos
	char *comp[] = {
		"L_CORCHETE ",
		"R_CORCHETE ",
		"L_LLAVE ",
		"R_LLAVE ",
		"COMA ",
		"DOS_PUNTOS ",
		"STRING ",
		"NUMBER ",
		"PR_TRUE ",
		"PR_FALSE ",
		"PR_NULL "};
	switch (compLex)
	{
	case L_CORCHETE:
		return comp[0];
		break;
	case R_CORCHETE:
		return comp[1];
		break;
	case L_LLAVE:
		return comp[2];
		break;
	case R_LLAVE:
		return comp[3];
		break;
	case COMA:
		return comp[4];
		break;
	case DOS_PUNTOS:
		return comp[5];
		break;
	case STRING:
		return comp[6];
		break;
	case NUMBER:
		return comp[7];
		break;
	case PR_TRUE:
		return comp[8];
		break;
	case PR_FALSE:
		return comp[9];
		break;
	case PR_NULL:
		return comp[10];
		break;
	case ' ':
		return " ";
	case '\t':
		return "\t";
	case '\n':
		return "\n";
	case -1:
		return "\0";
	default:
		error("No se encuentra este componente lexico");
	}
}

void sigLex()
{
	int i = 0;
	char c = 0;
	int acepto = 0;
	int estado = 0;
	char msg[41];
	entrada e;

	while ((c = fgetc(archivo)) != EOF)
	{

		if (c == ' ')
		{
			t.compLex = ' ';
			t.pe = buscar(" ");
			break;
		}
		else if (c == '\t')
		{
			t.compLex = '\t';
			t.pe = buscar("\t");
			break;
		}
		else if (c == '\n')
		{
			// incrementar el numero de linea
			numLinea++;
			// para fomartear el archivo de salida
			t.compLex = '\n';
			t.pe = buscar("\n");
			break;
		}
		else if (isalpha(c))
		{
			// es un identificador (o palabra reservada)
			i = 0;
			do
			{
				id[i] = c;
				i++;
				c = fgetc(archivo);
				if (i >= TAMLEX)
					error("Longitud de la palabra reservada excede tama�o de buffer");
			} while (isalpha(c) || isdigit(c));
			id[i] = '\0';
			if (c != EOF)
				ungetc(c, archivo);
			else
				c = 0;
			t.pe = buscar(id);
			t.compLex = t.pe->compLex;
			if (t.pe->compLex == -1)
			{
				char cad[300] = "Esta palabra reservada no pertece al lenguaje: ";
				error(strcat(cad, id));
			}
			break;
		}
		else if (isdigit(c))
		{
			// es un numero
			i = 0;
			estado = 0;
			acepto = 0;
			id[i] = c;

			while (!acepto)
			{
				switch (estado)
				{
				case 0: // una secuencia netamente de digitos, puede ocurrir . o e
					c = fgetc(archivo);
					if (isdigit(c))
					{
						id[++i] = c;
						estado = 0;
					}
					else if (c == '.')
					{
						id[++i] = c;
						estado = 1;
					}
					else if (tolower(c) == 'e')
					{
						id[++i] = c;
						estado = 3;
					}
					else
					{
						estado = 6;
					}
					break;

				case 1: // un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
					c = fgetc(archivo);
					if (isdigit(c))
					{
						id[++i] = c;
						estado = 2;
					}
					else if (c == '.')
					{
						i--;
						fseek(archivo, -1, SEEK_CUR);
						estado = 6;
					}
					else
					{
						sprintf(msg, "No se esperaba '%c'", c);
						estado = -1;
					}
					break;
				case 2: // la fraccion decimal, pueden seguir los digitos o e
					c = fgetc(archivo);
					if (isdigit(c))
					{
						id[++i] = c;
						estado = 2;
					}
					else if (tolower(c) == 'e')
					{
						id[++i] = c;
						estado = 3;
					}
					else
						estado = 6;
					break;
				case 3: // una e, puede seguir +, - o una secuencia de digitos
					c = fgetc(archivo);
					if (c == '+' || c == '-')
					{
						id[++i] = c;
						estado = 4;
					}
					else if (isdigit(c))
					{
						id[++i] = c;
						estado = 5;
					}
					else
					{
						sprintf(msg, "No se esperaba '%c'", c);
						estado = -1;
					}
					break;
				case 4: // necesariamente debe venir por lo menos un digito
					c = fgetc(archivo);
					if (isdigit(c))
					{
						id[++i] = c;
						estado = 5;
					}
					else
					{
						sprintf(msg, "No se esperaba '%c'", c);
						estado = -1;
					}
					break;
				case 5: // una secuencia de digitos correspondiente al exponente
					c = fgetc(archivo);
					if (isdigit(c))
					{
						id[++i] = c;
						estado = 5;
					}
					else
					{
						estado = 6;
					}
					break;
				case 6: // estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
					if (c != EOF)
						ungetc(c, archivo);
					else
						c = 0;
					id[++i] = '\0';
					acepto = 1;
					t.pe = buscar(id);
					if (t.pe->compLex == -1)
					{
						strcpy(e.lexema, id);
						e.compLex = NUMBER;
						insertar(e);
						t.pe = buscar(id);
					}
					t.compLex = NUMBER;
					break;
				case -1:
					if (c == EOF)
						error("No se esperaba el fin de archivo");
					else
						error(msg);
					exit(1);
				}
			}
			break;
		}
		else if (c == ':')
		{
			t.compLex = DOS_PUNTOS;
			t.pe = buscar(":");
			break;
		}
		else if (c == ',')
		{
			t.compLex = COMA;
			t.pe = buscar(",");
			break;
		}
		else if (c == '[')
		{
			t.compLex = L_CORCHETE;
			t.pe = buscar("[");
			break;
		}
		else if (c == ']')
		{
			t.compLex = R_CORCHETE;
			t.pe = buscar("]");
			break;
		}
		else if (c == '\"')
		{ // un caracter o una cadena de caracteres
			i = 0;
			id[i] = c;
			i++;
			do
			{
				c = fgetc(archivo);
				if (c == '\"')
				{
					c = fgetc(archivo);
					if (c == '\"')
					{
						id[i] = c;
						i++;
						id[i] = c;
						i++;
					}
					else
					{
						id[i] = '\"';
						i++;
						break;
					}
				}
				else if (c == EOF)
				{
					error("Se llego al fin de archivo sin finalizar un literal");
				}
				else
				{
					id[i] = c;
					i++;
				}
			} while (isascii(c));
			id[i] = '\0';
			if (c != EOF)
				ungetc(c, archivo);
			else
				c = 0;
			t.pe = buscar(id);
			t.compLex = t.pe->compLex;
			if (t.pe->compLex == -1)
			{
				strcpy(e.lexema, id);
				e.compLex = STRING;
				insertar(e);
				t.pe = buscar(id);
				t.compLex = e.compLex;
			}
			break;
		}
		else if (c == '{')
		{
			t.compLex = L_LLAVE;
			t.pe = buscar("{");
			break;
		}
		else if (c == '}')
		{
			t.compLex = R_LLAVE;
			t.pe = buscar("}");
			break;
		}
		else if (c != EOF)
		{
			sprintf(msg, "%c no esperado", c);
			error(msg);
		}
	}
	if (c == EOF)
	{
		t.compLex = EOF;
		// strcpy(e.lexema,"EOF");
		sprintf(e.lexema, "EOF");
		t.pe = &e;
	}
}

int main(int argc, char *args[])
{
	// inicializar analizador lexico

	initTabla();
	initTablaSimbolos();

	if (argc > 1)
	{
		if (!(archivo = fopen(args[1], "rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		if (!(archivo2 = fopen("output.txt", "w")))
		{
			printf("No se genera el archivo de salida.\n");
			exit(1);
		}
		while (t.compLex != EOF)
		{
			sigLex();
			fprintf(archivo2, "%s", buscarComponenteLexico(t.compLex));
			// printf("Lin %d: %s -> %d\n",numLinea,t.pe->lexema,t.compLex);
		}
		fclose(archivo);
		fclose(archivo2);
	}
	else
	{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}