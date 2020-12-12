#include <windows.h> 
#include "resource.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <wctype.h>
#include <fstream>
using namespace std;

struct Medico {
	char  nombre[MAX_PATH];
	char cedula[MAX_PATH];
	int IntCedula;
	char especialidad[MAX_PATH];
	char numconsultorio[MAX_PATH];
	char horario[MAX_PATH];
	bool Lunes;
	bool Martes;
	bool Miercoles;
	bool Jueves;
	bool Viernes;
	char telefono[MAX_PATH];
	int Contador = 0;
	char chFileName[MAX_PATH];
	Medico* derecha;
	Medico* izquierda;
	Medico* previo;
} arbol, * raiz;


static HBITMAP hBmpImg;

struct Especialidad {
	char especialidad[MAX_PATH];
	char descripcion[MAX_PATH];
	Especialidad* next;
	Especialidad* prev;
}*originE, * auxE;

struct Registros{
	char paciente[MAX_PATH];
	char especialidad[MAX_PATH];
	char nombreMedico[MAX_PATH];
	char Fecha[MAX_PATH];
	char Mes[MAX_PATH];
	char Dia[MAX_PATH];
	char Ano[MAX_PATH];
	char Semana[MAX_PATH];
	char hora[MAX_PATH];
	char minutos[MAX_PATH];
	int contador;
	char apellido[MAX_PATH];
	char TelefonoPaciente[MAX_PATH];
	char Consultorio[MAX_PATH];
	char NumeroCitas[MAX_PATH];
	bool EstadoConsulta = false;
	Registros* next;
	Registros* prev;
}*originR, * auxR;

struct Paciente {
	char nombre[MAX_PATH];
	int genero;
	char apellido[MAX_PATH];
    char numconsultorio;
	char edad[MAX_PATH];
	char fechaNacimiento[MAX_PATH];
	char Telefono[MAX_PATH];
	char descripcion[MAX_PATH];
	char MedicoRecomendado[MAX_PATH];
	int contador=0;
	Paciente* next;
	Paciente* prev;
}*originP, * auxP;
fstream myFile;
fstream miarchivo;
fstream miarchivoBIN;
fstream myFile1;
fstream myFile2;
fstream MedicosBIN;
fstream contadorRe;
fstream ContadorPa;

int contadorR = 0;
int contadorM = 0;
int contadorP = 0;
char semana[MAX_PATH] = "";
bool flag;

void buscadorCedula(Medico* Reserv, int intcedula, HWND hWnd) {
	
	if (Reserv != NULL) {
		buscadorCedula(Reserv->izquierda, intcedula, hWnd);
		if (Reserv->IntCedula == intcedula) {
			return;
		}
		buscadorCedula(Reserv->derecha, intcedula, hWnd);
	}

}
void EliminarEspecialidad(Especialidad*& originE, Especialidad*& auxE, char especialidad[MAX_PATH]) {
	if (originE == NULL)
		return;

	bool found = true;
	while (strcmp(auxE->especialidad, especialidad)!=0) {
		if (auxE->next == NULL)
		{
			found = false;
			return;
		}
		auxE = auxE->next;
	}
	if (!found)
		return;
	/*Casos de Borrado*/
	 /*Caso de unico elemento*/
	if (auxE == originE) {
		originE = originE->next;
	}
	else if (auxE->prev == NULL && auxE->next == NULL) {
		delete auxE;
		auxE = originE = NULL;
	}
	else if (auxE->next == NULL) /*Caso de primer elemento*/ {
		auxE->prev->next = NULL;
		delete auxE;
	}
	else if (auxE->prev == NULL) /*Caso de ultimo elemento*/ {
		auxE->next->prev = NULL;
		delete auxE;
	}
	else { /*cualquier otro*/
		auxE->next->prev = auxE->prev;
		auxE->prev->next = auxE->next;
		delete auxE;
	}
}

void ContadorRegistroGuardar() {
	ContadorPa.open("Contador Registro y Paciente.txt", ios::out | ios::trunc);
	if (!ContadorPa.is_open())
	{
		MessageBox(0, "ERROR AL GUARDAR Contador Registro", "ERROR", MB_ICONERROR);
		return;
	}
	ContadorPa << contadorR << endl;
	ContadorPa << contadorP << endl;

	ContadorPa.close();
}
ifstream ContadorCARGAR;
void ContadorRegistroCargar() {
	ContadorCARGAR.open("Contador Registro y Paciente.txt", ios::in);
	if (!ContadorCARGAR.is_open())
	{
		MessageBox(0, "ERROR AL GUARDAR Contador Registro", "ERROR", MB_ICONERROR);
		return;
	}
	while (!ContadorCARGAR.eof()) {
		ContadorCARGAR >> contadorR;
		ContadorCARGAR >> contadorP;
	}
	ContadorCARGAR.close();

}

void FDatosMedicosREGISTROSCITAS(Registros*& registro, Medico*& medico, HWND hWnd) {
	if (medico != NULL) {
		FDatosMedicosREGISTROSCITAS(registro, medico->izquierda, hWnd);
		if (strcmp(medico->nombre, registro->nombreMedico) == 0) {
			strcpy_s(registro->Consultorio, medico->numconsultorio);
			//myFile2 << "Consultorio:\t" << medico->numconsultorio << endl;
		}

		FDatosMedicosREGISTROSCITAS(registro, medico->derecha, hWnd);
	}
}

void FApellidoPacienteREGISTROSCITA(Registros*& registro, Paciente*& paciente, HWND hWnd) {
	flag = false;
	while (paciente != NULL) {
		if (strcmp(registro->paciente, paciente->nombre) == 0) {
			strcpy_s(registro->apellido, paciente->apellido);

			//myFile2 << "Telefono:\t" << paciente->Telefono << "\n";
			return;
		}
		else {
			paciente = paciente->next;
		}
	}

}
void FTelefonoPacienteREGISTROSCITA(Registros*& registro, Paciente*& paciente, HWND hWnd) {
	flag = false;
	while (paciente != NULL) {
		if (strcmp(registro->paciente, paciente->nombre) == 0) {
			strcpy_s(registro->TelefonoPaciente, paciente->Telefono);

			//myFile2 << "Telefono:\t" << paciente->Telefono << "\n";
			return;
		}
		else {
			paciente = paciente->next;
		}
	}

}
fstream EspecialidadesGuardar;
void GuardarEspecialidades(Especialidad*&reserv,HWND hWnd) {
		if (reserv != NULL) {
			EspecialidadesGuardar.open("Especialidades.bin", ios::out | ios::trunc | ios::binary);
			if (!EspecialidadesGuardar.is_open()) { //Revisamos que el archivo este disponible, caso contrario, terminamos.
				MessageBox(hWnd, "ERROR AL GUARDAR ARCHIVOS ESPECIALIDAD", "ERROR", MB_ICONERROR);
				return;
			}
			Especialidad* temp = originE; //Usamos otro puntero para avanzar y no mover aux
			while (temp != NULL) {
				EspecialidadesGuardar.write(reinterpret_cast<char*>(temp), sizeof(Especialidad));

				temp = temp->next;
			}

		}
		EspecialidadesGuardar.close(); //Cerramos el archivo.
		return;

	}

void CargarEspecialidades() {
	//Nos aseguramos de estar al final del archivo binario.
	EspecialidadesGuardar.open("Especialidades.bin", ios::in | ios::binary | ios::ate);
	if (!EspecialidadesGuardar.is_open()) {
		MessageBox(0, "No se pudo abrir el archivo", "CARGAR REGISTROS", MB_OK);
		return;
	}
	int size = EspecialidadesGuardar.tellg();
	if (size == 0) { /*Como nos pusimos al final si este nos devuelve 0 entonces no hay info*/
		MessageBox(0, "Archivo Vacio", "CARGAR REGISTROS", MB_OK);
		return;
	}
	for (int i = 0; i < (size / sizeof(Especialidad)); i++) {
		if (originE == NULL) {
			originE = new Especialidad;
			auxE = originE;
			auxE->prev = NULL;
		}
		else {
			while (auxE->next != NULL)
				auxE = auxE->next;
			auxE->next = new Especialidad;
			auxE->next->prev = auxE;
			auxE = auxE->next;
		}
		Especialidad* Temp = new Especialidad;

		EspecialidadesGuardar.seekg(i * sizeof(Especialidad));


		EspecialidadesGuardar.read(reinterpret_cast<char*>(Temp), sizeof(Especialidad));

		strcpy_s(auxE->especialidad, Temp->especialidad);
		strcpy_s(auxE->descripcion, Temp->descripcion);

		delete reinterpret_cast<char*>(Temp);

		auxE->next = NULL;
		auxE = originE;
	}
	EspecialidadesGuardar.close();
	MessageBox(0, "Cargado Exitoso", "CARGAR ESPECIALIDAD", MB_OK);
}



class ListaLigadasRegistros{
public:
	void NoRepetirNUMERO(Registros*& Reservadoo, HWND hWnd,char numerocita[MAX_PATH]) {
		if (Reservadoo != NULL) {
			if (strcmp(Reservadoo->NumeroCitas, numerocita) == 0) {
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {
				NoRepetirNUMERO(Reservadoo->next, hWnd, numerocita);
			}
		}
	}

	void CargarRegistro() {
			//Nos aseguramos de estar al final del archivo binario.
			myFile.open("Registros.bin", ios::in | ios::binary | ios::ate);
			if (!myFile.is_open()) {
				MessageBox(0, "No se pudo abrir el archivo", "CARGAR REGISTROS", MB_OK);
				return;
			}
			int size = myFile.tellg();
			if (size == 0) { /*Como nos pusimos al final si este nos devuelve 0 entonces no hay info*/
				MessageBox(0, "Archivo Vacio", "CARGAR REGISTROS", MB_OK);
				return;
			}
			for (int i = 0; i < (size / sizeof(Registros)); i++) {
				if (originR == NULL) {
					originR = new Registros;
					auxR = originR;
					auxR->prev = NULL;
				}
				else {
					while (auxR->next != NULL)
						auxR = auxR->next;
					auxR->next = new Registros;
					auxR->next->prev = auxR;
					auxR = auxR->next;
				}
				Registros*Temp = new Registros;

				myFile.seekg(i * sizeof(Registros));


				myFile.read(reinterpret_cast<char *>(Temp),sizeof(Registros));

				strcpy_s(auxR->paciente,Temp->paciente);
				strcpy_s(auxR->NumeroCitas, Temp->NumeroCitas);
				strcpy_s(auxR->hora, Temp->hora);
				strcpy_s(auxR->minutos, Temp->minutos);
				strcpy_s(auxR->nombreMedico,Temp->nombreMedico);
				strcpy_s(auxR->especialidad,Temp->especialidad);
				strcpy_s(auxR->Fecha, Temp->Fecha);
				auxR->EstadoConsulta=Temp->EstadoConsulta;
				strcpy_s(auxR->Dia, Temp->Dia);
				strcpy_s(auxR->Mes, Temp->Mes);




				delete reinterpret_cast<char *>(Temp);

				auxR->next = NULL;
				auxR = originR;
			  }
			  myFile.close();
			  MessageBox(0, "Cargado Exitoso", "CARGAR REGISTROS", MB_OK);
	}

	void GuardarRegistro(Registros*& reserv, HWND hWnd) {
		if (reserv != NULL) {
			miarchivo.open("Registros.bin", ios::out | ios::trunc |ios::binary);
			if (!miarchivo.is_open()) { //Revisamos que el archivo este disponible, caso contrario, terminamos.
				MessageBox(hWnd, "ERROR AL GUARDAR ARCHIVOS", "ERROR", MB_ICONERROR);
				return;
			}
			Registros* temp = originR; //Usamos otro puntero para avanzar y no mover aux
			while (temp != NULL) {
				miarchivo.write(reinterpret_cast<char*>(temp), sizeof(Registros));

				temp = temp->next;
			}
		
		}
		miarchivo.close(); //Cerramos el archivo.
		return;

	}
	void FiltroMedicoyMES(Registros*& reserv, char MedicoMEScita[MAX_PATH], char MEScita[MAX_PATH], HWND hWnd) {
		if (reserv != NULL) {


			myFile.open("medico y mes.txt", ios::out | ios::trunc);
			if (!myFile.is_open())
			{
				MessageBox(0, "ERROR AL GUARDAR ARCHIVOS", "ERROR", MB_ICONERROR);
				return;
			}
			for (int i = 0; i < contadorR; i++) {
				if (strcmp(reserv->nombreMedico, MedicoMEScita) == 0) {
					if (strcmp(reserv->Mes, MEScita) == 0) {

						if (reserv != NULL) {
							Paciente* Papaciente = originP;
							FApellidoPacienteREGISTROSCITA(reserv, Papaciente, hWnd);
							myFile << "Nombre del Paciente:\t" << reserv->apellido << "  " <<reserv->paciente << "\n";
							myFile << "Nombre del Medico:\t" << reserv->nombreMedico << "\n";
							myFile << "Especialidad:\t" << reserv->especialidad << "\n";
							myFile << "Fecha:\t" << reserv->Fecha << "\n";
							myFile << "hora:\t" << reserv->hora << "\tminutos\t" << reserv->minutos << "\n";
							Papaciente = originP;
							FTelefonoPacienteREGISTROSCITA(reserv, Papaciente, hWnd);
							Medico* mediccoco = raiz;
							FDatosMedicosREGISTROSCITAS(reserv, mediccoco, hWnd);
							myFile << "Telefono Paciente:\t" << reserv->TelefonoPaciente << "\n";
							myFile << "Consultorio:\t" << reserv->Consultorio << "\n";
							if (reserv->EstadoConsulta) {
								myFile << "Estado:\t" << "Aprobado" << "\n";
							}
							else {
								myFile << "Estado:\t" << "En revision" << "\n";
							}
							myFile << "--------------------------------------------------------------" << "\n";
						}
					}
				}
				if (reserv != NULL){
				reserv = reserv->next;
				}

			}
			MessageBox(hWnd, "La información fue guardada", "Filtro MEDICO Y SEMANA", MB_OK);

			myFile.close();


		}
		
	}

	void FiltroMesSemana(Registros*& reserv, char MEScita[MAX_PATH], char semanames[MAX_PATH], HWND hWnd) {
		if (reserv != NULL) {


			myFile1.open("MES Y SEMANA.txt", ios::out | ios::trunc);
			if (!myFile1.is_open())
			{
				MessageBox(0, "ERROR AL GUARDAR ARCHIVOS", "ERROR", MB_ICONERROR);
				return;
			}

			for (int i = 0; i < contadorR; i++) {

				if (strcmp(reserv->Mes, MEScita) == 0) {
					if (strcmp(reserv->Dia, "1") == 0 || strcmp(reserv->Dia, "2") == 0 || strcmp(reserv->Dia, "3") == 0 || strcmp(reserv->Dia, "4") == 0 || strcmp(reserv->Dia, "5") == 0 || strcmp(reserv->Dia, "6") == 0 || strcmp(reserv->Dia, "7") == 0) {
						strcpy_s(semana, "1");
					}
					else if (strcmp(reserv->Dia, "8") == 0 || strcmp(reserv->Dia, "9") == 0 || strcmp(reserv->Dia, "10") == 0 || strcmp(reserv->Dia, "11") == 0 || strcmp(reserv->Dia, "12") == 0 || strcmp(reserv->Dia, "13") == 0 || strcmp(reserv->Dia, "14") == 0) {
						strcpy_s(semana, "2");
					}
					else if (strcmp(reserv->Dia, "15") == 0 || strcmp(reserv->Dia, "16") == 0 || strcmp(reserv->Dia, "17") == 0 || strcmp(reserv->Dia, "18") == 0 || strcmp(reserv->Dia, "19") == 0 || strcmp(reserv->Dia, "20") == 0 || strcmp(reserv->Dia, "21") == 0) {
						strcpy_s(semana, "3");
					}
					else if (strcmp(reserv->Dia, "22") == 0 || strcmp(reserv->Dia, "23") == 0 || strcmp(reserv->Dia, "24") == 0 || strcmp(reserv->Dia, "25") == 0 || strcmp(reserv->Dia, "26") == 0 || strcmp(reserv->Dia, "27") == 0 || strcmp(reserv->Dia, "28") == 0) {
						strcpy_s(semana, "4");
					}
					else if (strcmp(reserv->Dia, "29") == 0 || strcmp(reserv->Dia, "30") == 0 || strcmp(reserv->Dia, "31") == 0) {
						strcpy_s(semana, "5");
					}
					if (strcmp(semana, semanames) == 0) {
						Paciente* Papaciente = originP;
						FApellidoPacienteREGISTROSCITA(reserv, Papaciente, hWnd);
						myFile1 << "Nombre del Paciente:\t" <<reserv->apellido<<"  " <<reserv->paciente << "\n";
						myFile1 << "Nombre del Medico:\t" << reserv->nombreMedico << "\n";
						myFile1 << "Especialidad:\t" << reserv->especialidad << "\n";
						myFile1 << "Fecha:\t" << reserv->Fecha << "\n";
						myFile1 << "hora:\t" << reserv->hora << "\tminutos\t" << reserv->minutos << "\n";

						
						Papaciente = originP;
						FTelefonoPacienteREGISTROSCITA(reserv, Papaciente, hWnd);
						Medico* mediccoco = raiz;
						FDatosMedicosREGISTROSCITAS(reserv, mediccoco, hWnd);
						myFile1 << "Consultorio:\t" << reserv->Consultorio << "\n";

						myFile1 << "Telefono Paciente:\t" << reserv->TelefonoPaciente << "\n";
						if (reserv->EstadoConsulta) {
							myFile1 << "Estado:\t" << "Aprobado" << "\n";
						}
						else {
							myFile1 << "Estado:\t" << "En revision" << "\n";
						}
						myFile1 << "--------------------------------------------------------------" << "\n";
					}
				}
				if (reserv != NULL) {
					reserv = reserv->next;
				}
			}
			MessageBox(hWnd, "La información fue guardada", "Filtro MES Y SEMANA", MB_OK);

			myFile1.close();
		}
	}

	void FiltroEspecialidadMesSemana(Registros*& reserv,char especialidad[MAX_PATH], char MEScita[MAX_PATH], char semanames[MAX_PATH], HWND hWnd) {
		if (reserv != NULL) {


			myFile2.open("Especialidad, MES Y SEMANA.txt", ios::out | ios::trunc);
			if (!myFile2.is_open())
			{
				MessageBox(0, "ERROR AL GUARDAR ARCHIVOS", "ERROR", MB_ICONERROR);
				return;
			}
			for (int i = 0; i < contadorR; i++) {
				if (strcmp(reserv->especialidad, especialidad) == 0) {
					if (strcmp(reserv->Mes, MEScita) == 0) {

						if (strcmp(reserv->Dia, "1") == 0 || strcmp(reserv->Dia, "2") == 0 || strcmp(reserv->Dia, "3") == 0 || strcmp(reserv->Dia, "4") == 0 || strcmp(reserv->Dia, "5") == 0 || strcmp(reserv->Dia, "6") == 0 || strcmp(reserv->Dia, "7") == 0) {
							strcpy_s(semana, "1");
						}
						else if (strcmp(reserv->Dia, "8") == 0 || strcmp(reserv->Dia, "9") == 0 || strcmp(reserv->Dia, "10") == 0 || strcmp(reserv->Dia, "11") == 0 || strcmp(reserv->Dia, "12") == 0 || strcmp(reserv->Dia, "13") == 0 || strcmp(reserv->Dia, "14") == 0) {
							strcpy_s(semana, "2");
						}
						else if (strcmp(reserv->Dia, "15") == 0 || strcmp(reserv->Dia, "16") == 0 || strcmp(reserv->Dia, "17") == 0 || strcmp(reserv->Dia, "18") == 0 || strcmp(reserv->Dia, "19") == 0 || strcmp(reserv->Dia, "20") == 0 || strcmp(reserv->Dia, "21") == 0) {
							strcpy_s(semana, "3");
						}
						else if (strcmp(reserv->Dia, "22") == 0 || strcmp(reserv->Dia, "23") == 0 || strcmp(reserv->Dia, "24") == 0 || strcmp(reserv->Dia, "25") == 0 || strcmp(reserv->Dia, "26") == 0 || strcmp(reserv->Dia, "27") == 0 || strcmp(reserv->Dia, "28") == 0) {
							strcpy_s(semana, "4");
						}
						else if (strcmp(reserv->Dia, "29") == 0 || strcmp(reserv->Dia, "30") == 0 || strcmp(reserv->Dia, "31") == 0) {
							strcpy_s(semana, "5");
						}
						if (strcmp(semana, semanames) == 0) {
							if (reserv != NULL) {
								Paciente* Papaciente = originP;
								FApellidoPacienteREGISTROSCITA(reserv, Papaciente, hWnd);
								myFile2 << "Nombre del Paciente:\t" <<reserv->apellido << "  " << reserv->paciente << "\n";
								myFile2 << "Nombre del Medico:\t" << reserv->nombreMedico << "\n";
								myFile2 << "Especialidad:\t" << reserv->especialidad << "\n";
								myFile2 << "Fecha:\t" << reserv->Fecha << "\n";
								myFile2 << "hora:\t" << reserv->hora << "\tminutos\t" << reserv->minutos << "\n";
								Papaciente = originP;
								FTelefonoPacienteREGISTROSCITA(reserv, Papaciente,hWnd);
								Medico* mediccoco = raiz;
								FDatosMedicosREGISTROSCITAS(reserv, mediccoco, hWnd);
								myFile2 << "Consultorio:\t" << reserv->Consultorio << "\n";
								myFile2 << "Telefono Paciente:\t" << reserv->TelefonoPaciente << "\n";


								if (reserv->EstadoConsulta) {
									myFile2 << "Estado:\t" << "Aprobado" << "\n";
								}
								else {
									myFile2 << "Estado:\t" << "En revision" << "\n";
								}
								myFile2 << "--------------------------------------------------------------" << "\n";
							}
						}
						

					}
				
				}
				if (reserv != NULL) {
					reserv = reserv->next;
				}
			}
			MessageBox(hWnd, "La información fue Guardada", "Filtro Especialidad, MES Y SEMANA", MB_OK);
			myFile2.close();
		}
	}

	void darAlta(Registros*& originR, Registros*& auxR) {
		if (originR == NULL) {
			originR = new Registros;
			auxR = originR;
			auxR->prev = NULL;
		}
		else {
			while (auxR->next != NULL)
				auxR = auxR->next;
			auxR->next = new Registros;
			auxR->next->prev = auxR;
			auxR = auxR->next;
		}
	}
	void Eliminar(Registros*& originR, Registros*& auxR, char Cita[MAX_PATH]) {
		if (originR == NULL)
			return;

		bool found = true;
		while (strcmp(auxR->NumeroCitas,Cita)!=0) {
			if (auxR->next == NULL)
			{
				found = false;
				return;
			}
			auxR = auxR->next;
		}
		if (!found)
			return;
		/*Casos de Borrado*/
		 /*Caso de unico elemento*/
		if (auxR == originR) {
			originR = originR->next;

		}
		else if (auxR->prev == NULL && auxR->next == NULL) {
			delete auxR;
			auxR = originR = NULL;
		}
		else if (auxR->next == NULL) /*Caso de primer elemento*/ {
			auxR->prev->next = NULL;
			delete auxR;
		}
		else if (auxR->prev == NULL) /*Caso de ultimo elemento*/ {
			auxR->next->prev = NULL;
			delete auxR;
		}
		else { /*cualquier otro*/
			auxR->next->prev = auxR->prev;
			auxR->prev->next = auxR->next;
			delete auxR;
		}
	}
};
ListaLigadasRegistros* Registro;
class ListaLigadas {
	fstream PacientesGuard;
	fstream PacientesGuardBIN;
public:
	void CargarPacientes() {
		
		//Nos aseguramos de estar al final del archivo binario.
		PacientesGuardBIN.open("PACIENTES.bin", ios::in | ios::binary | ios::ate);
		if (!PacientesGuardBIN.is_open()) {
			MessageBox(0, "No se pudo abrir el archivo", "CARGAR REGISTROS", MB_OK);
			return;
		}
		int size = PacientesGuardBIN.tellg();
		if (size == 0) { /*Como nos pusimos al final si este nos devuelve 0 entonces no hay info*/
			MessageBox(0, "Archivo Vacio", "CARGAR REGISTROS", MB_OK);
			return;
		}
		for (int i = 0; i < (size / sizeof(Paciente)); i++) {
			if (originP == NULL) {
				originP = new Paciente;
				auxP = originP;
				auxP->prev = NULL;
			}
			else {
				while (auxP->next != NULL)
					auxP = auxP->next;
				auxP->next = new Paciente;
				auxP->next->prev = auxP;
				auxP = auxP->next;
			}
			Paciente* Temp = new Paciente;

			PacientesGuardBIN.seekg(i * sizeof(Paciente));


			PacientesGuardBIN.read(reinterpret_cast<char*>(Temp), sizeof(Paciente));
			strcpy_s(auxP->apellido, Temp->apellido);
			strcpy_s(auxP->nombre, Temp->nombre);
			strcpy_s(auxP->fechaNacimiento, Temp->fechaNacimiento);
			strcpy_s(auxP->edad, Temp->edad);
			strcpy_s(auxP->MedicoRecomendado, Temp->MedicoRecomendado);
			auxP->genero = Temp->genero;
			strcpy_s(auxP->descripcion, Temp->descripcion);
			strcpy_s(auxP->Telefono, Temp->Telefono);



			delete reinterpret_cast<char*>(Temp);

			auxP->next = NULL;
			auxP = originP;
		}
		myFile.close();
		MessageBox(0, "Cargado Exitoso", "CARGAR REGISTROS", MB_OK);
	}
	void GuardarPacientesBIN(Paciente*& reserv, HWND hWnd) {
		if (reserv != NULL) {
			PacientesGuardBIN.open("PACIENTES.bin", ios::out | ios::trunc | ios::binary);
			if (!PacientesGuardBIN.is_open()) { //Revisamos que el archivo este disponible, caso contrario, terminamos.
				MessageBox(hWnd, "ERROR AL GUARDAR ARCHIVOS", "ERROR", MB_ICONERROR);
				return;
			}
			Paciente* temp = originP; //Usamos otro puntero para avanzar y no mover aux
			while (temp != NULL) {
				PacientesGuardBIN.write(reinterpret_cast<char*>(temp), sizeof(Paciente));

				temp = temp->next;
			}

		}
		PacientesGuardBIN.close(); //Cerramos el archivo.
		return;

	}

