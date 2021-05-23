#include <Time.h>
#include <TimeLib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// PINOS
#define PINO_BUZZER 0
#define PINO_RELE 1
#define PINO_BOTAO_1 2
#define PINO_BOTAO_2 3
#define PINO_BOTAO_3 4
#define PINO_BOTAO_4 5
#define PINO_SSR_1 6
#define PINO_SSR_2 7
#define PINO_SSR_3 8
#define PINO_SENSOR_TEMP 9

// CONSTS
#define TRUE 1
#define FALSE 0
#define ATIVO 1
#define INATIVO 0
#define MENU_PRINCIPAL 0
#define MENU_RAMPAS 1
#define MENU_TEMPERATURA 2
#define MENU_TEMPO 3
#define MENU_MASHOUT 4
#define MENU_MOSTURA 5
#define MENU_FERVURA 6
#define ETAPA_MOSTURA 7
#define ETAPA_MASHOUT 8
#define ETAPA_FERVURA 9
#define MENU_INICIAR_FERVURA 10
#define MENU_FINAL_FERVURA 11
#define ETAPA_RESFRIAMENTO 12
#define ETAPA_ADICAO_MALTE 13
#define MENU_MALTE_ADICIONADO 14
#define MENU_TEMPERATURA_CONSTANTE 15
#define ETAPA_TEMPERATURA_CONSTANTE 16
#define EXEC_RAMPA1 17
#define EXEC_RAMPA2 18
#define EXEC_RAMPA3 19
#define SUBIR_TEMP_RAMPA2 20
#define SUBIR_TEMP_RAMPA3 21
#define SUBIR_TEMP_MASHOUT 22
#define SUBIR_TEMP_FERVURA 23
#define EXEC_MASHOUT 24
#define EXEC_FERVURA 25
#define RAMPA1 0
#define RAMPA2 1
#define RAMPA3 2
#define MASHOUT 3
#define FERVURA 4
#define RESFRIAMENTO 5
#define MAX_RAMPAS 3
#define FATOR_CORRECAO_TEMPERATURA 2

OneWire temperatura_pino(PINO_SENSOR_TEMP);
DallasTemperature  temperatura_barramento(&temperatura_pino);
DeviceAddress temperatura_sensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte menu_item = MENU_PRINCIPAL;
byte sons = ATIVO;

unsigned int qtd_rampas = 1;
unsigned int temperatura[MAX_RAMPAS+3];
unsigned int tempo[MAX_RAMPAS+3];
unsigned int hora[MAX_RAMPAS+3];
unsigned int minuto[MAX_RAMPAS+3];

void menu(byte);
void menu_principal();
void menu_recebe_rampas();
void menu_recebe_temperatura();
void menu_recebe_tempo();
void menu_mashout();
void menu_mostura();
void menu_fervura();
void realiza_mostura();
void realiza_mashout();
void menu_iniciar_fervura();
void realiza_fervura();
void menu_final_fervura();
void realiza_resfriamento();
void realiza_adicao_malte();
void menu_malte_adicionado();
void menu_temperatura_constante();
void realiza_temperatura_constante();
void ajusta_temperatura_mashout();
void ajusta_temperatura_fervura();
void exibe_sim_nao(String);
void beep_pipi();
void beep_pi();
void beep_pi_longo();
void ativa_bomba();
void desativa_bomba();
void controla_temperatura(unsigned int);
void controle_comandos(byte*);
unsigned int controle_numeros(String, unsigned int);
void popup_msg(String, byte);
unsigned int realiza_rampa(unsigned int);
int tempo_restante(unsigned int);
int minuto_total(unsigned int, unsigned int);
void exibe_dados_alternados(unsigned int);
unsigned int leitura_temperatura();
void exibe_subindo_temperatura_alternados(unsigned int);
void exibe_subindo_temperatura_constante();
void exibe_temperatura_constante();

