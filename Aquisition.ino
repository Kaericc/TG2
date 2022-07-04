#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// Atribuindo um ID exclusivo ao sensor 
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

//Frequência de aquisição: 2000 rpm = 33,33Hz -- Nyquist -> f = 14,7e-3
int freq = 0.1;
int boot = 0;

// Print inicial após a inicialização identificação a configuração utilizada nos acelerômetros
void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

// Informações básicas em relação ao DataRate utilizado
void displayDataRate(void)
{
  Serial.print  ("Data Rate:    "); 
  
  switch(accel.getDataRate())
  {
    case ADXL345_DATARATE_3200_HZ:
      Serial.print  ("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      Serial.print  ("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      Serial.print  ("800 "); 
      break;
    default:
      Serial.print  ("???? "); 
      break;
  }  
  Serial.println(" Hz");  
}

// Informações em relação a sensibilidade utilizada

void displayRange(void)
{
  Serial.print  ("Range:         +/- "); 
  
  switch(accel.getRange())
  {
    case ADXL345_RANGE_16_G:
      Serial.print  ("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      Serial.print  ("8 "); 
      break;
    default:
      Serial.print  ("?? "); 
      break;
  }  
  Serial.println(" g");  
}

void setup(void) {
  Serial.begin(9600);
  // Definição do tempo Deep Sleep, no caso, 10s
  esp_sleep_enable_timer_wakeup(10*1000000); 
  Serial.println("Teste acelerômetro"); Serial.println("");
  
  // Inicialização do acelerômetro
  if(!accel.begin()){
    // Caso haja problema de identificação do ADXL345
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  // Definição da sensibilidade que será utilizada na aquisição dos dados
  
  //accel.setRange(ADXL345_RANGE_16_G);
   accel.setRange(ADXL345_RANGE_8_G);
  
  // Mostrar as informações básicas em relação ao funcionamento do sensor
  displaySensorDetails();
  displayDataRate();
  displayRange();
  Serial.println("");
}

void loop(void){

  // Número de coletas que serão realizadas antes de entrar no modo Deep Sleep, é fundamental que seja igual ao número de linhas utilizado na rotina Python  
  if(boot==6000){
    boot = 0;
    Serial.print('Sleeping');
    esp_deep_sleep_start();
  }
  else{

  
  // Inicia uma nova coleta com o sensor 
  sensors_event_t event; 
  accel.getEvent(&event);
 
  // Apresenta os dados no Monitor Serial, lembrar que a aceleração será apresentada em m/s^2 ou g
  Serial.print(event.acceleration.x); Serial.print(", "); Serial.print(event.acceleration.y); Serial.print(", "); Serial.print(event.acceleration.z); Serial.print("\n");
  delay(freq);
  boot = boot + 1;
  }
  
}