	void guardadoPaciente(Paciente*& reserv, HWND hWnd) {
		if (reserv != NULL) {
			PacientesGuard.open("Pacientes.txt", ios::out | ios::trunc);
			if (!PacientesGuard.is_open())
			{
				MessageBox(0, "ERROR AL GUARDAR ARCHIVOS", "ERROR", MB_ICONERROR);
				return;
			}
			for (int i = 0; i < contadorP; i++) {
						if (reserv != NULL) {
							PacientesGuard << "Nombre del Paciente:\t" << reserv->nombre << "\n";
							PacientesGuard << "Fecha Naciemiento :\t" << reserv->fechaNacimiento << "\n";
							if (reserv->genero) {
								PacientesGuard << "Genero:\t" << "Masculino" << "\n";
							}
							else {
								PacientesGuard << "Genero:\t" << "femenino" << "\n";
								
							}	
							PacientesGuard << "Edad\t" << reserv->edad << endl;
							PacientesGuard << "Telefono\t" << reserv->Telefono << endl;
							PacientesGuard << "Medico Recomendado:\t" << reserv->MedicoRecomendado << "\n";
							PacientesGuard << "Descripcion:\t" << reserv->descripcion << "\n";
							PacientesGuard << "--------------------------------------------------------------" << "\n";

						}
						if (reserv != NULL) {
							reserv = reserv->next;
						}

					}
			PacientesGuard.close();
				}

	}
	void darAlta(Paciente*& originP, Paciente*& auxP) {
		if (originP == NULL) {
			originP = new Paciente;
			auxP = originP;
			auxP->prev = NULL;
		}
		else {
			while (auxP->next != NULL)
				auxP = auxP->next;
			auxP->next = new Paciente;
			auxP->next->prev = auxP;
			auxP = auxP->next;
		}
	}
	void Eliminar(Paciente*& originP, Paciente*& auxp, char Paciente[MAX_PATH]) {
		if (originP == NULL)
			return;
		bool found = true;
		while (strcmp(auxp->nombre, Paciente)!=0) {
			if (auxp->next == NULL)
			{
				found = false;
				return;
			}
			auxp = auxp->next;
		}
		if (!found)
			return;
		/*Casos de Borrado*/
		 /*Caso de unico elemento*/
		if (auxp== originP) {
			originP = originP->next;

		}
		else if (auxp->prev == NULL && auxp->next == NULL) {
			delete auxp;
			auxp = originP = NULL;
		}
		else if (auxp->next == NULL) /*Caso de primer elemento*/ {
			auxp->prev->next = NULL;
			delete auxp;
		}
		else if (auxp->prev == NULL) /*Caso de ultimo elemento*/ {
			auxp->next->prev = NULL;
			delete auxp;
		}
		else { /*cualquier otro*/
			auxp->next->prev = auxp->prev;
			auxp->prev->next = auxp->next;
			delete auxp;
		}
	}
};
ListaLigadas* Pacientes;

enum lista {
	MOSTRAR,
	ESCONDER,
	DESACTIVAR
};
enum Semana {
	LUNES,
	MARTES,
	MIERCOLES,
	JUEVES,
	VIERNES
};

void validaciones(int graficon, HWND hWnd,int a) {
	if (a == MOSTRAR) {
		HWND ValidacionMedic = GetDlgItem(hWnd, graficon);
		ShowWindow(ValidacionMedic, SW_SHOW);
	}
	if (a == ESCONDER) {
		HWND ValidacionMedic = GetDlgItem(hWnd, graficon);
		ShowWindow(ValidacionMedic, SW_HIDE);
	}
	if (a == DESACTIVAR) {
		HWND ValidacionMedic = GetDlgItem(hWnd, graficon);
		EnableWindow(ValidacionMedic, false);
	}
}

class Arbolito {

public:
	void mama(Medico* mamita, Medico* aux) {
		if (mamita != NULL) {
			mama(mamita->izquierda, aux);
			if (mamita->izquierda == aux) {
				mamita->izquierda->previo = mamita;
				return;
			}
			else if (mamita->derecha == aux) {
				mamita->derecha->previo = mamita;
				return;
			}
			mama(mamita->derecha, aux);
		}
	}

	void insertarMedico(Medico*& arbol, Medico n) {
		if (arbol == NULL) {
			Medico* Nuevo_Medico = new Medico();
			*Nuevo_Medico = n;
			arbol = Nuevo_Medico;
			if (raiz == NULL) {
				raiz = arbol;
				raiz->previo = NULL;
			}
			else {
				mama(raiz, Nuevo_Medico);
			}
			Nuevo_Medico->derecha = NULL;
			Nuevo_Medico->izquierda = NULL;
		}
		else {
			if (n.IntCedula < arbol->IntCedula) {
				insertarMedico(arbol->izquierda, n);
			}
			else {
				insertarMedico(arbol->derecha, n);
			}
		}
	}
	void cargarMedicosBIN() {
		MedicosBIN.open("MEDICOS.bin", ios::in | ios::binary | ios::ate);
		if (!MedicosBIN.is_open()) {
			MessageBox(0, "No se pudo abrir el archivo", "CARGAR MEDICOS", MB_OK);
			return;
		}
		int size = MedicosBIN.tellg();
		if (size == 0) { /*Como nos pusimos al final si este nos devuelve 0 entonces no hay info*/
			MessageBox(0, "Archivo Vacio", "CARGAR MEDICOS", MB_OK);
			return;
		}
		for (int i = 0; i < (size / sizeof(Medico)); i++) {


			Medico* Temp = new Medico;

			MedicosBIN.seekg(i * sizeof(Medico));


			MedicosBIN.read(reinterpret_cast<char*>(Temp), sizeof(Medico));
			strcpy_s(arbol.nombre, Temp->nombre);
			strcpy_s(arbol.especialidad, Temp->especialidad);
			strcpy_s(arbol.horario, Temp->horario);
			arbol.IntCedula = Temp->IntCedula;
			arbol.Lunes = Temp->Lunes;
			arbol.Martes = Temp->Martes;
			arbol.Miercoles = Temp->Miercoles;
			arbol.Jueves = Temp->Jueves;
			arbol.Viernes = Temp->Viernes;
			strcpy_s(arbol.cedula, Temp->cedula);
			strcpy_s(arbol.numconsultorio, Temp->numconsultorio);
			strcpy_s(arbol.chFileName, Temp->chFileName);
			strcpy_s(arbol.telefono, Temp->telefono);
			insertarMedico(raiz, arbol);




			delete reinterpret_cast<char*>(Temp);
		}
		MedicosBIN.close();
		MessageBox(0, "Cargado Exitoso", "CARGAR REGISTROS", MB_OK);
	}
	fstream GuardarMedicos;
	void guardar(Medico*&reservar,HWND hWnd) {
		GuardarMedicos.open("Medicos.txt", ios::out | ios::trunc);
		if (!GuardarMedicos.is_open())
		{
			MessageBox(0, "ERROR AL GUARDAR ARCHIVOS", "ERROR", MB_ICONERROR);
			return;
		}
		auxMedic(reservar);
		
	}
	void auxMedic(Medico*& reservar) {
		if (reservar != NULL) {
			auxMedic(reservar->izquierda);
			GuardarMedicos << "Nombre del Medico:\t" << reservar->nombre << "\n";
			GuardarMedicos << "Numero de cedula:\t" << reservar->IntCedula << "\n";
			GuardarMedicos << "Telefono:\t" << reservar->telefono << "\n";
			GuardarMedicos << "Consultorio:\t" << reservar->numconsultorio << "\n";
			GuardarMedicos << "Especialidad:\t" << reservar->especialidad << "\n";
			if (reservar->horario) {
				GuardarMedicos << "horario:\t Mañana\t"<<"6 Am a 13 Am "<< "\n";

			}
			else {
				GuardarMedicos << "horario:\t Tarde\t" << "13 Pm a 19 Pm "<< "\n";

			}
			GuardarMedicos << "Día en que trabaja:" << endl;
			if (reservar->Lunes) {
				GuardarMedicos << "Lunes" << "\n";
			}
			if (reservar->Martes) {
				GuardarMedicos << "Martes" << "\n";
			}
			if (reservar->Miercoles) {
				GuardarMedicos << "Miercoles" << "\n";
			}
			if (reservar->Jueves) {
				GuardarMedicos << "Jueves" << "\n";
			}
			if (reservar->Viernes) {
				GuardarMedicos << "Viernes" << "\n";
			}
			GuardarMedicos << "-----------------------------------------------" << endl;




			auxMedic(reservar->derecha);
			if (reservar == NULL) {
				GuardarMedicos.close();

			}
		}
	}

	void guardarMedicoAux(Medico*&temp) {
		if (temp != NULL) {
			guardarMedicoAux(temp->izquierda);
			miarchivoBIN.write(reinterpret_cast<char*>(temp), sizeof(Medico));
			guardarMedicoAux(temp->derecha);

		}
	}
	void GuardarMedico(Medico*& reserv, HWND hWnd) {
		if (reserv != NULL) {
			miarchivoBIN.open("MEDICOS.bin", ios::out | ios::trunc | ios::binary);
			if (!miarchivoBIN.is_open()) { //Revisamos que el archivo este disponible, caso contrario, terminamos.
				MessageBox(hWnd, "ERROR AL GUARDAR ARCHIVOS", "ERROR", MB_ICONERROR);
				return;
			}
			Medico* temp = raiz; //Usamos otro puntero para avanzar y no mover aux
			guardarMedicoAux(temp);
			miarchivoBIN.close(); //Cerramos el archivo.
			return;

		}
	}
	void MananaNocheConsulta(Medico*& reserv, char Medico[MAX_PATH], HWND hwnd, char HoraConsul[MAX_PATH]) {
		if (reserv != NULL) {

			MananaNocheConsulta(reserv->izquierda, Medico, hwnd, HoraConsul);

			if (flag) {
				if (strcmp(Medico, reserv->nombre) == 0) {
					if (strcmp(HoraConsul, "06") == 0 || strcmp(HoraConsul, "07") == 0 || strcmp(HoraConsul, "08") == 0 || strcmp(HoraConsul, "08") == 0 || strcmp(HoraConsul, "09") == 0 || strcmp(HoraConsul, "10") == 0 || strcmp(HoraConsul, "11") == 0 || strcmp(HoraConsul, "12") == 0) {
						if (strcmp(reserv->horario, "1") == 0) {
							flag = true;
						}
						else {
							flag = false;
							MessageBox(hwnd, "No se puede añadir un horario en horario de MAÑANA ya que el Medico trabaja en la TARDE ", "HORA y MINUTOS", MB_OK);

						}

					}
					else {
						if (strcmp(reserv->horario, "2") == 0) {
							flag = true;
						}
						else {
							flag = false;
							MessageBox(hwnd, "No se puede añadir un horario en horario de TARDE ya que el Medico trabaja en la MAÑANA ", "HORA y MINUTOS", MB_OK);

						}

					}
				}
			}
			MananaNocheConsulta(reserv->derecha, Medico, hwnd, HoraConsul);

		}
	}