void setup() {
  temperatura[MASHOUT] = 32;
  temperatura[FERVURA] = 32;
  temperatura[RESFRIAMENTO] = 30;
  tempo[RESFRIAMENTO] = 30;
  
  temperatura_barramento.begin();
  temperatura_barramento.getAddress(temperatura_sensor, 1);
  
  lcd.init();
  lcd.backlight();

  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_BOTAO_1, INPUT);
  pinMode(PINO_BOTAO_2, INPUT);
  pinMode(PINO_BOTAO_3, INPUT);
  pinMode(PINO_BOTAO_4, INPUT);
  pinMode(PINO_SSR_1, OUTPUT);
  pinMode(PINO_SSR_2, OUTPUT);
  pinMode(PINO_SSR_3, OUTPUT);
  pinMode(PINO_SENSOR_TEMP, INPUT);
}

void loop() {
  menu(menu_item);
  delay(150);
}

void menu(byte item) {
  switch(menu_item) {
    case MENU_PRINCIPAL:
      menu_principal();
      break;
    case MENU_RAMPAS:
      menu_recebe_rampas();
      break;
    case MENU_TEMPERATURA:
      menu_recebe_temperatura();
      break;
    case MENU_TEMPO:
      menu_recebe_tempo();
      break;
    case MENU_MASHOUT:
      menu_mashout();
      break;
    case MENU_MOSTURA:
      menu_mostura();
      break;
    case MENU_FERVURA:
      menu_fervura();
      break;
    case ETAPA_MOSTURA:
      realiza_mostura();
      break;
    case ETAPA_MASHOUT:
      realiza_mashout();
      break;
    case MENU_INICIAR_FERVURA:
      menu_iniciar_fervura();
      break;
    case ETAPA_FERVURA:
      realiza_fervura();
      break;
    case MENU_FINAL_FERVURA:
      menu_final_fervura();
      break;
    case ETAPA_RESFRIAMENTO:
      realiza_resfriamento();
      break;
    case ETAPA_ADICAO_MALTE:
      realiza_adicao_malte();
      break;
    case MENU_MALTE_ADICIONADO:
      menu_malte_adicionado();
      break;
    case MENU_TEMPERATURA_CONSTANTE:
      menu_temperatura_constante();
      break;
    case ETAPA_TEMPERATURA_CONSTANTE:
      realiza_temperatura_constante();
      break;
    case SUBIR_TEMP_MASHOUT:
      ajusta_temperatura_mashout();
      break;
    case EXEC_MASHOUT:
      realiza_mashout();
      break;
    case SUBIR_TEMP_FERVURA:
      ajusta_temperatura_fervura();
      break;
    case EXEC_FERVURA:
      realiza_fervura();
      break;
    default:
      break;
  }
}

void menu_principal() {
  byte retorno = 0;

  exibe_sim_nao("BRASSAGEM:");
  controle_comandos(&retorno);
  
  if (retorno){
    menu_item = MENU_RAMPAS;
    if (sons==ATIVO) {
      beep_pi_longo();
    }
  }
  else if (retorno == 2) {
      menu_item = MENU_TEMPERATURA_CONSTANTE;
      if (sons==ATIVO) {
        beep_pipi();
      }
  }
  delay(100);
}

void menu_recebe_rampas() {
  qtd_rampas = controle_numeros("QTD DE RAMPAS:", 1);
  
  if (qtd_rampas < 1 || qtd_rampas > MAX_RAMPAS){
    popup_msg("QTD INCORRETA", 2);
    popup_msg("SETANDO PADRAO",2);
    popup_msg("RAMPAS = 1",2);
    qtd_rampas = 1;
  }

  menu_item = MENU_TEMPERATURA;
  lcd.clear();
}

void menu_recebe_temperatura() {
  String buffer PROGMEM;
  
  for(unsigned int i = 0; i < qtd_rampas; i++) {
    buffer = "TEMPERATURA";
    buffer += "("+String(i+1)+"):";
    temperatura[i] = controle_numeros(buffer, 2);
  }
   
  menu_item = MENU_TEMPO;
}

void menu_recebe_tempo() {
  String buffer PROGMEM;
    
  for(unsigned int i = 0; i < qtd_rampas; i++) {
    buffer = "TEMPO(min)";
    buffer += "("+ String(i+1)+"):";
    tempo[i] = controle_numeros(buffer, 3);
  }

  menu_item = MENU_MASHOUT;
}

