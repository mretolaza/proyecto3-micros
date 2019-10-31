/****************************************
UNIVERSIDAD DEL VALLE DE GUATEMALA

Proyecto 3, Sensor DTH22
Abril Palencia - 18198
Cristina Bautista - 161260
Maria Mercedes Retolaza - 16339

Descripción: En el siguiente programa se realizará 
la obtención de los datos a utilizar. Se obtendrá la 
temperatura y humedad por cada 2 segundos. 

Meta: 75000 datos (min)
****************************************/

#include <DHT.h> //definicion de protocolo
#define DHTPIN 2 //definicion de pin
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#include <SPI.h>   //definicion de procolo
#include <SD.h>    //uso de tarjeta SD
File project3Data; //nombre del archivo

//definicion de horas (se define el punto de partida
//en formato de fecha)
#define HOUR 11
#define MINUTE 55
#define DAY 22
#define MONTH 6
#define YEAR 19

//definicion de variables de tiempo en 32 bits y en 8 bit
uint32_t time_counter, t2_count;
uint8_t mins, hours, days, months, years;
bool state;
bool sd_data;

//variables de temperatura
float hum, tempC, tempF;
String file_name = "Hum";
bool flag_name;
uint16_t counter;
void (*reset)(void) = 0;
void setup()
//Calculo de inicio de programa
{
  Serial.begin(9600);
  mins = MINUTE;
  hours = HOUR;
  months = MONTH;
  years = YEAR;
  days = DAY;
  t2_count = 0;
  state = false;
  sd_init();
  dht.begin();
  Serial.println("Inicio de programa");
  sd_data = true;
  delay(2000);
  dht_read();
}

//Subrutina de tiempo
void loop()
{
  t2_count = millis();
  if ((t2_count % 2000) == 0)
  {
    Serial.println(t2_count);
    time_counter += 2;
    Serial.print(time_counter);
    Serial.println(F(" Segundos"));
    Serial.print(mins);
    Serial.println(F(" Minutos"));
    Serial.print(hours);
    Serial.println(F(" horas"));
    dht_read();
    sd_data = true;
  }
  get_time();
  if (sd_data == true)
  {
    Serial.print(time_counter);
    Serial.println(F(" Segundos"));
    Serial.print(mins);
    Serial.println(F(" Minutos"));
    Serial.print(hours);
    Serial.println(F(" horas"));
    data_write();
  }
  sd_data = false;
}

//evaluar el tiempo
void get_time(void)
{
  if (time_counter == 60)
  {
    mins++;
    time_counter = 0;
  }
  if (mins == 60)
  {
    hours++;
    mins = 0;
  }

  if (hours == 24)
  {
    days++;
    hours = 0;
  }
  switch (months)
  {
  case 1:
    if (days == 32)
    {
      days = 1;
      months++;
    }
    break;

  case 2:
    if (days == 29)
    {
      days = 1;
      months++;
    }
    break;
  case 3:
    if (days == 32)
    {
      days = 1;
      months++;
    }
    break;
  case 4:
    if (days == 31)
    {
      days = 1;
      months++;
    }
    break;
  case 5:
    if (days == 32)
    {
      days = 1;
      months++;
    }
    break;
  case 6:
    if (days == 31)
    {
      days = 1;
      months++;
    }
    break;
  case 7:
    if (days == 32)
    {
      days = 1;
      months++;
    }
    break;
  case 8:
    if (days == 32)
    {
      days = 1;
      months++;
    }
    break;
  case 9:
    if (days == 31)
    {
      days = 1;
      months++;
    }
    break;
  case 10:
    if (days == 32)
    {
      days = 1;
      months++;
    }
    break;
  case 11:
    if (days == 31)
    {
      days = 1;
      months++;
    }
    break;
  case 12:
    if (days == 32)
    {
      days = 1;
      months = 1;
      years++;
    }
    break;
  }
}

//impresion de datos
void dht_read(void)
{
  hum = dht.readHumidity();
  tempC = dht.readTemperature();
  tempF = dht.readTemperature(true);

  if (isnan(hum) || isnan(tempC) || isnan(tempF))
  {
    Serial.println(F("Error en sensor"));
    return;
  }

  Serial.print(F("Humedad: "));
  Serial.print(hum);
  Serial.println(F("%"));
  Serial.print(F("Temperatura: "));
  Serial.print(tempC);
  Serial.println(F("C"));
  Serial.println(F("-------------------------"));
}

void sd_init(void)
{

  if (!SD.begin(4))
  {
    Serial.println(F("Error en SD"));
    while (1)
      ;
    delay(1000);
    reset();
  }
  Serial.println(F("SD iniciada"));
  for (int i = 0; i < 1000; i++)
  {
    file_name = "Hum";
    file_name.concat(i);
    file_name.concat(F(".csv"));
    if (SD.exists(file_name.c_str()))
    {
      Serial.println(F("Si existe"));
      flag_name = false;
    }
    delay(250);
    if (!SD.exists(file_name.c_str()))
    {
      Serial.println(F("No existe"));
      flag_name = true;
      break;
    }
  }
  project3Data = SD.open(file_name.c_str(), FILE_WRITE);
  if (project3Data)
  {
    Serial.println(F("Escribiendo en SD"));
    project3Data.println(F("Hora, fecha, humedad, temperatura"));
    project3Data.close();
    Serial.println(F("Archivo cerrado"));
  }
  else
  {
    Serial.println(F("Error creando el archivo: "));
    delay(1000);
    reset();
  }
}
void data_write(void)
{
  project3Data = SD.open(file_name.c_str(), FILE_WRITE);
  if (project3Data)
  {
    Serial.println(F("Escribiendo en SD"));
    project3Data.print(hours);
    project3Data.print(F(":"));
    project3Data.print(mins);

    project3Data.print(F(";"));

    project3Data.print(days);
    project3Data.print(F("/"));
    project3Data.print(months);
    project3Data.print(F("/"));
    project3Data.print(years);

    project3Data.print(F(";"));

    project3Data.print(hum);
    project3Data.print(F(";"));
    project3Data.print(tempC);

    project3Data.println(F(";"));

    project3Data.close();
    Serial.println(F("Archivo cerrado"));
  }
  else
  {
    Serial.println("Error al abrir el archivo");
  }
}