	void IdentificadorSemana(Medico*reserv,char cedula[MAX_PATH],HWND hWnd,char BufferSEMANALarga[MAX_PATH]) {

		if (reserv != NULL) {
			IdentificadorSemana(reserv->izquierda, cedula, hWnd, BufferSEMANALarga);
			if (strcmp(cedula, reserv->nombre) == 0) {
				if (strcmp(BufferSEMANALarga, "lunes") == 0) {
					if (reserv->Lunes)
						flag = true;
				}
				else if (strcmp(BufferSEMANALarga, "martes") == 0) {
					if (reserv->Martes)
						flag = true;
				}
				else if (strcmp(BufferSEMANALarga, "miércoles") == 0) {
					if (reserv->Miercoles)
						flag= true;
				}
				else if (strcmp(BufferSEMANALarga, "jueves") == 0) {
					if (reserv->Jueves)
						flag= true;
				}
				else if (strcmp(BufferSEMANALarga, "viernes") == 0) {
					if (reserv->Viernes)
						flag= true;
				}
				else if (strcmp(BufferSEMANALarga, "sábado") == 0) {
					flag= false;
				}
				else if (strcmp(BufferSEMANALarga, "domingo") == 0) {
					flag=  false;
				}
				if (flag == false) {
					MessageBox(hWnd, "No se puede añadir medico en un dia el cual no trabaja el medico", "Medicos", MB_OK);
					flag = false;
				}
			}
			IdentificadorSemana(reserv->derecha, cedula, hWnd, BufferSEMANALarga);
		}
	}

	void identificadorMedicos(Medico* reservMeHo,  char consultorio[MAX_PATH],char DiaNoche[MAX_PATH],bool BuffLunes,bool BuffMartes,bool BuffMiercoles,bool BuffJueves,bool BuffViernes,HWND hWnd) {
		if (reservMeHo != NULL) {
			identificadorMedicos(reservMeHo->izquierda, consultorio,DiaNoche, BuffLunes, BuffMartes, BuffMiercoles, BuffJueves, BuffViernes, hWnd);
			if (strcmp(consultorio, reservMeHo->numconsultorio) == 0) {
				if (strcmp(reservMeHo->horario, DiaNoche)==0) {
					if (BuffLunes == reservMeHo->Lunes && BuffLunes == true) {
						MessageBox(hWnd, "No se puede añadir medico en LUNES en el mismo horario", "Medicos", MB_OK);
						flag = false;
					}
					 else if (BuffMartes == reservMeHo->Martes&& BuffMartes == true) {
						MessageBox(hWnd, "No se puede añadir medico en MARTES en el mismo horario", "Medicos", MB_OK);
						flag = false;
					}
					else if (BuffMiercoles == reservMeHo->Miercoles && BuffMiercoles == true) {
						MessageBox(hWnd, "No se puede añadir medico en MIERCOLES en el mismo horario", "Medicos", MB_OK);
						flag = false;
					}
					else if (BuffJueves == reservMeHo->Jueves && BuffJueves == true) {
						MessageBox(hWnd, "No se puede añadir medico en JUEVES en el mismo horario", "Medicos", MB_OK);
						flag = false;
					}
					else if (BuffViernes == reservMeHo->Viernes && BuffViernes == true) {
						MessageBox(hWnd, "No se puede añadir medico en VIERNES en el mismo horario", "Medicos", MB_OK);
						flag = false;
					}
				}
			}

			if(flag){
				flag = true;
			}
			identificadorMedicos(reservMeHo->derecha, consultorio, DiaNoche, BuffLunes, BuffMartes, BuffMiercoles, BuffJueves, BuffViernes, hWnd);

		}
	}
	void images(Medico*& reserv, char ValidacionMedicos2[MAX_PATH], HWND hWnd) {
		if (reserv != NULL)
		{
			images(reserv->izquierda, ValidacionMedicos2, hWnd);

			if (strcmp(ValidacionMedicos2, reserv->nombre) == 0) {

				HBITMAP hBmpImg;
				hBmpImg = (HBITMAP)LoadImage(NULL, reserv->chFileName, IMAGE_BITMAP, 150, 150, LR_LOADFROMFILE);
				SendMessage(GetDlgItem(hWnd, BTM_ImgMedCon), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpImg);
			}
			images(reserv->derecha, ValidacionMedicos2, hWnd);

		}
	}
	void estatic3(Medico*& reserv, char ValidacionMedico2[MAX_PATH],HWND hWnd) {
		if (reserv != NULL)
		{
			estatic3(reserv->izquierda,ValidacionMedico2,hWnd);

			if (strcmp(ValidacionMedico2, reserv->nombre) == 0) {
				if (strcmp(reserv->horario,"1") == 0) {
					validaciones(CB_Minutos, hWnd, MOSTRAR);
					validaciones(CB_Hora, hWnd, MOSTRAR);
					validaciones(EST_HORAB, hWnd, MOSTRAR);
					validaciones(EST_MINUTOSB, hWnd, MOSTRAR);
					validaciones(EST_HORARIORE, hWnd, MOSTRAR);
					validaciones(EST_DIA, hWnd, MOSTRAR);
					validaciones(EST_TURNO, hWnd, MOSTRAR);
					validaciones(EST_HORARIOMANANA, hWnd, MOSTRAR);

					validaciones(EST_HORARIOTARDE, hWnd, ESCONDER);
					validaciones(EST_NOCHE, hWnd, ESCONDER);


				}
				else if (strcmp(reserv->horario, "2") == 0) {
					validaciones(CB_Minutos, hWnd, MOSTRAR);
					validaciones(CB_Hora, hWnd, MOSTRAR);
					validaciones(EST_HORAB, hWnd, MOSTRAR);
					validaciones(EST_MINUTOSB, hWnd, MOSTRAR);
					validaciones(EST_HORARIORE, hWnd, MOSTRAR);
					validaciones(EST_NOCHE, hWnd, MOSTRAR);
					validaciones(EST_DIA, hWnd, ESCONDER);
					validaciones(EST_HORARIOMANANA, hWnd, ESCONDER);
					validaciones(EST_NOCHE, hWnd, MOSTRAR);
					validaciones(EST_HORARIOTARDE, hWnd, MOSTRAR);
					validaciones(EST_TURNO, hWnd, MOSTRAR);

				
				}

			}
			estatic3(reserv->derecha, ValidacionMedico2, hWnd);

		}
	}
	void estatic2(Medico*& reserv, HWND SemanaESTA, char ValidacionMedico2[MAX_PATH],int dia) {
		if (reserv != NULL)
		{
			estatic2(reserv->izquierda, SemanaESTA, ValidacionMedico2,dia);

			if (strcmp(ValidacionMedico2, reserv->nombre) == 0) {
				if (dia == LUNES) {
					if (reserv->Lunes == true) {
						SetWindowText(SemanaESTA, "SI");
					}
					else {
						SetWindowText(SemanaESTA, "No");
					}
				}
				if (dia == MARTES) {
					if (reserv->Martes == true) {
						SetWindowText(SemanaESTA, "SI");
					}
					else {
						SetWindowText(SemanaESTA, "No");
					}
				}
				if (dia == MIERCOLES) {
					if (reserv->Miercoles == true) {
						SetWindowText(SemanaESTA, "SI");
					}
					else {
						SetWindowText(SemanaESTA, "No");
					}
				}
				if (dia == JUEVES) {
					if (reserv->Jueves == true) {
						SetWindowText(SemanaESTA, "SI");
					}
					else {
						SetWindowText(SemanaESTA, "No");
					}
				}
				if (dia == VIERNES) {
					if (reserv->Viernes == true) {
						SetWindowText(SemanaESTA, "SI");
					}
					else {
						SetWindowText(SemanaESTA, "No");
					}
				}
				
			}
			estatic2(reserv->derecha, SemanaESTA, ValidacionMedico2,dia);

		}
	}

	void estatic(Medico*& reserv, HWND ConsultorioESTA, char ValidacionMedico2[MAX_PATH]) {
		char uno[MAX_PATH] = "1";
		char dos[MAX_PATH] = "2";
		char tres[MAX_PATH] = "3";
		if (reserv != NULL)
		{
			estatic(reserv->izquierda, ConsultorioESTA, ValidacionMedico2);

			if (strcmp(ValidacionMedico2, reserv->nombre) == 0) {
				if (strcmp(uno, reserv->numconsultorio) == 0) {
					SetWindowText(ConsultorioESTA, reserv->numconsultorio);
				}
				else if (strcmp(dos, reserv->numconsultorio) == 0) {
					SetWindowText(ConsultorioESTA, reserv->numconsultorio);
				}
				else if (strcmp(tres, reserv->numconsultorio) == 0) {
					SetWindowText(ConsultorioESTA, reserv->numconsultorio);
				}
			}
			estatic(reserv->derecha, ConsultorioESTA, ValidacionMedico2);
		}
	}

	void combo(Medico *reserv, HWND Primer) {
		if (reserv != NULL) 
		{
			combo(reserv->izquierda, Primer);
			SendMessage(Primer, CB_ADDSTRING, NULL, (LPARAM)reserv->nombre);
			combo(reserv->derecha, Primer);
		}
	};
	void comboCedula(Medico* reserv, HWND Primer) {
		if (reserv != NULL)
		{
				comboCedula(reserv->izquierda, Primer);
				SendMessage(Primer, CB_ADDSTRING, NULL, (LPARAM)reserv->cedula);
				comboCedula(reserv->derecha, Primer);
			
		}
	};
	void combo2(Medico*& reserv, HWND Primer, char especialidad[MAX_PATH]) {
		if (reserv != NULL)
		{
			combo2(reserv->izquierda, Primer, especialidad);
			if (strcmp(especialidad, reserv->especialidad) == 0) {
				SendMessage(Primer, CB_ADDSTRING, NULL, (LPARAM)reserv->nombre);
			}
			combo2(reserv->derecha, Primer, especialidad);
		}
	}
	
	

	Medico* minimo(Medico* arbol) {
		if (arbol == NULL) {
			return NULL;
		}
		if (arbol->izquierda) {
			return minimo(arbol->izquierda);
		}
		else {
			return arbol;
		}
	}

	void remplazar(Medico* arbol, Medico* nuevoNodo) {
		if (arbol->previo) {
			if (arbol->previo->izquierda != NULL) {
				if (arbol->IntCedula == arbol->previo->izquierda->IntCedula) {
					arbol->previo->izquierda = nuevoNodo;

				}
			}
			if (arbol->previo->derecha != NULL) {
				if (arbol->IntCedula == arbol->previo->derecha->IntCedula) {
					arbol->previo->derecha = nuevoNodo;
				}
			}
		}
		if (nuevoNodo) {
			nuevoNodo->previo = arbol->previo;
		}
		
	}
		
	void eliminarMedico(Medico* MedicoEliminar) {
		if (MedicoEliminar->previo == NULL) {
			if (MedicoEliminar->derecha == NULL && MedicoEliminar->izquierda == NULL) {
				raiz->previo = NULL;
				raiz = NULL;

				delete MedicoEliminar;
			}
			else if (MedicoEliminar->izquierda != NULL) {
				//raiz = MedicoEliminar->izquierda;
				remplazar(raiz, MedicoEliminar->izquierda);
				raiz->previo = NULL;

				delete MedicoEliminar;

			}
			else if(MedicoEliminar->derecha != NULL){
				raiz = MedicoEliminar->derecha;
				remplazar(raiz, MedicoEliminar->derecha);
				raiz->previo = NULL;
				delete MedicoEliminar;
			}
			
		}	
		else if (MedicoEliminar->izquierda && MedicoEliminar->derecha) {
			Medico* menor = minimo(MedicoEliminar->derecha);
			MedicoEliminar->IntCedula = menor->IntCedula;
			eliminarMedico(menor);
		}
		else if (MedicoEliminar->izquierda) {
			remplazar(MedicoEliminar, MedicoEliminar->izquierda);
			MedicoEliminar = NULL;
			delete MedicoEliminar;

			
		}
		else if (MedicoEliminar->derecha) {
			remplazar(MedicoEliminar, MedicoEliminar->derecha);
			delete MedicoEliminar;
		}
		else
		{
			remplazar(MedicoEliminar, NULL);
			MedicoEliminar = NULL;
			delete MedicoEliminar;
		}
	}