void menu_mashout() {
  tempo[MASHOUT] = controle_numeros("MASHOUT(min):", 2);
  menu_item = MENU_FERVURA;
}

void menu_mostura() {
  byte retorno = 0;
  
  exibe_sim_nao("INICIAR MOSTURA");
  controle_comandos(&retorno);
  
  if(retorno) {
    menu_item = ETAPA_ADICAO_MALTE;
    if (sons == ATIVO) {
      beep_pi_longo();
      }
  }

  delay(100);
}

void menu_fervura() {
  tempo[FERVURA] = controle_numeros("FERVURA(min):", 3);
  menu_item = MENU_MOSTURA;
}

void realiza_mostura() {
}

void realiza_mashout() {
  if (!realiza_rampa(MASHOUT)) { 
      if (sons == ATIVO) {
        beep_pi_longo();
      }
      desativa_bomba();
      menu_item = MENU_INICIAR_FERVURA;
  }
}

void menu_iniciar_fervura() {
  byte retorno = 0;
  
  exibe_sim_nao("INICIAR FERVURA");
  controle_comandos(&retorno);

  if (retorno) {
    menu_item = SUBIR_TEMP_FERVURA;
    if (sons == ATIVO) {
      beep_pi_longo();
    }
  }

  delay(200);
}

void realiza_fervura() {
  if (tempo_restante(FERVURA) > 0) {
    ativa_bomba();
    exibe_dados_alternados(FERVURA);
  } else {
    if (sons == ATIVO){ 
      beep_pipi();
    }
    menu_item = MENU_FINAL_FERVURA;
    desativa_bomba();
  } 
}

void menu_final_fervura() {
  byte retorno = 0;
  exibe_sim_nao("RESFRIAMENTO:");
  controle_comandos(&retorno);

  if (sons == ATIVO) {
    beep_pipi();
  }
  
  if (retorno) {
    menu_item = ETAPA_RESFRIAMENTO;
  }

  delay(100);
}

void exibe_dados_alternados(unsigned int RAMPA) {
  String buffer PROGMEM;

  if((second() % 10) <= 5){
    lcd.clear();
    lcd.print("Temp(c) | Alvo:");
    lcd.setCursor(2, 1);
    buffer = String(leitura_temperatura()) + "    |   " + String(temperatura[RAMPA]);
    lcd.print(buffer);  
    delay(250);
  } else {
    lcd.clear();
    lcd.print("Tempo restante:");
    lcd.setCursor(2,1);
    int dif = tempo_restante(RAMPA);
    
    buffer = String(dif) + " minutos";
    lcd.print(buffer);
    delay(250);
  }
}

void realiza_resfriamento() {
  desativa_bomba();
  lcd.clear();
  lcd.print("RESFRIAMENTO >>");
  delay(5000);
  if (sons == ATIVO) { 
    beep_pi_longo();
  }  
}

void realiza_adicao_malte() {
  ativa_bomba();
  unsigned int temperatura_alvo = temperatura[RAMPA1] + FATOR_CORRECAO_TEMPERATURA; 
  controla_temperatura(temperatura_alvo);
  exibe_subindo_temperatura_alternados(RAMPA1);
  unsigned int temperatura_atual = leitura_temperatura();

  if (temperatura_atual >= temperatura_alvo) {
    menu_item = MENU_MALTE_ADICIONADO;
  }

  delay(250);
}

void menu_malte_adicionado() {
  byte retorno=0;
  
  if (sons == ATIVO) { 
    beep_pipi();
  }
  exibe_sim_nao("MALTE ADICIONADO");
  controle_comandos(&retorno);
  controla_temperatura(temperatura[RAMPA1] + FATOR_CORRECAO_TEMPERATURA); 

  if (retorno) {
    menu_item = ETAPA_MOSTURA;
    time_t t2 = calcula_horario(tempo[RAMPA1]);
    hora[RAMPA1] = hour();
    minuto[RAMPA1] = minute();
    if (sons == ATIVO) { 
      beep_pi_longo();
    }
  }

  delay(100);
}

void menu_temperatura_constante() {
  String buffer PROGMEM;
  
  temperatura[RAMPA1] = controle_numeros("TEMPERATURA FIXA",2);
  menu_item = ETAPA_TEMPERATURA_CONSTANTE;

  if (sons == ATIVO) {
    beep_pi_longo();
  }

  lcd.clear();
  lcd.print("CONSTANTE >>>>");
  delay(5000);
}

void realiza_temperatura_constante() {
  static byte atingida=0;
  
  controla_temperatura(temperatura[RAMPA1]);

  if (!atingida) {
    if (leitura_temperatura() < temperatura[RAMPA1]) {
      exibe_subindo_temperatura_constante(); 
    } else if (leitura_temperatura() == temperatura[RAMPA1]){
      atingida = 1;
      time_t t1 = now();
      hora[RAMPA1] = hour();
      minuto[RAMPA1] = minute();
    }
  } else if(atingida) {
    exibe_temperatura_constante();
  }
  
  delay(250);
}

void ajusta_temperatura_mashout() {
  unsigned int temperatura_atual = leitura_temperatura();
    
  controla_temperatura(temperatura[MASHOUT]);
  exibe_subindo_temperatura_alternados(MASHOUT);
  ativa_bomba();

  if (temperatura_atual >= temperatura[MASHOUT]) {
    time_t t2 = calcula_horario(tempo[MASHOUT]);
    hora[MASHOUT] = hour();
    minuto[MASHOUT] = minute();

    if (sons == ATIVO) {
      beep_pi_longo();
    }

    menu_item = EXEC_MASHOUT;
  }
}

void ajusta_temperatura_fervura() {
  unsigned int temperatura_atual = leitura_temperatura();
    
  controla_temperatura(temperatura[FERVURA]);
  exibe_subindo_temperatura_alternados(FERVURA);

  if (temperatura_atual >= temperatura[FERVURA]) {
    time_t t2 = calcula_horario(tempo[FERVURA]);
    hora[FERVURA] = hour();
    minuto[FERVURA] = minute();
    if (sons == ATIVO) {
      beep_pi_longo();
    }
    menu_item = EXEC_FERVURA;
  }
}

void exibe_sim_nao(String titulo) {
  lcd.clear();
  lcd.print(titulo);
  lcd.setCursor(2,1); 
  lcd.print("1=SIM | 2=NAO");
  delay(150);
}

void beep_pipi() {
  digitalWrite(PINO_BUZZER, LOW);
  delay(200);
  digitalWrite(PINO_BUZZER, HIGH);
  delay(50);
  digitalWrite(PINO_BUZZER, LOW);
  delay(200);
  digitalWrite(PINO_BUZZER, HIGH);
  delay(50);
}

void beep_pi() {
  digitalWrite(PINO_BUZZER, LOW);
  delay(200);
  digitalWrite(PINO_BUZZER, HIGH);
  delay(50);
}


void beep_pi_longo() {
  digitalWrite(PINO_BUZZER, LOW);
  delay(400);
  digitalWrite(PINO_BUZZER, HIGH);
  delay(50);
}

int calcula_horario(unsigned int minutos) {
  return minute() - minutos;
}

void ativa_bomba() {
  digitalWrite(PINO_RELE, LOW);
}

void desativa_bomba() {
  digitalWrite(PINO_RELE, HIGH);
}

void controla_temperatura(unsigned int TEMP_CONTROLE) {
  unsigned int temperatura_atual = leitura_temperatura();
  if (temperatura_atual < TEMP_CONTROLE) { 
    ativa_bomba();
  } else {
    desativa_bomba();
  }
}

void controle_comandos(byte *valor) {
  int botao1 = digitalRead(PINO_BOTAO_1);
  int botao2 = digitalRead(PINO_BOTAO_2);

  *valor = 0;

  if (botao1 == HIGH) {
    *valor += 1;
  }

  if (botao2 == HIGH) {
    *valor -= 1;
  }
  delay(100);
}

unsigned int controle_numeros(String titulo, unsigned int qtd_digitos) {
  static unsigned int i = 0;
  static char interno[4];
  for(int j = 0; j <= 4; j++) {
    interno[j] = ' ';
  }

  if (i > qtd_digitos) {
    i = 0;
    lcd.setCursor(0, 1);
    for(int j = 0; j <= qtd_digitos; j++) {
      interno[j] = ' ';
    }
  }
    
  lcd.clear();
  lcd.print(titulo);
  lcd.setCursor(0,1);
  for(int j = 0; j <= qtd_digitos; j++) {
    if (interno[j] != ' ') {
      lcd.print(interno[j]);
    }
  }
  delay(100);
  return 0;
}