	void Eliminar(Medico* arbol, int cedula) {
		if (arbol == NULL) {
			return;
		}
		else if (cedula < arbol->IntCedula) {
			Eliminar(arbol->izquierda, cedula);
		}
		else if (cedula > arbol->IntCedula) {
			Eliminar(arbol->derecha, cedula);
		}
		else {
			eliminarMedico(arbol);
		}
	}
};
Arbolito* AMedicos;



void DatosMedicosREGISTROS(Registros*& registro, Medico*& medico, Paciente*& paciente, char numeroCita[MAX_PATH], HWND hWnd) {
	HWND ConsultoriosREGISTRO = GetDlgItem(hWnd, INF_CONSULTORIOREGISTRO);
	if (medico != NULL) {
		DatosMedicosREGISTROS(registro, medico->izquierda, paciente, numeroCita, hWnd);
		if (strcmp(medico->nombre, registro->nombreMedico) == 0) {
			SetWindowText(ConsultoriosREGISTRO, medico->numconsultorio);
			HBITMAP hBmpImg;
			hBmpImg = (HBITMAP)LoadImage(NULL, medico->chFileName, IMAGE_BITMAP, 150, 150, LR_LOADFROMFILE);
			SendMessage(GetDlgItem(hWnd, BTM_IMAGENTEGISTRO), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpImg);
		}

		DatosMedicosREGISTROS(registro, medico->derecha, paciente, numeroCita, hWnd);
	}
}

void TelefonoPacienteREGISTROS(Registros*& registro, Medico*& medico, Paciente*& paciente, char numeroCita[MAX_PATH], HWND hWnd) {
	HWND TelefonoREGISTRO = GetDlgItem(hWnd, INF_TELEFONOREGISTRO);
	HWND ApellidoREGISTRO = GetDlgItem(hWnd, INFAPELLIDOSPACIENTE);
	flag = false;
	while (paciente!=NULL) {
		if (strcmp(registro->paciente, paciente->nombre) == 0) {
			SetWindowText(TelefonoREGISTRO, paciente->Telefono);
			SetWindowText(ApellidoREGISTRO, paciente->apellido);

			return;
		}
		else {
			flag = false;
			paciente = paciente->next;
		}
	}
}


void registrosFINAL(Registros*& registro, Medico*& medico, Paciente*& paciente, char numeroCita[MAX_PATH], HWND hWnd) {
	HWND HoraREGISTRO = GetDlgItem(hWnd, INF_HORAREGISTRO);
	HWND FechaREGISTRO = GetDlgItem(hWnd, INF_FECHAREGISTRO);
	HWND MinutosREGISTRO = GetDlgItem(hWnd, INF_MINUTOSREGISTRO);
	HWND EspecialidadREGISTRO = GetDlgItem(hWnd, INF_ESPECIALIDADREGISTRO);
	HWND MedicoREGISTRO = GetDlgItem(hWnd, INF_MEDICOREGISTRO);
	HWND EstadoConsultaREGISTRO = GetDlgItem(hWnd, INF_ESTADOREGISTRO);
	HWND ApellidosREGISTRO = GetDlgItem(hWnd, INFAPELLIDOSPACIENTE);
	HWND PacienteREGISTRO = GetDlgItem(hWnd, INF_PACIENTEREGISTRO);


		flag = false;
	while (flag==false) {
		if (strcmp(numeroCita, registro->NumeroCitas) == 0) {
			SetWindowText(FechaREGISTRO, registro->Fecha);
			SetWindowText(HoraREGISTRO, registro->hora);
			SetWindowText(MinutosREGISTRO, registro->minutos);
			SetWindowText(EspecialidadREGISTRO, registro->especialidad);
			SetWindowText(PacienteREGISTRO, registro->paciente);
			SetWindowText(MedicoREGISTRO, registro->nombreMedico);
			SetWindowText(ApellidosREGISTRO, registro->apellido);
			if (registro->EstadoConsulta) {
				SetWindowText(EstadoConsultaREGISTRO, "Terminada");
			}
			else {
				SetWindowText(EstadoConsultaREGISTRO, "En Revisión");
			}
			TelefonoPacienteREGISTROS(registro, medico, paciente, numeroCita, hWnd);
			
			DatosMedicosREGISTROS(registro, medico, paciente, numeroCita, hWnd);
				flag = true;
		}
		else {
			registro=registro->next;
			flag = false;
		}
	}
}



HWND hVentanaPrincipal;
HINSTANCE hGlobalInst;
bool CIERRE = false;

BOOL CALLBACK AltasBajasCambios(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegistroMedicos(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegistroPacientes(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegistroCitas(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Especialidades(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SemanaMes(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK BusquedaMedicoMes(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EspecialidadSemanaMes(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegistroConsultasFinal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK IniciarSesion(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND pacientes, htexnombre;


int WINAPI WinMain(HINSTANCE hInst,
	HINSTANCE hPrevInstance,
	PSTR cmdLine, int showCmd)
{
	Pacientes = new ListaLigadas;
	AMedicos = new Arbolito;
	Registro = new ListaLigadasRegistros;

	hVentanaPrincipal = CreateDialog(
		hInst,
		MAKEINTRESOURCE(SDLG_INCIARSESION),
		NULL,
		IniciarSesion


	);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	ShowWindow(hVentanaPrincipal, SW_SHOW);

	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//strcpy_s(arbol.nombre, "Marco David Castillo Cantú");

	delete Registro;
	delete Pacientes;
	delete AMedicos;
	return msg.wParam;
}
bool DESACTIVADO = false;

BOOL CALLBACK RegistroMedicos(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	static char chFileName[MAX_PATH] = "";


	switch (msg) {


	case WM_INITDIALOG: {
		Especialidad* reserva = originE;
		HWND  PrimeraEspecialidad = GetDlgItem(hWnd, CB_Especialidades);
	
		while (reserva != NULL) {
			SendMessage(PrimeraEspecialidad, CB_ADDSTRING, NULL, (LPARAM)reserva->especialidad);
			reserva = reserva->next;
		}
	}
					  break;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_GuardarSALIR && HIWORD(wParam) == BN_CLICKED) {
			ContadorRegistroGuardar();

			Medico* reservaME = raiz;
			AMedicos->guardar(reservaME, hWnd);

			Especialidad* ReservaME = originE;
			GuardarEspecialidades(ReservaME,hWnd);

			reservaME = raiz;
			AMedicos->GuardarMedico(reservaME,hWnd);

			Paciente* reservaPara = originP;
			Pacientes->guardadoPaciente(reservaPara, hWnd);

			reservaPara = originP;
			Pacientes->GuardarPacientesBIN(reservaPara,hWnd);

			Registros* reservaRe = originR;
			Registro->GuardarRegistro(reservaRe,hWnd);

			MessageBox(hWnd, "Se guardaron los datos", "GUARDAR Y SALIR", MB_OK);
			PostQuitMessage(10);

		}
		if (LOWORD(wParam) == ID_BAJASCAMBIOS && HIWORD(wParam) == BN_CLICKED) {
			HWND BajasCambios = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
			ShowWindow(BajasCambios, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ConsultaCitas2 && HIWORD(wParam) == BN_CLICKED) {
			HWND Consultadecitas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_RegistroCita), NULL, RegistroConsultasFinal);
			ShowWindow(Consultadecitas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_MEDICOYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND ConsultasMedicoMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SGDL_CitaMedico), NULL, BusquedaMedicoMes);
			ShowWindow(ConsultasMedicoMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_SEMANAYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ConsultaSemanaMes), NULL, SemanaMes);
			ShowWindow(BusquedaSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_ESPECIALIDADYSEMANADELMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaEspecialidadSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_EspecialidadSemanaMes), NULL, EspecialidadSemanaMes);
			ShowWindow(BusquedaEspecialidadSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTRODEPACIENTES && HIWORD(wParam) == BN_CLICKED) {
			HWND Pacientes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROPACIENTE), NULL, RegistroPacientes);
			ShowWindow(Pacientes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ESPECIALIDAD && HIWORD(wParam) == BN_CLICKED) {
			HWND Especialidad = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ESPECIALIDADES), NULL, Especialidades);
			ShowWindow(Especialidad, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		; //Aquí depositaremos la dirección del archiv

		if (LOWORD(wParam) == BTN_CargarImagen && HIWORD(wParam) == BN_CLICKED) {
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lpstrFilter = "Bit Map Images (*.bmp)\0*.bmp\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"; //Filtramos los formatos de archivo
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = chFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.hwndOwner = hWnd;
			ofn.lpstrDefExt = "txt";
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
			if (GetOpenFileName(&ofn)) {
				//MessageBox(NULL, chFileName, "Archivo", MB_OK); //Checamos la dirección en un MessageBox
				static HBITMAP hBmpImg;
				hBmpImg = (HBITMAP)LoadImage(NULL, chFileName, IMAGE_BITMAP, 150, 150, LR_LOADFROMFILE);
				SendMessage(GetDlgItem(hWnd, BTM_ImageMedic), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpImg);
			}
		}
		if (LOWORD(wParam) == BTN_CargarInformacion && HIWORD(wParam) == BN_CLICKED) {
			if (DESACTIVADO == false) {
				ContadorRegistroCargar();
				CargarEspecialidades();
				AMedicos->cargarMedicosBIN();
				Pacientes->CargarPacientes();
				Registro->CargarRegistro();
				DESACTIVADO = true;
				HWND MedicosNUEVOS = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
				ShowWindow(MedicosNUEVOS, SW_SHOW);
				//CIERRE = true;
				DestroyWindow(hWnd);
			}
			else {
				MessageBox(hWnd, "Ya se dio de alta la informacion", "CARGAR INFORMACION", MB_OK);
			}
			
			//validaciones(BTN_CargarInformacion, hWnd, DESACTIVAR);

		}
		if (LOWORD(wParam) == BTN_DarAltaMedic && HIWORD(wParam) == BN_CLICKED) {

			/////////////Nombre Medico
			HWND NomMedico = GetDlgItem(hWnd, TEX_NombreMedico);
			int length = (int)GetWindowTextLength(NomMedico);
			char NombreMedico[MAX_PATH];
			GetWindowTextA(NomMedico, NombreMedico, length + 1);
			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en la sección ""Nombre del medico""", "Nombre del Medico", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag == true) {
				for (int i = 0; i < length; i++) {
					if ((NombreMedico[i] < 65 || NombreMedico[i] > 90) && (NombreMedico[i] < 97 || NombreMedico[i] > 122)) {
						flag = false;
					}
					else {
						flag = true;
					}
				}
				if (flag == false)
					MessageBox(hWnd, "Coloca solo letras en el apartado de Numero de Medicos", "Nombre del Medico", MB_OK);
			}
			///////////////Cedula
			char NumeroCedula[MAX_PATH];
			int Cedulaint;
			if (flag == true) {
				HWND NumCedula = GetDlgItem(hWnd, TEX_Cedula);
				int length = (int)GetWindowTextLength(NumCedula);
				GetWindowTextA(NumCedula, NumeroCedula, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Numero de Cedula""", "Nombre del Medico", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
				if (flag == true) {
					for (int i = 0; i < length; i++) {
						if (NumeroCedula[i] > 57 || NumeroCedula[i] < 48) {
							flag = false;
						}
						else {
							flag = true;
					
						}
					}
					if (flag == false)
						MessageBox(hWnd, "Coloca solo numero en el apartado de  Cedula", "Numero de Cedula", MB_OK);
				}
			}
			Cedulaint = atoi(NumeroCedula);
			///////////////Especialidades
			char Especialidad2[MAX_PATH];
			if (flag) {
				length = 0;
				HWND Especialidades = GetDlgItem(hWnd, CB_Especialidades);
				int length = (int)GetWindowTextLength(Especialidades);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Especialidad""", "Tipo de Especialidad", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}

				if (flag) {
					GetWindowText(Especialidades, (LPSTR)Especialidad2, length + 1);
				}
			}
			//////////////Consultorios
			char Consultorio[MAX_PATH];
			if (flag) {
				if (IsDlgButtonChecked(hWnd, RD_Consultorio1) == BST_CHECKED || IsDlgButtonChecked(hWnd, RD_Consultorio2) == BST_CHECKED || IsDlgButtonChecked(hWnd, RD_Consultorio3) == BST_CHECKED) {


					if (IsDlgButtonChecked(hWnd, RD_Consultorio1) == BST_CHECKED) {
						strcpy_s(Consultorio, "1");
						flag = true;
					}
					else {

						if (IsDlgButtonChecked(hWnd, RD_Consultorio2) == BST_CHECKED) {
							strcpy_s(Consultorio, "2");
							flag = true;
						}
						else {
							if (IsDlgButtonChecked(hWnd, RD_Consultorio3) == BST_CHECKED) {
								strcpy_s(Consultorio, "3");
								flag = true;
							}
							else {
								flag = false;
							}
						}
					}
				}
				else {
					flag = false;
					MessageBox(hWnd, "No se coloco nada en la sección ""Numero de Consultorio""", "Nombre del Consultorio", MB_OK);

				}
			}
			////////////Telefono
			char NumeroTelefono[MAX_PATH];
			if (flag) {
				length = 0;
				HWND NumTel = GetDlgItem(hWnd, TEX_Telefono);
				int length = (int)GetWindowTextLength(NumTel);
				GetWindowTextA(NumTel, NumeroTelefono, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Numero de Telefono""", "Telefono del Medico", MB_OK);
					flag = false;
				}
				else {
					if (length == 8 || length == 16) {
						flag = true;
					}
					else {

						MessageBox(hWnd, "Debe colocarse más de 8 o 15 numeros en el espacio de telefono", "Telefono del Medico", MB_OK);
						flag = false;
					}
				}

				if (flag) {
					for (int i = 0; i < length; i++) {
						if (NumeroTelefono[i] > 57 || NumeroTelefono[i] < 48) {
							flag = false;
						}
						else {
							flag = true;
						}
					}
					if (flag == false)
						MessageBox(hWnd, "Coloca solo numero en el apartado de Telefono", "Numero de Telefono", MB_OK);

				}
			}
			////////////////Horario
			char BuffHorario[MAX_PATH];
			if (flag) {
				length = 0;
				if (IsDlgButtonChecked(hWnd, RD_Manana) == BST_CHECKED || IsDlgButtonChecked(hWnd, RD_Tarde) == BST_CHECKED) {
					if (IsDlgButtonChecked(hWnd, RD_Manana) == BST_CHECKED) {
						strcpy_s(BuffHorario, "1");
						flag = true;
					}
					else {
						if (IsDlgButtonChecked(hWnd, RD_Tarde) == BST_CHECKED) {
							flag = true;
							strcpy_s(BuffHorario, "2");
						}
						else {
							flag = false;
						}
					}
				}
				else {
					flag = false;
					MessageBox(hWnd, "No se selecciono ninguna opción en horario del medico", "Medicos", MB_OK);
				}
			}
			/////////////////Semana
			bool BuffLunes = false;
			bool BuffMartes = false;
			bool BuffMiercoles = false;
			bool BuffJueves = false;
			bool BuffViernes = false;



			if (flag) {
				length = 0;
				if (IsDlgButtonChecked(hWnd, CHK_Lunes) == BST_CHECKED || IsDlgButtonChecked(hWnd, CHK_Martes) == BST_CHECKED || IsDlgButtonChecked(hWnd, CHK_Miercoles) == BST_CHECKED || IsDlgButtonChecked(hWnd, CHK_Jueves) == BST_CHECKED || IsDlgButtonChecked(hWnd, CHK_Viernes) == BST_CHECKED){
					if (IsDlgButtonChecked(hWnd, CHK_Lunes) == BST_CHECKED) {
						BuffLunes = true;
						flag = true;
					}
					if (IsDlgButtonChecked(hWnd, CHK_Martes) == BST_CHECKED) {
						
							flag = true;
							BuffMartes = true;
					}
					if (IsDlgButtonChecked(hWnd, CHK_Miercoles) == BST_CHECKED) {

						flag = true;
						BuffMiercoles = true;
					}
					if (IsDlgButtonChecked(hWnd, CHK_Jueves) == BST_CHECKED) {

						flag = true;
						BuffJueves = true;
					}
					if (IsDlgButtonChecked(hWnd, CHK_Viernes) == BST_CHECKED) {

						flag = true;
						BuffViernes = true;
					}
				}
				else {
					flag = false;
					MessageBox(hWnd, "No se selecciono ninguna opción en días de la semana", "Medicos", MB_OK);
				}
			}
			if (flag) {
				Medico* reservMeHo = raiz;
				AMedicos->identificadorMedicos(reservMeHo,Consultorio, BuffHorario, BuffLunes, BuffMartes, BuffMiercoles, BuffJueves, BuffViernes,hWnd);
				contadorM++;

			}
///////////////////////////////////////////////
			if (flag) {
				
				if (strcmp(chFileName,"")!=0) {
					flag = true;

				}
				else
				{
					MessageBox(hWnd, "Hace falta dar de alta una imagen", "Medicos", MB_OK);

					flag = false;
				}
			}
			////////////////Mensaje final
			if (flag) {
				strcpy_s(arbol.nombre,250, NombreMedico);
				strcpy_s(arbol.especialidad, 250, Especialidad2);
				strcpy_s(arbol.numconsultorio, Consultorio);
				strcpy_s(arbol.telefono, 250, NumeroTelefono);
				strcpy_s(arbol.chFileName,250,chFileName);
				strcpy_s(arbol.horario, 250, BuffHorario);
				strcpy_s(arbol.cedula, 250, NumeroCedula);
				arbol.Lunes = BuffLunes;
				arbol.Martes = BuffMartes;
				arbol.Miercoles = BuffMiercoles;
				arbol.Jueves = BuffJueves;
				arbol.IntCedula = Cedulaint;
				arbol.Viernes = BuffViernes;
				AMedicos->insertarMedico(raiz,arbol);
				char vacio[MAX_PATH] = "";
				strcpy_s(Consultorio,"");
				strcpy_s(chFileName, 250, vacio);
				strcpy_s(BuffHorario, "0");
				BuffLunes = false;
					BuffMartes = false;
					BuffMiercoles = false;
					BuffJueves = false;
					BuffViernes = false;
			}
			else {
				flag = false;
			}
			if (flag)
				MessageBox(hWnd, "Se dio de alta al medico", "Medicos", MB_OK);
		

		}
		if (LOWORD(wParam) == ID_REGISTROCITAS && HIWORD(wParam) == BN_CLICKED) {
			HWND Citas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROCONSULTAS), NULL, RegistroCitas);
			ShowWindow(Citas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
	
		break;
	
	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);
		break;
	}
	return FALSE;

}

BOOL CALLBACK RegistroPacientes(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		
	case WM_INITDIALOG: {
		Medico* reserva = raiz;
		HWND PrimerMedic = GetDlgItem(hWnd, CB_PrimerMedic);
		AMedicos->combo(reserva, PrimerMedic);
	}
	break;
	

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_GuardarSALIR && HIWORD(wParam) == BN_CLICKED) {
			ContadorRegistroGuardar();

			Medico* reservaME = raiz;
			AMedicos->guardar(reservaME, hWnd);

			Especialidad* ReservaME = originE;
			GuardarEspecialidades(ReservaME, hWnd);

			reservaME = raiz;
			AMedicos->GuardarMedico(reservaME, hWnd);

			Paciente* reservaPara = originP;
			Pacientes->guardadoPaciente(reservaPara, hWnd);

			reservaPara = originP;
			Pacientes->GuardarPacientesBIN(reservaPara, hWnd);

			Registros* reservaRe = originR;
			Registro->GuardarRegistro(reservaRe, hWnd);

			MessageBox(hWnd, "Se guardaron los datos", "GUARDAR Y SALIR", MB_OK);
			PostQuitMessage(10);

		}
		if (LOWORD(wParam) == ID_BAJASCAMBIOS && HIWORD(wParam) == BN_CLICKED) {
			HWND BajasCambios = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
			ShowWindow(BajasCambios, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_ESPECIALIDADYSEMANADELMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaEspecialidadSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_EspecialidadSemanaMes), NULL, EspecialidadSemanaMes);
			ShowWindow(BusquedaEspecialidadSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_MEDICOYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND ConsultasMedicoMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SGDL_CitaMedico), NULL, BusquedaMedicoMes);
			ShowWindow(ConsultasMedicoMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_SEMANAYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ConsultaSemanaMes), NULL, SemanaMes);
			ShowWindow(BusquedaSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROSMEDICOS && HIWORD(wParam) == BN_CLICKED) {
			HWND Medicos = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
			ShowWindow(Medicos, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ConsultaCitas2 && HIWORD(wParam) == BN_CLICKED) {
			HWND Consultadecitas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_RegistroCita), NULL, RegistroConsultasFinal);
			ShowWindow(Consultadecitas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ESPECIALIDAD && HIWORD(wParam) == BN_CLICKED) {
			HWND Especialidad = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ESPECIALIDADES), NULL, Especialidades);
			ShowWindow(Especialidad, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROCITAS && HIWORD(wParam) == BN_CLICKED) {
			HWND Citas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROCONSULTAS), NULL, RegistroCitas);
			ShowWindow(Citas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
////////Nombre_Paciente
		int length;
		if (LOWORD(wParam) == BTN_DarAltaPaciente && HIWORD(wParam) == BN_CLICKED) {
			char NombrePaciente[MAX_PATH];
			char ApellidoPaciente[MAX_PATH];
			HWND apePaciente = GetDlgItem(hWnd, TEX_Pacientes2);
			int length = (int)GetWindowTextLength(apePaciente);
			GetWindowTextA(apePaciente, ApellidoPaciente, length + 1);
			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en la sección ""Apellido del Paciente""", "Apellido del Paciente", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {
				for (int i = 0; i < length; i++) {
					if ((ApellidoPaciente[i] < 65 || ApellidoPaciente[i] > 90) && (ApellidoPaciente[i] < 97 || ApellidoPaciente[i] > 122)) {
						flag = false;
					}
					else {
						flag = true;
					}
				}
				if (flag == false) {
					MessageBox(hWnd, "Coloca solo letras en el apartado de Nombre de paciente", "Nombre de Paciente", MB_OK);
				}
			}
			if (flag) {
				HWND NomPaciente = GetDlgItem(hWnd, TEX_Pacientes);
				int length = (int)GetWindowTextLength(NomPaciente);
				GetWindowTextA(NomPaciente, NombrePaciente, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Nombre del Paciente""", "Nombre del Paciente", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
				if (flag) {
					for (int i = 0; i < length; i++) {
						if ((NombrePaciente[i] < 65 || NombrePaciente[i] > 90) && (NombrePaciente[i] < 97 || NombrePaciente[i] > 122)) {
							flag = false;
						}
						else {
							flag = true;
						}
					}
					if (flag == false) {
						MessageBox(hWnd, "Coloca solo letras en el apartado de Nombre de paciente", "Nombre de Paciente", MB_OK);
					}
				}
			}
/////////////////Genero
			bool BuffGenero;

			if (flag) {
				if (flag) {
					length = 0;
					if (IsDlgButtonChecked(hWnd, RD_Masculino) == BST_CHECKED || IsDlgButtonChecked(hWnd, RD_Femenino) == BST_CHECKED) {
						if (IsDlgButtonChecked(hWnd, RD_Masculino) == BST_CHECKED) {
							BuffGenero = true;
							flag = true;
						}
						else {
							if (IsDlgButtonChecked(hWnd, RD_Femenino) == BST_CHECKED) {
								flag = true;
								BuffGenero = false;
							}
							else {
								flag = false;
							}
						}
					}
					else {
						flag = false;
						MessageBox(hWnd, "No se selecciono ninguna opción de genero", "Paciente Genero", MB_OK);
					}
				}
			}
////////////Telefono_Paciente
			char NumeroTelefonoPaciente[MAX_PATH];
			if (flag) {
				length = 0;
				HWND NumTelPaciente = GetDlgItem(hWnd, TEX_TelefonoPaciente);
				int length = (int)GetWindowTextLength(NumTelPaciente);
				GetWindowTextA(NumTelPaciente, NumeroTelefonoPaciente, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Numero de Telefono""", "Telefono del Paciente", MB_OK);
					flag = false;
				}
				else {
					if (length == 8 || length == 16) {
						flag = true;
					}
					else {

						MessageBox(hWnd, "Debe colocarse más de 8 o 15 numeros en el espacio de telefono", "Telefono del Paciente", MB_OK);
						flag = false;
					}
				}

				if (flag) {
					for (int i = 0; i < length; i++) {
						if (NumeroTelefonoPaciente[i] > 57 || NumeroTelefonoPaciente[i] < 48) {
							flag = false;
						}
						else {
							flag = true;
						}
					}
					if (flag == false)
						MessageBox(hWnd, "Coloca solo numeros en el apartado de Telefono", "Numero de Telefono", MB_OK);

				}
			}
////////////EDAD
			char EdadPaciente[MAX_PATH] = "";
			if (flag) {
				length = 0;
				HWND EDAD = GetDlgItem(hWnd, TEX_Edad);
				int length = (int)GetWindowTextLength(EDAD);
				GetWindowTextA(EDAD, EdadPaciente, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""EDAD""", "Telefono del Paciente", MB_OK);
					flag = false;
				}
				else {
					int edadEntera = atoi(EdadPaciente);
					if (edadEntera > 0 && edadEntera <= 99) {
						flag = true;
					}
					else {

						MessageBox(hWnd, "Solo se le dara Consulta a personas 1-99 años", "Telefono del Paciente", MB_OK);
						flag = false;
					}
				}

				if (flag) {
					for (int i = 0; i < length; i++) {
						if (EdadPaciente[i] > 57 || EdadPaciente[i] < 48) {
							flag = false;
						}
						else {
							flag = true;
						}
					}
					if (flag == false)
						MessageBox(hWnd, "Coloca solo numeros en el apartado de Telefono", "Numero de Telefono", MB_OK);

				}
			}
///////////Fecha
			char BufferNacimiento[MAX_PATH] = "";
			if (flag) {
				length = 0;
				HWND FechaNacimiento = GetDlgItem(hWnd, TEX_FechaPaciente);
				length = (int)GetWindowTextLength(FechaNacimiento);
				GetWindowText(FechaNacimiento, BufferNacimiento, length + 1);
			}
	
			char BufferMedico[MAX_PATH] = "";
			if (flag) {
				length = 0;
				HWND  MedicoPaciente = GetDlgItem(hWnd, CB_PrimerMedic);
				length = (int)GetWindowTextLength(MedicoPaciente);
				GetWindowText(MedicoPaciente, BufferMedico, length + 1);

				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en Medico que le atendio", "PACIENTES", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
			}
///////////Descripcíon
			char BufferDescripcion[MAX_PATH] = "";
			if (flag) {
				length = 0;
				HWND Descripcion = GetDlgItem(hWnd, TEX_Descripcion);
				length = (int)GetWindowTextLength(Descripcion);
				GetWindowText(Descripcion, BufferDescripcion, length + 1);

				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en Descripcion", "PACIENTES", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
			}
////////////////Mensaje final
		
///////////Lista_LIGADA
			if (flag) {

				Pacientes->darAlta(originP, auxP);
				auxP->next = NULL;
				strcpy_s(auxP->nombre, NombrePaciente);
				auxP->genero = BuffGenero;
				strcpy_s(auxP->Telefono, NumeroTelefonoPaciente );
				strcpy_s(auxP->edad, EdadPaciente);
				strcpy_s(auxP->fechaNacimiento, BufferNacimiento);
				strcpy_s(auxP->MedicoRecomendado, BufferMedico);
				strcpy_s(auxP->descripcion, BufferDescripcion);
				strcpy_s(auxP->apellido, ApellidoPaciente);
				contadorP++;


				MessageBox(hWnd, TEXT("Se dio de alta al paciente"), "Paciente", MB_OK);
			}
		}
		break;
	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);
		break;
	}
	return FALSE;

}
BOOL CALLBACK RegistroCitas(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int length = 0;
	Medico* reservaMe = raiz;

	switch (msg) {
	case WM_INITDIALOG: {

		Especialidad* reserva = originE;
		HWND  PrimeraEspecialidad = GetDlgItem(hWnd, CB_EspecialidadesConsul);
		while (reserva != NULL) {
			SendMessage(PrimeraEspecialidad, CB_ADDSTRING, NULL, (LPARAM)reserva->especialidad);
			reserva = reserva->next;
		}

		Paciente* reservaPacientes = originP;
		HWND  PrimeraPaciente = GetDlgItem(hWnd, CB_PacientesConsul);

		while (reservaPacientes != NULL) {
			SendMessage(PrimeraPaciente, CB_ADDSTRING, NULL, (LPARAM)reservaPacientes->nombre);
			reservaPacientes = reservaPacientes->next;
		}

		HWND Hora_Combo= GetDlgItem(hWnd, CB_Hora);
		char Horas[16][MAX_PATH]{ "06","07","08","09","10","11","12","13","14","15","16","17","18","19" };
		for (int i = 0; i < 14; i++) {
			SendMessage(Hora_Combo, CB_ADDSTRING, NULL, (LPARAM)Horas[i]);
	
		}

		HWND Minuto_Combo = GetDlgItem(hWnd, CB_Minutos);
		char Minutos[3][MAX_PATH]{ "00","20","40" };
		for (int i = 0; i < 3; i++) {
			SendMessage(Minuto_Combo, CB_ADDSTRING, NULL, (LPARAM)Minutos[i]);
		}
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_GuardarSALIR && HIWORD(wParam) == BN_CLICKED) {
			ContadorRegistroGuardar();

			Medico* reservaME = raiz;
			AMedicos->guardar(reservaME, hWnd);

			Especialidad* ReservaME = originE;
			GuardarEspecialidades(ReservaME, hWnd);

			reservaME = raiz;
			AMedicos->GuardarMedico(reservaME, hWnd);

			Paciente* reservaPara = originP;
			Pacientes->guardadoPaciente(reservaPara, hWnd);

			reservaPara = originP;
			Pacientes->GuardarPacientesBIN(reservaPara, hWnd);

			Registros* reservaRe = originR;
			Registro->GuardarRegistro(reservaRe, hWnd);

			MessageBox(hWnd, "Se guardaron los datos", "GUARDAR Y SALIR", MB_OK);
			PostQuitMessage(10);

		}
		if (LOWORD(wParam) == ID_BAJASCAMBIOS && HIWORD(wParam) == BN_CLICKED) {
			HWND BajasCambios = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
			ShowWindow(BajasCambios, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_ESPECIALIDADYSEMANADELMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaEspecialidadSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_EspecialidadSemanaMes), NULL, EspecialidadSemanaMes);
			ShowWindow(BusquedaEspecialidadSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ConsultaCitas2 && HIWORD(wParam) == BN_CLICKED) {
			HWND Consultadecitas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_RegistroCita), NULL, RegistroConsultasFinal);
			ShowWindow(Consultadecitas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROSMEDICOS && HIWORD(wParam) == BN_CLICKED) {
			HWND Medicos = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
			ShowWindow(Medicos, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_MEDICOYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND ConsultasMedicoMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SGDL_CitaMedico), NULL, BusquedaMedicoMes);
			ShowWindow(ConsultasMedicoMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_SEMANAYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ConsultaSemanaMes), NULL,SemanaMes);
			ShowWindow(BusquedaSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ESPECIALIDAD && HIWORD(wParam) == BN_CLICKED) {
			HWND Especialidad = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ESPECIALIDADES), NULL, Especialidades);
			ShowWindow(Especialidad, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}

		if (LOWORD(wParam) == ID_REGISTRODEPACIENTES && HIWORD(wParam) == BN_CLICKED) {
			HWND Pacientes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROPACIENTE), NULL, RegistroPacientes);
			ShowWindow(Pacientes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}

		if (LOWORD(wParam) == BTN_Despliegue && HIWORD(wParam) == BN_CLICKED) {
			char ValidacionEspecie[MAX_PATH];
				
				HWND ValidacionEspecialidades = GetDlgItem(hWnd, CB_EspecialidadesConsul);
				int length = (int)GetWindowTextLength(ValidacionEspecialidades);
				GetWindowText(ValidacionEspecialidades, (LPSTR)ValidacionEspecie, length + 1);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Especialidad""", "Tipo de Especialidad", MB_OK);
					flag = false;

				}
				else {
					flag = true;
					validaciones(CB_MedicoConsul,hWnd,MOSTRAR);
					validaciones(BTN_Despliegue2, hWnd, MOSTRAR);
					validaciones(BTN_Despliegue, hWnd, DESACTIVAR);



					HWND PrimerMedico = GetDlgItem(hWnd, CB_MedicoConsul);
					HWND EspecialidadESTA = GetDlgItem(hWnd, EST_ESPECIALIDAD);

					AMedicos->combo2(reservaMe, PrimerMedico, ValidacionEspecie);

					validaciones(CB_EspecialidadesConsul, hWnd , ESCONDER);
					SetWindowText(EspecialidadESTA, ValidacionEspecie);
					validaciones(EST_ESPECIALIDAD, hWnd, MOSTRAR);


				}

				if (flag) {
					GetWindowText(ValidacionEspecialidades, (LPSTR)ValidacionEspecie, length + 1);
				}
			}
		if (LOWORD(wParam) == BTN_Despliegue2 && HIWORD(wParam) == BN_CLICKED) {
			char ValidacionMedico2[MAX_PATH];

			HWND ValidacionMedico = GetDlgItem(hWnd, CB_MedicoConsul);
			int length = (int)GetWindowTextLength(ValidacionMedico);
			GetWindowText(ValidacionMedico, (LPSTR)ValidacionMedico2, length + 1);


			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en la sección ""Medico""", "Tipo de Especialidad", MB_OK);
				flag = false;

			}
			else {
				

				reservaMe=raiz;
				HWND ConsultorioESTA = GetDlgItem(hWnd, EST_Consultorio);

				AMedicos->estatic(reservaMe, ConsultorioESTA, ValidacionMedico2);
				reservaMe = raiz;
				HWND LunesESTA = GetDlgItem(hWnd, EST_Lunes);
				HWND MartesESTA = GetDlgItem(hWnd, EST_Martes);
				HWND MiercolesESTA = GetDlgItem(hWnd, EST_Miercoles);
				HWND JuevesESTA = GetDlgItem(hWnd, EST_Jueves);
				HWND ViernesESTA = GetDlgItem(hWnd, EST_Viernes);
				AMedicos->estatic2(reservaMe, LunesESTA, ValidacionMedico2,LUNES);
				reservaMe = raiz;

				AMedicos->estatic2(reservaMe, MartesESTA, ValidacionMedico2,MARTES);
				reservaMe = raiz;

				AMedicos->estatic2(reservaMe, MiercolesESTA, ValidacionMedico2,MIERCOLES);
				reservaMe = raiz;

				AMedicos->estatic2(reservaMe, JuevesESTA, ValidacionMedico2,JUEVES);
				reservaMe = raiz;

				AMedicos->estatic2(reservaMe, ViernesESTA, ValidacionMedico2, VIERNES);
				reservaMe = raiz;

				AMedicos->estatic3(reservaMe, ValidacionMedico2, hWnd);
				reservaMe = raiz;
				AMedicos->images(reservaMe, ValidacionMedico2, hWnd);
				
				validaciones(EST_Horario, hWnd, MOSTRAR);
				validaciones(EST_Consultorio, hWnd, MOSTRAR);
				validaciones(EST_InfoConsultorio, hWnd, MOSTRAR);
				validaciones(EST_InfoHorario, hWnd, MOSTRAR);
				validaciones(EST_Lunes, hWnd, MOSTRAR);
				validaciones(EST_Martes, hWnd, MOSTRAR);
				validaciones(EST_Miercoles, hWnd, MOSTRAR);
				validaciones(EST_Jueves, hWnd, MOSTRAR);
				validaciones(EST_Viernes, hWnd, MOSTRAR);
				validaciones(TEX_FechaConsul, hWnd, MOSTRAR);
				validaciones(EST_FechaRE, hWnd, MOSTRAR);
				validaciones(EST_Disponibilidad, hWnd, MOSTRAR);		
			}
		}

		if (LOWORD(wParam) == BTN_DarAltaCita && HIWORD(wParam) == BN_CLICKED) {
			char pacienteconsulta[MAX_PATH];

			length = 0;
			HWND Pacientes = GetDlgItem(hWnd, CB_PacientesConsul);
			int length = (int)GetWindowTextLength(Pacientes);


			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en la sección ""Paciente""", "Paciente", MB_OK);
				flag = false;

			}
			else {
				flag = true;
				GetWindowText(Pacientes, (LPSTR)pacienteconsulta, length + 1);

			}
			char EspecieConsulta[MAX_PATH];

			if (flag) {

				HWND ValidacionEspecialidades = GetDlgItem(hWnd, CB_EspecialidadesConsul);
				int length = (int)GetWindowTextLength(ValidacionEspecialidades);
				GetWindowText(ValidacionEspecialidades, (LPSTR)EspecieConsulta, length + 1);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Especialidad""", "Tipo de Especialidad", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}
			}
			char MedicoConsulta[MAX_PATH];

			if (flag) {

				HWND ValidacionMedico = GetDlgItem(hWnd, CB_MedicoConsul);
				int length = (int)GetWindowTextLength(ValidacionMedico);
				GetWindowText(ValidacionMedico, (LPSTR)MedicoConsulta, length + 1);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Medico""", "Nombre Medico", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}
			}
			char HoraConsul[MAX_PATH] = "";

			//////////////////////////////////HORA
			if (flag) {
				int length = 0;


				HWND ValidacionHora = GetDlgItem(hWnd, CB_Hora);
				length = (int)GetWindowTextLength(ValidacionHora);
				GetWindowText(ValidacionHora, (LPSTR)HoraConsul, length + 1);

				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Hora""", "Hora Cita", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}
				Medico* reservaHora = raiz;
				AMedicos->MananaNocheConsulta(reservaHora, MedicoConsulta, hWnd, HoraConsul);

			}
			char MinutoConsul[MAX_PATH]="";

			//////////////MINUTOS REGISTRO
			if (flag) {
				int lenght = 0;
				HWND ValidacionMinuto = GetDlgItem(hWnd, CB_Minutos);
				int length = (int)GetWindowTextLength(ValidacionMinuto);
				GetWindowText(ValidacionMinuto, (LPSTR)MinutoConsul, length + 1);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Minuto""", "Minutos Cita", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}

			}
			char BufferFECHALarga[MAX_PATH] = "";
			char BufferSEMANALarga[MAX_PATH] = "";
			char BufferDIALarga[MAX_PATH] = "";
			char BufferMESLarga[MAX_PATH] = "";
			char BufferANOLarga[MAX_PATH] = "";
			///////////////////////FECHA REGISTRO
			if (flag) {
				length = 0;

				int j = 0;
				int k = 0;
				int p = 0;
				HWND  FechaLarga = GetDlgItem(hWnd, TEX_FechaConsul);
				length = (int)GetWindowTextLength(FechaLarga);
				GetWindowText(FechaLarga, BufferFECHALarga, length + 1);
				for (int i = 0; i < length; i++) {
					if (BufferFECHALarga[i] != 44) {
						BufferSEMANALarga[i] = BufferFECHALarga[i];
					}
					else {
						i++;
						i++;
						for (i; i < length; i++) {
							for (j; j < 4; j++) {
								if (BufferFECHALarga[i] != 100 && BufferFECHALarga[i] != 32) {
									BufferDIALarga[j] = BufferFECHALarga[i];
									flag = false;
									i++;
								}
								else {
									flag = true;
								}
							}
							break;
						}
						if (flag) {
							i++;
							i++;
							i++;
							i++;
							for (k; k < length; k++) {
								if (BufferFECHALarga[i] != 32) {
									BufferMESLarga[k] = BufferFECHALarga[i];
									i++;
									flag = false;
								}
								else {
									flag = true;
								}
								if (flag) {
									k = length;
								}
							}
							if (flag) {
								i = i + 4;
								for (p; p < length; p++) {
									if (BufferFECHALarga[i] != 32) {
										BufferANOLarga[p] = BufferFECHALarga[i];
										i++;
										flag = true;
									}
									else {
										i = length;
										j = length;
										k = length;
										p = length;
									}
								}
							}
						}
					}
				}
			}
			if (flag) {
				flag = false;
				Medico* raizSE = raiz;
				AMedicos->IdentificadorSemana(raizSE, MedicoConsulta, hWnd, BufferSEMANALarga);
				

			}
			char numeroCitas[MAX_PATH] = "";
			if(flag){
			int lenght = 0;
			HWND ValidacionNUmCITAS = GetDlgItem(hWnd, TEX_NUMERODECITA);
			int length = (int)GetWindowTextLength(ValidacionNUmCITAS);
			GetWindowText(ValidacionNUmCITAS, (LPSTR)numeroCitas, length + 1);


			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en la sección ""Numero de cita""", "Minutos Cita", MB_OK);
				flag = false;

			}
			else {
				flag = true;
			}
			}
			bool flag2 = true;

			if (flag) {
				Registros* Reservadoo = originR;
				Registro->NoRepetirNUMERO(Reservadoo, hWnd, numeroCitas);
				if (flag == false) {
					MessageBox(hWnd, "Porfavor colocar otro numero este ya esta ingresado", "Numero de cita", MB_OK);
				}
			}

			if (flag) {

				Registro->darAlta(originR, auxR);
				auxR->next = NULL;
				strcpy_s(auxR->paciente, pacienteconsulta);
				strcpy_s(auxR->especialidad, EspecieConsulta);
				strcpy_s(auxR->nombreMedico, MedicoConsulta);
				strcpy_s(auxR->Fecha, BufferFECHALarga);
				strcpy_s(auxR->Ano, BufferANOLarga);
				strcpy_s(auxR->Dia, BufferDIALarga);
				strcpy_s(auxR->Semana, BufferSEMANALarga);
				strcpy_s(auxR->Mes, BufferMESLarga);
				strcpy_s(auxR->hora, HoraConsul);
				strcpy_s(auxR->minutos, MinutoConsul);
				strcpy_s(auxR->NumeroCitas, numeroCitas);
				contadorR++;
				auxR->contador = contadorR;

				MessageBox(hWnd, "Se dio de alta la cita", "Paciente", MB_OK);

			}
		}
		break;

	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);

		break;
	}
	return FALSE;

}
BOOL CALLBACK Especialidades(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_GuardarSALIR && HIWORD(wParam) == BN_CLICKED) {
			ContadorRegistroGuardar();

			Medico* reservaME = raiz;
			AMedicos->guardar(reservaME, hWnd);

			Especialidad* ReservaME = originE;
			GuardarEspecialidades(ReservaME, hWnd);

			reservaME = raiz;
			AMedicos->GuardarMedico(reservaME, hWnd);

			Paciente* reservaPara = originP;
			Pacientes->guardadoPaciente(reservaPara, hWnd);

			reservaPara = originP;
			Pacientes->GuardarPacientesBIN(reservaPara, hWnd);

			Registros* reservaRe = originR;
			Registro->GuardarRegistro(reservaRe, hWnd);

			MessageBox(hWnd, "Se guardaron los datos", "GUARDAR Y SALIR", MB_OK);
			PostQuitMessage(10);

		}
		if (LOWORD(wParam) == ID_BAJASCAMBIOS && HIWORD(wParam) == BN_CLICKED) {
			HWND BajasCambios = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
			ShowWindow(BajasCambios, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROSMEDICOS && HIWORD(wParam) == BN_CLICKED) {
			HWND Medicos = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
			ShowWindow(Medicos, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == BTN_VOLVERE && HIWORD(wParam) == BN_CLICKED) {
			HWND Medicos = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
			ShowWindow(Medicos, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}

		if (LOWORD(wParam) == ID_REGISTRODEPACIENTES && HIWORD(wParam) == BN_CLICKED) {
			HWND Pacientes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROPACIENTE), NULL, RegistroPacientes);
			ShowWindow(Pacientes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROCITAS && HIWORD(wParam) == BN_CLICKED) {
			HWND Citas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROCONSULTAS), NULL, RegistroCitas);
			ShowWindow(Citas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}

		if (LOWORD(wParam) == BTN_ACEPTARE && HIWORD(wParam) == BN_CLICKED) {
			wchar_t Especie[MAX_PATH];
			HWND newEspe = GetDlgItem(hWnd, TEX_NewEspecialidad);
			int length = (int)GetWindowTextLength(newEspe);
			GetWindowTextA(newEspe, (LPSTR)Especie, length + 1);
			if (length == 0) {
				MessageBox(hWnd, "Coloque primero un nombre a la especialidad", "Especialidad", MB_OK);
				flag = false;
			}
			else {
				flag = true;

			}
			char Des[MAX_PATH];

			if (flag) {
				HWND newDes = GetDlgItem(hWnd, TEX_DescripcionEspecialidad);
				int length = (int)GetWindowTextLength(newDes);
				GetWindowTextA(newEspe, (LPSTR)Des, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "Es necesario poner una descripcion", "Especialidad", MB_OK);
					flag = false;
				}
				else
					flag = true;
			}
			if (flag) {
				if (originE == NULL) {
					originE = new Especialidad;
					auxE = originE;
					auxE->prev = NULL;
				}
				else {
					while (auxE->next != NULL)
						auxE = auxE->next;
					auxE->next = new Especialidad;
					auxE->next->prev = auxE;
					auxE = auxE->next;
				}
				
				auxE->next = NULL;
				memcpy(auxE->especialidad, Especie, 260);
				memcpy(auxE->descripcion, Des, 260);
				MessageBox(hWnd, "Se ha añadido la nueva especialidad", "Especialidad", MB_OK);
			}
		
		}

		break;

	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);

		break;
	}
	return FALSE;

}
BOOL CALLBACK SemanaMes(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG: {
		HWND Hora_Combo2 = GetDlgItem(hWnd, CB_MesConsul2);
		char Horas2[12][MAX_PATH]{ "enero","febrero","marzo","abril","mayo","junio","julio","agosto","septiembre","octubre","noviembre","diciembre" };
		for (int j = 0; j < 12; j++) {
			SendMessage(Hora_Combo2, CB_ADDSTRING, NULL, (LPARAM)Horas2[j]);

		}
		HWND Semana_Combo2 = GetDlgItem(hWnd, CB_SemanaConsul2);
		char Semana2[12][MAX_PATH]{ "1","2","3","4","5" };
		for (int j = 0; j < 5; j++) {
			SendMessage(Semana_Combo2, CB_ADDSTRING, NULL, (LPARAM)Semana2[j]);

		}
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_GuardarSALIR && HIWORD(wParam) == BN_CLICKED) {
			ContadorRegistroGuardar();

			Medico* reservaME = raiz;
			AMedicos->guardar(reservaME, hWnd);

			Especialidad* ReservaME = originE;
			GuardarEspecialidades(ReservaME, hWnd);

			reservaME = raiz;
			AMedicos->GuardarMedico(reservaME, hWnd);

			Paciente* reservaPara = originP;
			Pacientes->guardadoPaciente(reservaPara, hWnd);

			reservaPara = originP;
			Pacientes->GuardarPacientesBIN(reservaPara, hWnd);

			Registros* reservaRe = originR;
			Registro->GuardarRegistro(reservaRe, hWnd);

			MessageBox(hWnd, "Se guardaron los datos", "GUARDAR Y SALIR", MB_OK);
			PostQuitMessage(10);

		}
		if (LOWORD(wParam) == ID_BAJASCAMBIOS && HIWORD(wParam) == BN_CLICKED) {
			HWND BajasCambios = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
			ShowWindow(BajasCambios, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ESPECIALIDAD && HIWORD(wParam) == BN_CLICKED) {
			HWND Especialidad = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ESPECIALIDADES), NULL, Especialidades);
			ShowWindow(Especialidad, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ConsultaCitas2 && HIWORD(wParam) == BN_CLICKED) {
			HWND Consultadecitas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_RegistroCita), NULL, RegistroConsultasFinal);
			ShowWindow(Consultadecitas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		
		if (LOWORD(wParam) == ID_CONSULTADECITAS_ESPECIALIDADYSEMANADELMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaEspecialidadSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_EspecialidadSemanaMes), NULL, EspecialidadSemanaMes);
			ShowWindow(BusquedaEspecialidadSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROSMEDICOS && HIWORD(wParam) == BN_CLICKED) {
			HWND Medicos = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
			ShowWindow(Medicos, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_MEDICOYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND ConsultasMedicoMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SGDL_CitaMedico), NULL, BusquedaMedicoMes);
			ShowWindow(ConsultasMedicoMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
	
		if (LOWORD(wParam) == ID_REGISTRODEPACIENTES && HIWORD(wParam) == BN_CLICKED) {
			HWND Pacientes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROPACIENTE), NULL, RegistroPacientes);
			ShowWindow(Pacientes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROCITAS && HIWORD(wParam) == BN_CLICKED) {
			HWND Citas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROCONSULTAS), NULL, RegistroCitas);
			ShowWindow(Citas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}

		if (LOWORD(wParam) == BTM_ACEPTARSEMANAMES && HIWORD(wParam) == BN_CLICKED) {
			char Meso[MAX_PATH] = "";
			HWND VAlidacionMes = GetDlgItem(hWnd, CB_MesConsul2);
			int length = (int)GetWindowTextLength(VAlidacionMes);
			GetWindowText(VAlidacionMes, (LPSTR)Meso, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se ha colado un Mes en la sección", "Mes y Semana", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			char Semanan[MAX_PATH] = "";

			if (flag) {
				HWND VAlidacionSemana = GetDlgItem(hWnd, CB_SemanaConsul2);
				int length = (int)GetWindowTextLength(VAlidacionSemana);
				GetWindowText(VAlidacionSemana, (LPSTR)Semanan, length + 1);

				if (length == 0) {
					MessageBox(hWnd, "No se ha colado una Semana en la sección", "Mes y Semana", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
			}

			if (flag) {
					Registros* reservado = originR;
					Registro->FiltroMesSemana(reservado, Meso, Semanan, hWnd);


			}
		}
		break;

	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);

		break;
	}
	return FALSE;
}
BOOL CALLBACK EspecialidadSemanaMes(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg) {
	case WM_INITDIALOG:{
		Especialidad* reserva = originE;
		HWND  PrimeraEspecialidad = GetDlgItem(hWnd, CB_EspecialidadFiltro);
		while (reserva != NULL) {
			SendMessage(PrimeraEspecialidad, CB_ADDSTRING, NULL, (LPARAM)reserva->especialidad);
			reserva = reserva->next;
		}
		HWND Hora_Combo2 = GetDlgItem(hWnd, CB_MESFILTRO3);
		char Horas2[12][MAX_PATH]{ "enero","febrero","marzo","abril","mayo","junio","julio","agosto","septiembre","octubre","noviembre","diciembre" };
		for (int j = 0; j < 12; j++) {
			SendMessage(Hora_Combo2, CB_ADDSTRING, NULL, (LPARAM)Horas2[j]);

		}
		HWND Semana_Combo2 = GetDlgItem(hWnd, CB_SEMANAFILTRO3);
		char Semana2[12][MAX_PATH]{ "1","2","3","4","5" };
		for (int j = 0; j < 5; j++) {
			SendMessage(Semana_Combo2, CB_ADDSTRING, NULL, (LPARAM)Semana2[j]);

		}
	}
	
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_GuardarSALIR && HIWORD(wParam) == BN_CLICKED) {
			ContadorRegistroGuardar();

			Medico* reservaME = raiz;
			AMedicos->guardar(reservaME, hWnd);

			Especialidad* ReservaME = originE;
			GuardarEspecialidades(ReservaME, hWnd);

			reservaME = raiz;
			AMedicos->GuardarMedico(reservaME, hWnd);

			Paciente* reservaPara = originP;
			Pacientes->guardadoPaciente(reservaPara, hWnd);

			reservaPara = originP;
			Pacientes->GuardarPacientesBIN(reservaPara, hWnd);

			Registros* reservaRe = originR;
			Registro->GuardarRegistro(reservaRe, hWnd);

			MessageBox(hWnd, "Se guardaron los datos", "GUARDAR Y SALIR", MB_OK);
			PostQuitMessage(10);

		}
		if (LOWORD(wParam) == ID_BAJASCAMBIOS && HIWORD(wParam) == BN_CLICKED) {
			HWND BajasCambios = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
			ShowWindow(BajasCambios, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ConsultaCitas2 && HIWORD(wParam) == BN_CLICKED) {
			HWND Consultadecitas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_RegistroCita), NULL, RegistroConsultasFinal);
			ShowWindow(Consultadecitas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ESPECIALIDAD && HIWORD(wParam) == BN_CLICKED) {
			HWND Especialidad = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ESPECIALIDADES), NULL, Especialidades);
			ShowWindow(Especialidad, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_MEDICOYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND ConsultasMedicoMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SGDL_CitaMedico), NULL, BusquedaMedicoMes);
			ShowWindow(ConsultasMedicoMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_SEMANAYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ConsultaSemanaMes), NULL, SemanaMes);
			ShowWindow(BusquedaSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROSMEDICOS && HIWORD(wParam) == BN_CLICKED) {
			HWND Medicos = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
			ShowWindow(Medicos, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTRODEPACIENTES && HIWORD(wParam) == BN_CLICKED) {
			HWND Pacientes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROPACIENTE), NULL, RegistroPacientes);
			ShowWindow(Pacientes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROCITAS && HIWORD(wParam) == BN_CLICKED) {
			HWND Citas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROCONSULTAS), NULL, RegistroCitas);
			ShowWindow(Citas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == BTN_ESPECIALIDADGUARDAR && HIWORD(wParam) == BN_CLICKED) {
			char Especialidadad[MAX_PATH] = "";
			HWND VAlidacionEspecialidad = GetDlgItem(hWnd, CB_EspecialidadFiltro);
			int length = (int)GetWindowTextLength(VAlidacionEspecialidad);
			GetWindowText(VAlidacionEspecialidad, (LPSTR)Especialidadad, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se ha colado una Especialidad en la sección", "Especialidad, Mes y Semana", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			char Mes[MAX_PATH] = "";
			HWND VAlidacionMes = GetDlgItem(hWnd, CB_MESFILTRO3);
			length = (int)GetWindowTextLength(VAlidacionMes);
			GetWindowText(VAlidacionMes, (LPSTR)Mes, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se ha colado un Mes en la sección", "Especialidad, Mes y Semana", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {
				char Semanan[MAX_PATH] = "";
				HWND VAlidacionSemana = GetDlgItem(hWnd, CB_SEMANAFILTRO3);
				int length = (int)GetWindowTextLength(VAlidacionSemana);
				GetWindowText(VAlidacionSemana, (LPSTR)Semanan, length + 1);

				if (length == 0) {
					MessageBox(hWnd, "No se ha colado una Semana en la sección", "Mes y Semana", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
				if (flag) {
					Registros* reservado = originR;
					Registro->FiltroEspecialidadMesSemana(reservado, Especialidadad, Mes, Semanan, hWnd);
				}
			}
		}

		break;

	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);

		break;
	}
	return FALSE;
}

BOOL CALLBACK BusquedaMedicoMes(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG:{
			HWND Hora_Combo = GetDlgItem(hWnd, CB_MesesFiltro);
		char Horas[12][MAX_PATH]{ "enero","febrero","marzo","abril","mayo","junio","julio","agosto","septiembre","octubre","noviembre","diciembre" };
		for (int i = 0; i < 12; i++) {
			SendMessage(Hora_Combo, CB_ADDSTRING, NULL, (LPARAM)Horas[i]);

		}
		Medico* reserva = raiz;
		HWND PrimerMedic = GetDlgItem(hWnd, CB_MEDICOSYMES);
		AMedicos->combo(reserva, PrimerMedic);
		}
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BAJASCAMBIOS && HIWORD(wParam) == BN_CLICKED) {
			HWND BajasCambios = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
			ShowWindow(BajasCambios, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_GuardarSALIR && HIWORD(wParam) == BN_CLICKED) {
			ContadorRegistroGuardar();

			Medico* reservaME = raiz;
			AMedicos->guardar(reservaME, hWnd);

			Especialidad* ReservaME = originE;
			GuardarEspecialidades(ReservaME, hWnd);

			reservaME = raiz;
			AMedicos->GuardarMedico(reservaME, hWnd);

			Paciente* reservaPara = originP;
			Pacientes->guardadoPaciente(reservaPara, hWnd);

			reservaPara = originP;
			Pacientes->GuardarPacientesBIN(reservaPara, hWnd);

			Registros* reservaRe = originR;
			Registro->GuardarRegistro(reservaRe, hWnd);

			MessageBox(hWnd, "Se guardaron los datos", "GUARDAR Y SALIR", MB_OK);
			PostQuitMessage(10);

		}
		if (LOWORD(wParam) == ID_ConsultaCitas2 && HIWORD(wParam) == BN_CLICKED) {
			HWND Consultadecitas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_RegistroCita), NULL, RegistroConsultasFinal);
			ShowWindow(Consultadecitas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_ESPECIALIDAD && HIWORD(wParam) == BN_CLICKED) {
			HWND Especialidad = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ESPECIALIDADES), NULL, Especialidades);
			ShowWindow(Especialidad, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_ESPECIALIDADYSEMANADELMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaEspecialidadSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_EspecialidadSemanaMes), NULL, EspecialidadSemanaMes);
			ShowWindow(BusquedaEspecialidadSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_SEMANAYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ConsultaSemanaMes), NULL, SemanaMes);
			ShowWindow(BusquedaSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROSMEDICOS && HIWORD(wParam) == BN_CLICKED) {
			HWND Medicos = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
			ShowWindow(Medicos, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTRODEPACIENTES && HIWORD(wParam) == BN_CLICKED) {
			HWND Pacientes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROPACIENTE), NULL, RegistroPacientes);
			ShowWindow(Pacientes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROCITAS && HIWORD(wParam) == BN_CLICKED) {
			HWND Citas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROCONSULTAS), NULL, RegistroCitas);
			ShowWindow(Citas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == BTN_ACEPTARMEDICOMES && HIWORD(wParam) == BN_CLICKED) {
			char MedicoMEScita[MAX_PATH] = "";
			HWND ValidacionMedicoo = GetDlgItem(hWnd, CB_MEDICOSYMES);
			int length = (int)GetWindowTextLength(ValidacionMedicoo);
			GetWindowText(ValidacionMedicoo, (LPSTR)MedicoMEScita, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se ha colado un Medico en la sección", "Mes y Medico", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			char MEScita[MAX_PATH] = "";

			if (flag) {
				HWND validacionMES = GetDlgItem(hWnd, CB_MesesFiltro);
				int length = (int)GetWindowTextLength(validacionMES);
				GetWindowText(validacionMES, (LPSTR)MEScita, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se ha colado un Medico en la sección", "Mes y Medico", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
			}
			if (flag) {
				Registros* reserva = originR;
				Registro->FiltroMedicoyMES(reserva, MedicoMEScita, MEScita, hWnd);
			}
		}

		

		break;

	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);

		break;
	}
	return FALSE;
}
BOOL CALLBACK RegistroConsultasFinal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_INITDIALOG:{
			Registros* reserva = originR;
		HWND PrimirCedula = GetDlgItem(hWnd, CB_Cedula);
		while (reserva != NULL) {
			SendMessage(PrimirCedula, CB_ADDSTRING, NULL, (LPARAM)reserva->NumeroCitas);
			reserva = reserva->next;
		}
					 
				
	}
		
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_GuardarSALIR && HIWORD(wParam) == BN_CLICKED) {
			ContadorRegistroGuardar();

			Medico* reservaME = raiz;
			AMedicos->guardar(reservaME, hWnd);

			Especialidad* ReservaME = originE;
			GuardarEspecialidades(ReservaME, hWnd);

			reservaME = raiz;
			AMedicos->GuardarMedico(reservaME, hWnd);

			Paciente* reservaPara = originP;
			Pacientes->guardadoPaciente(reservaPara, hWnd);

			reservaPara = originP;
			Pacientes->GuardarPacientesBIN(reservaPara, hWnd);

			Registros* reservaRe = originR;
			Registro->GuardarRegistro(reservaRe, hWnd);

			MessageBox(hWnd, "Se guardaron los datos", "GUARDAR Y SALIR", MB_OK);
			PostQuitMessage(10);

		}
		if (LOWORD(wParam) == ID_ESPECIALIDAD && HIWORD(wParam) == BN_CLICKED) {
			HWND Especialidad = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ESPECIALIDADES), NULL, Especialidades);
			ShowWindow(Especialidad, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_BAJASCAMBIOS && HIWORD(wParam) == BN_CLICKED) {
			HWND BajasCambios = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
			ShowWindow(BajasCambios, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_ESPECIALIDADYSEMANADELMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaEspecialidadSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_EspecialidadSemanaMes), NULL, EspecialidadSemanaMes);
			ShowWindow(BusquedaEspecialidadSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_CONSULTADECITAS_SEMANAYMES && HIWORD(wParam) == BN_CLICKED) {
			HWND BusquedaSemanaMes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ConsultaSemanaMes), NULL, SemanaMes);
			ShowWindow(BusquedaSemanaMes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROSMEDICOS && HIWORD(wParam) == BN_CLICKED) {
			HWND Medicos = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
			ShowWindow(Medicos, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTRODEPACIENTES && HIWORD(wParam) == BN_CLICKED) {
			HWND Pacientes = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROPACIENTE), NULL, RegistroPacientes);
			ShowWindow(Pacientes, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == ID_REGISTROCITAS && HIWORD(wParam) == BN_CLICKED) {
			HWND Citas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROCONSULTAS), NULL, RegistroCitas);
			ShowWindow(Citas, SW_SHOW);
			//CIERRE = true;
			DestroyWindow(hWnd);
		}
		if (LOWORD(wParam) == BTN_DespliegueREGISTROS && HIWORD(wParam) == BN_CLICKED) {
			char CedulaRegis[MAX_PATH] = "";
			HWND VAlidacioncedulaRegist = GetDlgItem(hWnd, CB_Cedula);
			int length = (int)GetWindowTextLength(VAlidacioncedulaRegist);
			GetWindowText(VAlidacioncedulaRegist, (LPSTR)CedulaRegis, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se ha colado una Semana en la sección", "Mes y Semana", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {
				Registros* registroFINAL = originR;
				Medico* medicoFINAL = raiz;
				Paciente* pacienteFINAL = originP;
				HWND NumerodeCita = GetDlgItem(hWnd, INF_NUMEROCITA);
				registrosFINAL(registroFINAL, medicoFINAL, pacienteFINAL,CedulaRegis, hWnd);


			}
		}
	break;

	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);

		break;
	}
	return FALSE;
}
BOOL CALLBACK IniciarSesion(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_COMMAND: 
		if (LOWORD(wParam) == BTN_INICIARSESION && HIWORD(wParam) == BN_CLICKED) {
			char Usuario[MAX_PATH] = "";
			HWND ValidacionUsuario = GetDlgItem(hWnd, EDIT_NOMBREINICIAR);
			int length = (int)GetWindowTextLength(ValidacionUsuario);
			GetWindowText(ValidacionUsuario, (LPSTR)Usuario, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se coloco ningún Usuario", "INICIAR SESION", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {
				if (strcmp(Usuario, "Marco Cantu") == 0) {
					flag = true;
				}
				else {
					MessageBox(hWnd, "No se coloco el usuario correcto", "INICIAR SESION", MB_OK);
					flag = false;
				}
				if (flag) {
					char Contrasena[MAX_PATH] = "";
					HWND ValidacionContrasena = GetDlgItem(hWnd, EDIT_CONTRASENAINICIAR2);
					int length = (int)GetWindowTextLength(ValidacionContrasena);
					GetWindowText(ValidacionContrasena, (LPSTR)Contrasena, length + 1);
					if (length = 0) {
						MessageBox(hWnd, "No se coloco ninguna contraseña", "INICIAR SESION", MB_OK);
						flag = false;
					}
					else {
						flag = true;
					}
					if (flag) {
						if (strcmp(Contrasena, "estructuradatos") == 0) {
							HWND Medicos = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
							ShowWindow(Medicos, SW_SHOW);
							//CIERRE = true;
							DestroyWindow(hWnd);
						}
						else {
							MessageBox(hWnd, "No se coloco la contraseña correcta", "INICIAR SESION", MB_OK);
							flag = false;
						
						}
					}
				}
			}
		}

		break;

	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);

		break;
	}
	return FALSE;
}
BOOL CALLBACK AltasBajasCambios(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg) {

	case WM_INITDIALOG: {
		Especialidad* reserva = originE;
		HWND  PrimeraEspecialidad = GetDlgItem(hWnd, CB_NuevoEspecialidadRe);
		while (reserva != NULL) {
			SendMessage(PrimeraEspecialidad, CB_ADDSTRING, NULL, (LPARAM)reserva->especialidad);
			reserva = reserva->next;
		}
		Especialidad* reservaAltas = originE;
		HWND EspecialidadAltas = GetDlgItem(hWnd, CB_EspecialidadALTAS);
		while (reservaAltas != NULL) {
			SendMessage(EspecialidadAltas, CB_ADDSTRING, NULL, (LPARAM)reservaAltas->especialidad);
			if (reservaAltas != NULL) {
				reservaAltas = reservaAltas->next;
			}
		}
		reservaAltas = originE;
		HWND EspecialidadAltasMEdicos = GetDlgItem(hWnd, CB_NuevasEspecialidadMedico);
		while (reservaAltas != NULL) {
			SendMessage(EspecialidadAltasMEdicos, CB_ADDSTRING, NULL, (LPARAM)reservaAltas->especialidad);
			if (reservaAltas != NULL) {
				reservaAltas = reservaAltas->next;
			}
		}
		Paciente* reservaPacientes = originP;
		HWND PrimeraPaciente = GetDlgItem(hWnd, CB_NuevoPacienteRe);
		while (reservaPacientes != NULL) {
			SendMessage(PrimeraPaciente, CB_ADDSTRING, NULL, (LPARAM)reservaPacientes->nombre);
			reservaPacientes = reservaPacientes->next;
		}

		HWND Hora_Combo = GetDlgItem(hWnd, CB_NuevaHora);
		char Horas[16][MAX_PATH]{ "06","07","08","09","10","11","12","13","14","15","16","17","18","19" };
		for (int i = 0; i < 14; i++) {
			SendMessage(Hora_Combo, CB_ADDSTRING, NULL, (LPARAM)Horas[i]);

		}

		HWND Minuto_Combo = GetDlgItem(hWnd, CB_NuevoMinutos);
		char Minutos[3][MAX_PATH]{ "00","20","40" };
		for (int i = 0; i < 3; i++) {
			SendMessage(Minuto_Combo, CB_ADDSTRING, NULL, (LPARAM)Minutos[i]);
		}

		Medico* reservaMedicoAltas = raiz;
		HWND MedicosAltas = GetDlgItem(hWnd, CB_MedicosALTAS);
		AMedicos->comboCedula(reservaMedicoAltas, MedicosAltas);

		reservaMedicoAltas = raiz;
		HWND MedicosAltasPacientes = GetDlgItem(hWnd, CB_NuevaMedicoRecomendadoPa);
		AMedicos->combo(reservaMedicoAltas, MedicosAltasPacientes);


		Paciente* reservaPacientesAltas = originP;
		HWND PacienteAltas = GetDlgItem(hWnd, CB_PacienteALTAS);
		while (reservaPacientesAltas != NULL) {
			SendMessage(PacienteAltas, CB_ADDSTRING, NULL, (LPARAM)reservaPacientesAltas->nombre);
			reservaPacientesAltas = reservaPacientesAltas->next;
		}
		Registros* reservaREgistrosAltas = originR;
		HWND RegistrosAltas = GetDlgItem(hWnd, CB_RegistrosALTAS);
		while (reservaREgistrosAltas != NULL) {
			SendMessage(RegistrosAltas, CB_ADDSTRING, NULL, (LPARAM)reservaREgistrosAltas->NumeroCitas);
			reservaREgistrosAltas = reservaREgistrosAltas->next;
		}
	}

	case WM_COMMAND: {
		if (LOWORD(wParam) == BTN_ELIMINARESPECIALIDAD && HIWORD(wParam) == BN_CLICKED) {
			char BufferEspecialidad[MAX_PATH] = "";
			int length;
			length = 0;
			HWND  EspecialidadsBAjas = GetDlgItem(hWnd, CB_EspecialidadALTAS);
			length = (int)GetWindowTextLength(EspecialidadsBAjas);
			GetWindowText(EspecialidadsBAjas, BufferEspecialidad, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en Medico que le atendio", "PACIENTES", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {
				Especialidad* Aux = auxE;
				EliminarEspecialidad(originE, Aux, BufferEspecialidad);
				contadorR--;
				HWND RecargarAltas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
				ShowWindow(RecargarAltas, SW_SHOW);
				DestroyWindow(hWnd);


			}
		}
		if (LOWORD(wParam) == BTN_ELIMINARPACIENTE && HIWORD(wParam) == BN_CLICKED) {
			char BufferPaciente[MAX_PATH] = "";
			int length;
			length = 0;
			HWND  PacientesBAjas = GetDlgItem(hWnd, CB_PacienteALTAS);
			length = (int)GetWindowTextLength(PacientesBAjas);
			GetWindowText(PacientesBAjas, BufferPaciente, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en Paciente", "PACIENTES BAJAS Y CAMBIOS", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {
				Paciente* AuxPa = auxP;
				Pacientes->Eliminar(originP, AuxPa, BufferPaciente);
				contadorP--;
				HWND RecargarAltas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
				ShowWindow(RecargarAltas, SW_SHOW);
				DestroyWindow(hWnd);


			}
		}
		if (LOWORD(wParam) == BTN_ELIMINARREGISTRO && HIWORD(wParam) == BN_CLICKED) {
			char BufferRegistro[MAX_PATH] = "";
			int length;
			length = 0;
			HWND  REgistrosBAjas = GetDlgItem(hWnd, CB_RegistrosALTAS);
			length = (int)GetWindowTextLength(REgistrosBAjas);
			GetWindowText(REgistrosBAjas, BufferRegistro, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en Registro", "PACIENTES BAJAS Y CAMBIOS", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {
				Registros* AuxRe = auxR;
				Registro->Eliminar(originR, AuxRe, BufferRegistro);
				HWND RecargarAltas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
				ShowWindow(RecargarAltas, SW_SHOW);
				DestroyWindow(hWnd);


			}
		}
		if (LOWORD(wParam) == BTN_ELIMINARMEDICO && HIWORD(wParam) == BN_CLICKED) {
			char BufferCeduka[MAX_PATH] = "";
			int length;
			length = 0;
			HWND  CedulaBAjas = GetDlgItem(hWnd, CB_MedicosALTAS);
			length = (int)GetWindowTextLength(CedulaBAjas);
			GetWindowText(CedulaBAjas, BufferCeduka, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en Registro", "PACIENTES BAJAS Y CAMBIOS", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {

				Medico* reservMed = raiz;

				int intcedula = atoi(BufferCeduka);
				AMedicos->Eliminar(reservMed, intcedula);

				HWND RecargarAltas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
				ShowWindow(RecargarAltas, SW_SHOW);
				DestroyWindow(hWnd);


			}
		}
		if (LOWORD(wParam) == BTN_MODIFICARESPECIALIDADES && HIWORD(wParam) == BN_CLICKED) {
			char Especie[MAX_PATH];
			char BufferEspecialidad[MAX_PATH] = "";
			char Des[MAX_PATH];
			int length;
			length = 0;
			HWND  EspecialidadsBAjas = GetDlgItem(hWnd, CB_EspecialidadALTAS);
			length = (int)GetWindowTextLength(EspecialidadsBAjas);
			GetWindowText(EspecialidadsBAjas, BufferEspecialidad, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en Nueva especialidad que le atendio", "BAJAS Y CAMBIOS", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			if (flag) {
				HWND newEspe = GetDlgItem(hWnd, TEX_NuevaEspecialidad);
				int length = (int)GetWindowTextLength(newEspe);
				GetWindowTextA(newEspe, Especie, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "Coloque primero un nombre a la Nueva especialidad", "Especialidad", MB_OK);
					flag = false;
				}
				else {
					flag = true;

				}
			}
			if (flag) {
				HWND newDes = GetDlgItem(hWnd, TEX_NuevaDescripcion);
				int length = (int)GetWindowTextLength(newDes);
				GetWindowTextA(newDes, (LPSTR)Des, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "Es necesario poner una Nueva descripcion", "Especialidad", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
				if (flag) {
					Especialidad* Reserva = originE;
					while (strcmp(BufferEspecialidad, Reserva->especialidad)!=0) {
						Reserva = Reserva->next;
					}
					strcpy_s(Reserva->especialidad, Especie);
					strcpy_s(Reserva->descripcion, Des);
					MessageBox(hWnd, "Se ha añadido la nueva especialidad", "Especialidad", MB_OK);
					HWND RecargarAltas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
					ShowWindow(RecargarAltas, SW_SHOW);
					DestroyWindow(hWnd);
				}
			}
		}
		if (LOWORD(wParam) == BTN_MODIFICARPACIENTES && HIWORD(wParam) == BN_CLICKED) {
			char Bufferpacientes[MAX_PATH] = "";

			int length;
			length = 0;
			HWND  PacientesBAjas = GetDlgItem(hWnd, CB_PacienteALTAS);
			length = (int)GetWindowTextLength(PacientesBAjas);
			GetWindowText(PacientesBAjas, Bufferpacientes, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en Nuevo Paciente que le atendio", "BAJAS Y CAMBIOS", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			char Buffernuevopaciente[MAX_PATH];
			if (flag) {
				HWND newPacien = GetDlgItem(hWnd, TEX_NuevoPaciente);
				int length = (int)GetWindowTextLength(newPacien);
				GetWindowTextA(newPacien, (LPSTR)Buffernuevopaciente, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "Es necesario poner una Nuevo Paciente", "BAJAS Y CAMBIOS", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}

			}
			bool Genero;
			if (flag) {
				length = 0;
				if (IsDlgButtonChecked(hWnd, RAD_NUEVOMASCULINO) == BST_CHECKED || IsDlgButtonChecked(hWnd, RAD_NUEVOFEMENINO) == BST_CHECKED) {
					if (IsDlgButtonChecked(hWnd, RAD_NUEVOMASCULINO) == BST_CHECKED) {
						Genero = true;
						flag = true;
					}
					else {
						if (IsDlgButtonChecked(hWnd, RAD_NUEVOFEMENINO) == BST_CHECKED) {
							flag = true;
							Genero = false;
						}
						else {
							flag = false;
						}
					}
				}
				else {
					flag = false;
					MessageBox(hWnd, "No se selecciono ninguna opción de genero", "Paciente Genero", MB_OK);
				}
			}
			char NumeroTelefonoPaciente[MAX_PATH];
			if (flag) {
				length = 0;
				HWND NumeTelPaciente = GetDlgItem(hWnd, TEX_NuevoTelefonoPaciente);
				int length = (int)GetWindowTextLength(NumeTelPaciente);
				GetWindowTextA(NumeTelPaciente, NumeroTelefonoPaciente, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Numero de Telefono""", "Te", MB_OK);
					flag = false;
				}
				else {
					if (length == 8 || length == 16) {
						flag = true;
					}
					else {

						MessageBox(hWnd, "Debe colocarse más de 8 o 15 numeros en el espacio de telefono", "Telefono del Paciente", MB_OK);
						flag = false;
					}
				}
			}
			char BufferNacimiento[MAX_PATH] = "";
			if (flag) {
				length = 0;
				HWND FechaNacimiento = GetDlgItem(hWnd, DATE_FechaNacimientoPaciente);
				length = (int)GetWindowTextLength(FechaNacimiento);
				GetWindowText(FechaNacimiento, BufferNacimiento, length + 1);
			}
	
			char BufferMedico[MAX_PATH] = "";
			if (flag) {
				length = 0;
				HWND  MedicoPaciente = GetDlgItem(hWnd, CB_NuevaMedicoRecomendadoPa);
				length = (int)GetWindowTextLength(MedicoPaciente);
				GetWindowText(MedicoPaciente, BufferMedico, length + 1);

				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en Medico que le atendio", "PACIENTES", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
			}
			char EdadPaciente[MAX_PATH] = "";
			if (flag) {
				length = 0;
				HWND EDAD = GetDlgItem(hWnd, TEX_NuevaEdad);
				int length = (int)GetWindowTextLength(EDAD);
				GetWindowTextA(EDAD, EdadPaciente, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""EDAD""", "Telefono del Paciente", MB_OK);
					flag = false;
				}
				else {
					int edadEntera = atoi(EdadPaciente);
					if (edadEntera > 0 && edadEntera <= 99) {
						flag = true;
					}
					else {

						MessageBox(hWnd, "Solo se le dara Consulta a personas 1-99 años", "Telefono del Paciente", MB_OK);
						flag = false;
					}
				}

				if (flag) {
					for (int i = 0; i < length; i++) {
						if (EdadPaciente[i] > 57 || EdadPaciente[i] < 48) {
							flag = false;
						}
						else {
							flag = true;
						}
					}
					if (flag == false)
						MessageBox(hWnd, "Coloca solo numeros en el apartado de Telefono", "Numero de Telefono", MB_OK);

				}
				char BufferDescripcion[MAX_PATH] = "";
				if (flag) {
					length = 0;
					HWND Descripcion = GetDlgItem(hWnd, TEX_NuevaDescripcionPa);
					length = (int)GetWindowTextLength(Descripcion);
					GetWindowText(Descripcion, BufferDescripcion, length + 1);

					if (length == 0) {
						MessageBox(hWnd, "No se coloco nada en Descripcion", "PACIENTES", MB_OK);
						flag = false;
					}
					else {
						flag = true;
					}
				}
				if (flag) {
					Paciente* Reserva = originP;
					while (strcmp(Bufferpacientes, Reserva->nombre)!=0) {
						Reserva = Reserva->next;
					}
					strcpy_s(Reserva->nombre, Buffernuevopaciente);
					strcpy_s(Reserva->descripcion, BufferDescripcion);
					strcpy_s(Reserva->fechaNacimiento, BufferNacimiento);
					strcpy_s(Reserva->edad, EdadPaciente);
					strcpy_s(Reserva->Telefono, NumeroTelefonoPaciente);
					Reserva->genero = Genero;
					strcpy_s(Reserva->MedicoRecomendado, BufferMedico);
					MessageBox(hWnd, "Se ha añadido la nueva Paciente", "Nuevo Paciente", MB_OK);
					HWND RecargarAltas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
					ShowWindow(RecargarAltas, SW_SHOW);
					DestroyWindow(hWnd);
				}
			}
		}
		if (LOWORD(wParam) == BTN_NuevoDespliegue && HIWORD(wParam) == BN_CLICKED) {
			char ValidacionEspecie[MAX_PATH];

			HWND ValidacionEspecialidades = GetDlgItem(hWnd, CB_NuevoEspecialidadRe);
			int length = (int)GetWindowTextLength(ValidacionEspecialidades);
			GetWindowText(ValidacionEspecialidades, (LPSTR)ValidacionEspecie, length + 1);


			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en la sección ""Especialidad""", "Tipo de Especialidad", MB_OK);
				flag = false;

			}
			else {
				flag = true;
				validaciones(CB_NuevoMedicoRe, hWnd, MOSTRAR);
				validaciones(BTN_NuevoDespliegue2, hWnd, MOSTRAR);
				validaciones(BTN_NuevoDespliegue, hWnd, DESACTIVAR);



				HWND PrimerMedico = GetDlgItem(hWnd, CB_NuevoMedicoRe);
				HWND EspecialidadESTA = GetDlgItem(hWnd, EST_nuevaEspecialidad2);
				Medico* reservaMe = raiz;
				AMedicos->combo2(reservaMe, PrimerMedico, ValidacionEspecie);

				validaciones(CB_NuevoEspecialidadRe, hWnd, ESCONDER);
				SetWindowText(EspecialidadESTA, ValidacionEspecie);
				validaciones(EST_nuevaEspecialidad2, hWnd, MOSTRAR);


			}

			if (flag) {
				GetWindowText(ValidacionEspecialidades, (LPSTR)ValidacionEspecie, length + 1);
			}
		}
		if (LOWORD(wParam) == BTN_NuevoDespliegue2 && HIWORD(wParam) == BN_CLICKED) {
			char ValidacionMedico2[MAX_PATH];

			HWND ValidacionMedico = GetDlgItem(hWnd, CB_NuevoMedicoRe);
			int length = (int)GetWindowTextLength(ValidacionMedico);
			GetWindowText(ValidacionMedico, (LPSTR)ValidacionMedico2, length + 1);


			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en la sección ""Medico""", "Tipo de Especialidad", MB_OK);
				flag = false;

			}
			else {


				Medico* reservaMe = raiz;
				HWND ConsultorioESTA = GetDlgItem(hWnd, ESTNUEVOCONSULTORIORE);

				AMedicos->estatic(reservaMe, ConsultorioESTA, ValidacionMedico2);
				reservaMe = raiz;
				HWND LunesESTA = GetDlgItem(hWnd, EST_NUEVOLUNES);
				HWND MartesESTA = GetDlgItem(hWnd, EST_NUEVOMARTES);
				HWND MiercolesESTA = GetDlgItem(hWnd, EST_NUEVOMIERCOLES);
				HWND JuevesESTA = GetDlgItem(hWnd, EST_NUEVOJUEVES);
				HWND ViernesESTA = GetDlgItem(hWnd, EST_NUEVOVIERNES);
				AMedicos->estatic2(reservaMe, LunesESTA, ValidacionMedico2, LUNES);
				reservaMe = raiz;

				AMedicos->estatic2(reservaMe, MartesESTA, ValidacionMedico2, MARTES);
				reservaMe = raiz;

				AMedicos->estatic2(reservaMe, MiercolesESTA, ValidacionMedico2, MIERCOLES);
				reservaMe = raiz;

				AMedicos->estatic2(reservaMe, JuevesESTA, ValidacionMedico2, JUEVES);
				reservaMe = raiz;

				AMedicos->estatic2(reservaMe, ViernesESTA, ValidacionMedico2, VIERNES);
				reservaMe = raiz;

				AMedicos->estatic3(reservaMe, ValidacionMedico2, hWnd);
				reservaMe = raiz;
				AMedicos->images(reservaMe, ValidacionMedico2, hWnd);

				validaciones(EST_Horario, hWnd, MOSTRAR);
				validaciones(EST_Consultorio, hWnd, MOSTRAR);
				validaciones(EST_InfoConsultorio, hWnd, MOSTRAR);
				validaciones(EST_InfoHorario, hWnd, MOSTRAR);
				validaciones(EST_Lunes, hWnd, MOSTRAR);
				validaciones(EST_Martes, hWnd, MOSTRAR);
				validaciones(EST_Miercoles, hWnd, MOSTRAR);
				validaciones(EST_Jueves, hWnd, MOSTRAR);
				validaciones(EST_Viernes, hWnd, MOSTRAR);
				validaciones(TEX_FechaConsul, hWnd, MOSTRAR);
				validaciones(EST_FechaRE, hWnd, MOSTRAR);
				validaciones(EST_Disponibilidad, hWnd, MOSTRAR);
			}
		}

		if (LOWORD(wParam) == BTN_VOLVERMEDICOS && HIWORD(wParam) == BN_CLICKED) {
			HWND MedicosREcargar = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_REGISTROMEDICOS), NULL, RegistroMedicos);
			ShowWindow(MedicosREcargar, SW_SHOW);
			DestroyWindow(hWnd);
		}

		if (LOWORD(wParam) == BTN_MODIFICARREGISTROS && HIWORD(wParam) == BN_CLICKED) {
			char Numeroconsulta[MAX_PATH];
			int length;
			length = 0;
			HWND NumeroPacientes = GetDlgItem(hWnd, CB_RegistrosALTAS);
			length = (int)GetWindowTextLength(NumeroPacientes);


			if (length == 0) {
				flag = false;

			}
			else {
				flag = true;
				GetWindowText(NumeroPacientes, (LPSTR)Numeroconsulta, length + 1);

			}
			if (flag == false) {
				MessageBox(hWnd, "No se coloco nada en la sección ""Paciente""", "Paciente", MB_OK);
			}
			char pacienteconsulta[MAX_PATH];

			if (flag) {
				length;
				length = 0;
				HWND Pacientes = GetDlgItem(hWnd, CB_NuevoPacienteRe);
				length = (int)GetWindowTextLength(Pacientes);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Paciente""", "Paciente", MB_OK);
					flag = false;

				}
				else {
					flag = true;
					GetWindowText(Pacientes, (LPSTR)pacienteconsulta, length + 1);

				}
			}
			char EspecieConsulta[MAX_PATH];

			if (flag) {

				HWND ValidacionEspecialidades = GetDlgItem(hWnd, CB_NuevoEspecialidadRe);
				int length = (int)GetWindowTextLength(ValidacionEspecialidades);
				GetWindowText(ValidacionEspecialidades, (LPSTR)EspecieConsulta, length + 1);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Especialidad""", "Tipo de Especialidad", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}
			}
			char MedicoConsulta[MAX_PATH];

			if (flag) {

				HWND ValidacionMedico = GetDlgItem(hWnd, CB_NuevoMedicoRe);
				int length = (int)GetWindowTextLength(ValidacionMedico);
				GetWindowText(ValidacionMedico, (LPSTR)MedicoConsulta, length + 1);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Medico""", "Nombre Medico", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}
			}
			char HoraConsul[MAX_PATH] = "";

			//////////////////////////////////HORA
			if (flag) {
				int length = 0;


				HWND ValidacionHora = GetDlgItem(hWnd, CB_NuevaHora);
				length = (int)GetWindowTextLength(ValidacionHora);
				GetWindowText(ValidacionHora, (LPSTR)HoraConsul, length + 1);

				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Hora""", "Hora Cita", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}
				Medico* reservaHora = raiz;
				AMedicos->MananaNocheConsulta(reservaHora, MedicoConsulta, hWnd, HoraConsul);

			}
			char MinutoConsul[MAX_PATH] = "";

			//////////////MINUTOS REGISTRO
			if (flag) {
				int lenght = 0;
				HWND ValidacionMinuto = GetDlgItem(hWnd, CB_NuevoMinutos);
				int length = (int)GetWindowTextLength(ValidacionMinuto);
				GetWindowText(ValidacionMinuto, (LPSTR)MinutoConsul, length + 1);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Minuto""", "Minutos Cita", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}

			}
			char BufferFECHALarga[MAX_PATH] = "";
			char BufferSEMANALarga[MAX_PATH] = "";
			char BufferDIALarga[MAX_PATH] = "";
			char BufferMESLarga[MAX_PATH] = "";
			char BufferANOLarga[MAX_PATH] = "";
			///////////////////////FECHA REGISTRO
			if (flag) {
				length = 0;

				int j = 0;
				int k = 0;
				int p = 0;
				HWND  FechaLarga = GetDlgItem(hWnd, DATE_NUEVAFECHA);
				length = (int)GetWindowTextLength(FechaLarga);
				GetWindowText(FechaLarga, BufferFECHALarga, length + 1);
				for (int i = 0; i < length; i++) {
					if (BufferFECHALarga[i] != 44) {
						BufferSEMANALarga[i] = BufferFECHALarga[i];
					}
					else {
						i++;
						i++;
						for (i; i < length; i++) {
							for (j; j < 4; j++) {
								if (BufferFECHALarga[i] != 100 && BufferFECHALarga[i] != 32) {
									BufferDIALarga[j] = BufferFECHALarga[i];
									flag = false;
									i++;
								}
								else {
									flag = true;
								}
							}
							break;
						}
						if (flag) {
							i++;
							i++;
							i++;
							i++;
							for (k; k < length; k++) {
								if (BufferFECHALarga[i] != 32) {
									BufferMESLarga[k] = BufferFECHALarga[i];
									i++;
									flag = false;
								}
								else {
									flag = true;
								}
								if (flag) {
									k = length;
								}
							}
							if (flag) {
								i = i + 4;
								for (p; p < length; p++) {
									if (BufferFECHALarga[i] != 32) {
										BufferANOLarga[p] = BufferFECHALarga[i];
										i++;
										flag = true;
									}
									else {
										i = length;
										j = length;
										k = length;
										p = length;
									}
								}
							}
						}
					}
				}
			}
			if (flag) {
				flag = false;
				Medico* raizSE = raiz;
				AMedicos->IdentificadorSemana(raizSE, MedicoConsulta, hWnd, BufferSEMANALarga);


			}
			


			if (flag) {

				//Registro->darAlta(originR, auxR);
				Registros* NuevoRegistroo = originR;
				while (strcmp(NuevoRegistroo->NumeroCitas, Numeroconsulta)!=0) {
					NuevoRegistroo = NuevoRegistroo->next;
				}
				strcpy_s(NuevoRegistroo->paciente, pacienteconsulta);
				strcpy_s(NuevoRegistroo->especialidad, EspecieConsulta);
				strcpy_s(NuevoRegistroo->nombreMedico, MedicoConsulta);
				strcpy_s(NuevoRegistroo->Fecha, BufferFECHALarga);
				strcpy_s(NuevoRegistroo->Ano, BufferANOLarga);
				strcpy_s(NuevoRegistroo->Dia, BufferDIALarga);
				strcpy_s(NuevoRegistroo->Semana, BufferSEMANALarga);
				strcpy_s(NuevoRegistroo->Mes, BufferMESLarga);
				strcpy_s(NuevoRegistroo->hora, HoraConsul);
				strcpy_s(NuevoRegistroo->minutos, MinutoConsul);

				MessageBox(hWnd, "Se dio de alta la cita", "Paciente", MB_OK);
				HWND RecargarAltas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
				ShowWindow(RecargarAltas, SW_SHOW);
				DestroyWindow(hWnd);

			}
		

		}
		static char chFileName[MAX_PATH] = "";
		if (LOWORD(wParam) == BTN_CARGARNUEVAFOTOMEDICO && HIWORD(wParam) == BN_CLICKED) {
			OPENFILENAME ofi;
			ZeroMemory(&ofi, sizeof(ofi));
			ofi.lpstrFilter = "Bit Map Images (*.bmp)\0*.bmp\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"; //Filtramos los formatos de archivo
			ofi.lStructSize = sizeof(ofi);
			ofi.lpstrFile = chFileName;
			ofi.nMaxFile = MAX_PATH;
			ofi.hwndOwner = hWnd;
			ofi.lpstrDefExt = "txt";
			ofi.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
			if (GetOpenFileName(&ofi)) {
				//MessageBox(NULL, chFileName, "Archivo", MB_OK); //Checamos la dirección en un MessageBox
				static HBITMAP hBmpImg;
				hBmpImg = (HBITMAP)LoadImage(NULL, chFileName, IMAGE_BITMAP, 150, 150, LR_LOADFROMFILE);
				SendMessage(GetDlgItem(hWnd, IMG_NUEVAIMAGENMEDICO), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpImg);
			}
		}
		if (LOWORD(wParam) == BTN_MODIFICARMEDICOS && HIWORD(wParam) == BN_CLICKED) {
			char BufferCedulaMedico [MAX_PATH] = "";
			int length;
			length = 0;
			HWND  MedicosModificar = GetDlgItem(hWnd, CB_MedicosALTAS);
			length = (int)GetWindowTextLength(MedicosModificar);
			GetWindowText(MedicosModificar, BufferCedulaMedico, length + 1);

			if (length == 0) {
				MessageBox(hWnd, "No se coloco nada en el combo de Medico", "PACIENTES BAJAS Y CAMBIOS", MB_OK);
				flag = false;
			}
			else {
				flag = true;
			}
			char NombreMedico[MAX_PATH];

			/////////////Nombre Medico
			if (flag) {
				HWND NomMedico = GetDlgItem(hWnd, TEX_NuevoNombreMedico);
				int length = (int)GetWindowTextLength(NomMedico);
				GetWindowTextA(NomMedico, NombreMedico, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Nuevo Nombre del medico""", "Nombre del Medico", MB_OK);
					flag = false;
				}
				else {
					flag = true;
				}
				if (flag == true) {
					for (int i = 0; i < length; i++) {
						if ((NombreMedico[i] < 65 || NombreMedico[i] > 90) && (NombreMedico[i] < 97 || NombreMedico[i] > 122)) {
							flag = false;
						}
						else {
							flag = true;
						}
					}
					if (flag == false)
						MessageBox(hWnd, "Coloca solo letras en el apartado de Numero de Medicos", "Nombre del Medico", MB_OK);
				}
			}
			///////////////Especialidades
			char Especialidad2[MAX_PATH];
			if (flag) {
				length = 0;
				HWND Especialidades = GetDlgItem(hWnd, CB_NuevasEspecialidadMedico);
				int length = (int)GetWindowTextLength(Especialidades);


				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Especialidad""", "Tipo de Especialidad", MB_OK);
					flag = false;

				}
				else {
					flag = true;
				}

				if (flag) {
					GetWindowText(Especialidades, (LPSTR)Especialidad2, length + 1);
				}
			}
			//////////////Consultorios
			char Consultorio[MAX_PATH];
			if (flag) {
				if (IsDlgButtonChecked(hWnd, RA_NuevoConsultorio1) == BST_CHECKED || IsDlgButtonChecked(hWnd, RA_NuevoConsultorio2) == BST_CHECKED || IsDlgButtonChecked(hWnd, RA_NuevoConsultorio3) == BST_CHECKED) {


					if (IsDlgButtonChecked(hWnd, RA_NuevoConsultorio1) == BST_CHECKED) {
						strcpy_s(Consultorio, "1");
						flag = true;
					}
					else {

						if (IsDlgButtonChecked(hWnd, RA_NuevoConsultorio2) == BST_CHECKED) {
							strcpy_s(Consultorio, "2");
							flag = true;
						}
						else {
							if (IsDlgButtonChecked(hWnd, RA_NuevoConsultorio3) == BST_CHECKED) {
								strcpy_s(Consultorio, "3");
								flag = true;
							}
							else {
								flag = false;
							}
						}
					}
				}
				else {
					flag = false;
					MessageBox(hWnd, "No se coloco nada en la sección ""Numero de Consultorio""", "Nombre del Consultorio", MB_OK);

				}
			}
			////////////Telefono
			char NumeroTelefono[MAX_PATH];
			if (flag) {
				length = 0;
				HWND NumTel = GetDlgItem(hWnd, TEX_NuevoTelefonoMedico);
				int length = (int)GetWindowTextLength(NumTel);
				GetWindowTextA(NumTel, NumeroTelefono, length + 1);
				if (length == 0) {
					MessageBox(hWnd, "No se coloco nada en la sección ""Numero de Telefono""", "Telefono del Medico", MB_OK);
					flag = false;
				}
				else {
					if (length == 8 || length == 16) {
						flag = true;
					}
					else {

						MessageBox(hWnd, "Debe colocarse más de 8 o 15 numeros en el espacio de telefono", "Telefono del Medico", MB_OK);
						flag = false;
					}
				}

				if (flag) {
					for (int i = 0; i < length; i++) {
						if (NumeroTelefono[i] > 57 || NumeroTelefono[i] < 48) {
							flag = false;
						}
						else {
							flag = true;
						}
					}
					if (flag == false)
						MessageBox(hWnd, "Coloca solo numero en el apartado de Telefono", "Numero de Telefono", MB_OK);

				}
			}
			////////////////Horario
			char BuffHorario[MAX_PATH];
			if (flag) {
				length = 0;
				if (IsDlgButtonChecked(hWnd, RAD_NuevoManana) == BST_CHECKED || IsDlgButtonChecked(hWnd, RAD_NuevoTarde) == BST_CHECKED) {
					if (IsDlgButtonChecked(hWnd, RAD_NuevoManana) == BST_CHECKED) {
						strcpy_s(BuffHorario, "1");
						flag = true;
					}
					else {
						if (IsDlgButtonChecked(hWnd, RAD_NuevoTarde) == BST_CHECKED) {
							flag = true;
							strcpy_s(BuffHorario, "2");
						}
						else {
							flag = false;
						}
					}
				}
				else {
					flag = false;
					MessageBox(hWnd, "No se selecciono ninguna opción en horario del medico", "Medicos", MB_OK);
				}
			}
			/////////////////Semana
			bool BuffLunes = false;
			bool BuffMartes = false;
			bool BuffMiercoles = false;
			bool BuffJueves = false;
			bool BuffViernes = false;



			if (flag) {
				length = 0;
				if (IsDlgButtonChecked(hWnd, CHECK_LUNESMEDICOS) == BST_CHECKED || IsDlgButtonChecked(hWnd, CHECK_MARTESMEDICOS) == BST_CHECKED || IsDlgButtonChecked(hWnd, CHECK_MIERCOLESMEDICOS) == BST_CHECKED || IsDlgButtonChecked(hWnd, CHECK_JUEVESMEDICOS) == BST_CHECKED || IsDlgButtonChecked(hWnd, CHECK_VIERNESMEDICOS) == BST_CHECKED) {
					if (IsDlgButtonChecked(hWnd, CHECK_LUNESMEDICOS) == BST_CHECKED) {
						BuffLunes = true;
						flag = true;
					}
					if (IsDlgButtonChecked(hWnd, CHECK_MARTESMEDICOS) == BST_CHECKED) {

						flag = true;
						BuffMartes = true;
					}
					if (IsDlgButtonChecked(hWnd, CHECK_MIERCOLESMEDICOS) == BST_CHECKED) {

						flag = true;
						BuffMiercoles = true;
					}
					if (IsDlgButtonChecked(hWnd, CHECK_JUEVESMEDICOS) == BST_CHECKED) {

						flag = true;
						BuffJueves = true;
					}
					if (IsDlgButtonChecked(hWnd, CHECK_VIERNESMEDICOS) == BST_CHECKED) {

						flag = true;
						BuffViernes = true;
					}
				}
				else {
					flag = false;
					MessageBox(hWnd, "No se selecciono ninguna opción en días de la semana", "Medicos", MB_OK);
				}
			}
			if (flag) {
				Medico* reservMeHo = raiz;
				AMedicos->identificadorMedicos(reservMeHo, Consultorio, BuffHorario, BuffLunes, BuffMartes, BuffMiercoles, BuffJueves, BuffViernes, hWnd);

			}
			///////////////////////////////////////////////
			if (flag) {

				if (strcmp(chFileName, "") != 0) {
					flag = true;

				}
				else
				{
					MessageBox(hWnd, "Hace falta dar de alta una imagen", "Medicos", MB_OK);

					flag = false;
				}
			}
			////////////////Mensaje final
			if (flag) {
				Medico* Reserv = raiz;
				int intcedulas = atoi(BufferCedulaMedico);
				if (intcedulas != Reserv->IntCedula) {
					buscadorCedula(Reserv, intcedulas, hWnd);

				}
				Registros* auxRe = originR;
				while (strcmp(Reserv->nombre, auxRe->nombreMedico) != 0) {
					auxRe = auxRe->next;
				}

				strcpy_s(auxRe->nombreMedico, 250, NombreMedico);
				strcpy_s(Reserv->nombre, 250, NombreMedico);
				strcpy_s(Reserv->especialidad, 250, Especialidad2);
				strcpy_s(Reserv->numconsultorio, Consultorio);
				strcpy_s(Reserv->telefono, 250, NumeroTelefono);
				strcpy_s(Reserv->chFileName, 250, chFileName);
				strcpy_s(Reserv->horario, 250, BuffHorario);
				Reserv->Lunes = BuffLunes;
				Reserv->Martes = BuffMartes;
				Reserv->Miercoles = BuffMiercoles;
				Reserv->Jueves = BuffJueves;
				Reserv->Viernes = BuffViernes;
			}
			else {
				flag = false;
			}
			if (flag)
				MessageBox(hWnd, "Se dio de alta al medico", "Medicos", MB_OK);
			HWND RecargarAltas = CreateDialog(hGlobalInst, MAKEINTRESOURCE(SDLG_ALTASBAJASCAMBIOS), NULL, AltasBajasCambios);
			ShowWindow(RecargarAltas, SW_SHOW);
			DestroyWindow(hWnd);
		}
		
	}
	
		break;

	case WM_DESTROY:
		if (CIERRE)
			PostQuitMessage(10);
		break;
	case WM_CLOSE:
		CIERRE = true;
		DestroyWindow(hWnd);

		break;
	}
	return FALSE;
}