void popup_msg(String msg, byte tempo) {
  lcd.clear();
  lcd.print(msg);
  delay(500*tempo);
}

unsigned int realiza_rampa(unsigned int RAMPA) {
  if (tempo_restante(RAMPA) > 0) {
    controla_temperatura(temperatura[RAMPA]);
    exibe_dados_alternados(RAMPA);
    
    ativa_bomba();
    
    return 1;
  }
  return 0;
}

int tempo_restante(unsigned int num_rampa) {
  int r1 = minuto_total(hour(), minute());
  int r2 = minuto_total(hora[num_rampa], minuto[num_rampa]);
  
  int dif = (r2 - r1);
  return dif + 1;
}

int minuto_total(unsigned int hora, unsigned int minuto) {
  int r = (hora*60) + minuto;
  return r;
}

unsigned int leitura_temperatura() { 
  temperatura_barramento.requestTemperatures(); 

  unsigned int temperatura_retorno = temperatura_barramento.getTempC(temperatura_sensor);

  if (temperatura_retorno > 100 || temperatura_retorno < 0) {
    temperatura_retorno=20;
  }

  return temperatura_retorno;
}

void exibe_subindo_temperatura_alternados(unsigned int RAMPA) {
  String buffer PROGMEM;
  
  if ((second() % 10) <= 5){
    lcd.clear();
    lcd.print("Temp(c) | Alvo:");
    lcd.setCursor(2,1);
  
    if (RAMPA == RAMPA1) {
      buffer = String(leitura_temperatura()) + "    |   " + String(temperatura[RAMPA1]+FATOR_CORRECAO_TEMPERATURA);
    } else {
      buffer = String(leitura_temperatura()) + "    |   " + String(temperatura[RAMPA]);
    }
    
    lcd.print(buffer);  
    delay(250);
  } else {
    lcd.clear();
    if (RAMPA == MASHOUT) {
      lcd.print("MASHOUT> Subindo");
    } else if (RAMPA == FERVURA) {
      lcd.print("FERVURA> Subindo");
    } else if (RAMPA == RAMPA1) {
      lcd.print("RAMPA N1> Subindo");
    } else if (RAMPA == RAMPA2) {
      lcd.print("RAMPA N2> Subindo");
    } else if (RAMPA == RAMPA3) {
      lcd.print("RAMPA N3> Subindo");
    }
    
    lcd.setCursor(0,1);
    lcd.print("temperatura...");
    delay(250);
  }
}

void exibe_subindo_temperatura_constante() {
  String buffer PROGMEM;
  
  if ((second() % 10) <= 5){
    lcd.clear();
    lcd.print("Temp(c) | Alvo:");
    lcd.setCursor(2, 1);
    buffer = String(leitura_temperatura()) + "    |   " + String(temperatura[RAMPA1]);
    lcd.print(buffer);  
    delay(250);
  } else {
    lcd.clear();
    lcd.print("CONSTANTE >>>>");
    lcd.setCursor(0, 1);
    lcd.print("Subindo temp...");
    delay(250);
  }
}

void exibe_temperatura_constante() {
  String buffer PROGMEM;
  static byte temperatura_atingida = 0;

  if((second() % 10) <= 5){
    lcd.clear();
    lcd.print("Temp(c) | Alvo:");
    lcd.setCursor(2, 1);
    buffer=String(leitura_temperatura()) + "    |   " + String(temperatura[RAMPA1]);
    lcd.print(buffer);  
    delay(250);
  } else {
    lcd.clear();
    lcd.print("Transcorrido:");
    lcd.setCursor(2, 1);
  
    int dif, r1, r2;
    
    r1 = minuto_total(hour(), minute());
    r2 = minuto_total(hora[RAMPA1], minuto[RAMPA1]);
    dif = (r1 - r2);
    
    buffer = String(dif) + " minutos";
    lcd.print(buffer);
    delay(250);
  }
}
