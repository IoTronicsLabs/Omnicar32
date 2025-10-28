#include <Bluepad32.h>

// ===== CONFIGURACIÓN DE PINES DEL MOTOR =====
// Motor Izquierdo (Motor A)
const int motorIzqIN1 = 27;  // Control de dirección
const int motorIzqIN2 = 26;  // Control de dirección
const int motorIzqENA = 14;  // Control de velocidad (PWM)

// Motor Derecho (Motor B)
const int motorDerIN3 = 25;  // Control de dirección
const int motorDerIN4 = 33;  // Control de dirección  
const int motorDerENB = 32;  // Control de velocidad (PWM)

// ===== CONFIGURACIÓN PWM =====
const int frecuenciaPWM = 1000;     // Frecuencia en Hz
const int canalPWM_Izq = 0;         // Canal PWM para motor izquierdo
const int canalPWM_Der = 1;         // Canal PWM para motor derecho
const int resolucion = 8;           // Resolución de 8 bits (0-255)

// ===== VARIABLES DEL CONTROLADOR =====
ControllerPtr miMando[BP32_MAX_GAMEPADS];

// ===== FUNCIONES DE CALLBACK PARA CONEXIÓN/DESCONEXIÓN =====
void mandoConectado(ControllerPtr ctl) {
  bool espacioEncontrado = false;
  
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (miMando[i] == nullptr) {
      Serial.printf("Mando conectado en slot %d\n", i);
      
      // Obtener propiedades del mando
      ControllerProperties props = ctl->getProperties();
      Serial.printf("Modelo: %s, VID=0x%04x, PID=0x%04x\n", 
                    ctl->getModelName().c_str(), 
                    props.vendor_id, 
                    props.product_id);
      
      miMando[i] = ctl;
      espacioEncontrado = true;
      break;
    }
  }
  
  if (!espacioEncontrado) {
    Serial.println("Mando conectado pero no hay espacio disponible");
  }
}

void mandoDesconectado(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (miMando[i] == ctl) {
      Serial.printf("Mando desconectado del slot %d\n", i);
      miMando[i] = nullptr;
      
      // Detener motores por seguridad
      detenerMotores();
      break;
    }
  }
}

// ===== FUNCIONES DE CONTROL DE MOTORES =====
void detenerMotores() {
  digitalWrite(motorIzqIN1, LOW);
  digitalWrite(motorIzqIN2, LOW);
  digitalWrite(motorDerIN3, LOW);
  digitalWrite(motorDerIN4, LOW);
  ledcWrite(canalPWM_Izq, 0);
  ledcWrite(canalPWM_Der, 0);
}

void avanzar(int velocidad) {
  // Motor izquierdo hacia adelante
  digitalWrite(motorIzqIN1, HIGH);
  digitalWrite(motorIzqIN2, LOW);
  
  // Motor derecho hacia adelante
  digitalWrite(motorDerIN3, HIGH);
  digitalWrite(motorDerIN4, LOW);
  
  // Establecer velocidad
  ledcWrite(canalPWM_Izq, velocidad);
  ledcWrite(canalPWM_Der, velocidad);
}

void retroceder(int velocidad) {
  // Motor izquierdo hacia atrás
  digitalWrite(motorIzqIN1, LOW);
  digitalWrite(motorIzqIN2, HIGH);
  
  // Motor derecho hacia atrás
  digitalWrite(motorDerIN3, LOW);
  digitalWrite(motorDerIN4, HIGH);
  
  // Establecer velocidad
  ledcWrite(canalPWM_Izq, velocidad);
  ledcWrite(canalPWM_Der, velocidad);
}

void girarIzquierda(int velocidad) {
  // Motor izquierdo hacia atrás
  digitalWrite(motorIzqIN1, LOW);
  digitalWrite(motorIzqIN2, HIGH);
  
  // Motor derecho hacia adelante
  digitalWrite(motorDerIN3, HIGH);
  digitalWrite(motorDerIN4, LOW);
  
  // Establecer velocidad
  ledcWrite(canalPWM_Izq, velocidad);
  ledcWrite(canalPWM_Der, velocidad);
}

void girarDerecha(int velocidad) {
  // Motor izquierdo hacia adelante
  digitalWrite(motorIzqIN1, HIGH);
  digitalWrite(motorIzqIN2, LOW);
  
  // Motor derecho hacia atrás
  digitalWrite(motorDerIN3, LOW);
  digitalWrite(motorDerIN4, HIGH);
  
  // Establecer velocidad
  ledcWrite(canalPWM_Izq, velocidad);
  ledcWrite(canalPWM_Der, velocidad);
}

// ===== PROCESAR COMANDOS DEL MANDO =====
void procesarMando(ControllerPtr ctl) {
  // Obtener valores del joystick izquierdo
  int joystickX = ctl->axisX();  // Rango: -511 a 512 (izquierda/derecha)
  int joystickY = ctl->axisY();  // Rango: -511 a 512 (arriba/abajo)
  
  // Zona muerta del joystick (evita movimientos involuntarios)
  const int zonaMuerta = 50;
  
  // Calcular velocidad basada en la posición del joystick
  // Convertir de rango -511 a 512 → 0 a 255
  int velocidad = 0;
  
  // ===== CONTROL VERTICAL (Adelante/Atrás) =====
  if (joystickY < -zonaMuerta) {
    // Joystick hacia arriba = Avanzar
    velocidad = map(abs(joystickY), zonaMuerta, 511, 100, 255);
    velocidad = constrain(velocidad, 100, 255);
    
    if (joystickX < -zonaMuerta) {
      // Avanzar girando a la izquierda
      ledcWrite(canalPWM_Izq, velocidad * 0.5);
      ledcWrite(canalPWM_Der, velocidad);
      digitalWrite(motorIzqIN1, HIGH);
      digitalWrite(motorIzqIN2, LOW);
      digitalWrite(motorDerIN3, HIGH);
      digitalWrite(motorDerIN4, LOW);
    } 
    else if (joystickX > zonaMuerta) {
      // Avanzar girando a la derecha
      ledcWrite(canalPWM_Izq, velocidad);
      ledcWrite(canalPWM_Der, velocidad * 0.5);
      digitalWrite(motorIzqIN1, HIGH);
      digitalWrite(motorIzqIN2, LOW);
      digitalWrite(motorDerIN3, HIGH);
      digitalWrite(motorDerIN4, LOW);
    }
    else {
      // Avanzar recto
      avanzar(velocidad);
    }
    
    Serial.printf("Avanzar - Velocidad: %d, JoyX: %d, JoyY: %d\n", velocidad, joystickX, joystickY);
  }
  else if (joystickY > zonaMuerta) {
    // Joystick hacia abajo = Retroceder
    velocidad = map(abs(joystickY), zonaMuerta, 511, 100, 255);
    velocidad = constrain(velocidad, 100, 255);
    
    if (joystickX < -zonaMuerta) {
      // Retroceder girando a la izquierda
      ledcWrite(canalPWM_Izq, velocidad * 0.5);
      ledcWrite(canalPWM_Der, velocidad);
      digitalWrite(motorIzqIN1, LOW);
      digitalWrite(motorIzqIN2, HIGH);
      digitalWrite(motorDerIN3, LOW);
      digitalWrite(motorDerIN4, HIGH);
    }
    else if (joystickX > zonaMuerta) {
      // Retroceder girando a la derecha
      ledcWrite(canalPWM_Izq, velocidad);
      ledcWrite(canalPWM_Der, velocidad * 0.5);
      digitalWrite(motorIzqIN1, LOW);
      digitalWrite(motorIzqIN2, HIGH);
      digitalWrite(motorDerIN3, LOW);
      digitalWrite(motorDerIN4, HIGH);
    }
    else {
      // Retroceder recto
      retroceder(velocidad);
    }
    
    Serial.printf("Retroceder - Velocidad: %d, JoyX: %d, JoyY: %d\n", velocidad, joystickX, joystickY);
  }
  // ===== CONTROL HORIZONTAL (Girar en su lugar) =====
  else if (joystickX < -zonaMuerta) {
    // Joystick hacia la izquierda = Girar a la izquierda
    velocidad = map(abs(joystickX), zonaMuerta, 511, 100, 255);
    velocidad = constrain(velocidad, 100, 255);
    girarIzquierda(velocidad);
    Serial.printf("Girar Izquierda - Velocidad: %d\n", velocidad);
  }
  else if (joystickX > zonaMuerta) {
    // Joystick hacia la derecha = Girar a la derecha
    velocidad = map(abs(joystickX), zonaMuerta, 511, 100, 255);
    velocidad = constrain(velocidad, 100, 255);
    girarDerecha(velocidad);
    Serial.printf("Girar Derecha - Velocidad: %d\n", velocidad);
  }
  // ===== JOYSTICK EN ZONA MUERTA (Detener) =====
  else {
    detenerMotores();
  }
  
  // ===== CONTROL CON BOTONES (OPCIONAL) =====
  if (ctl->buttons() == 0x0001) {
    Serial.println("Modo TURBO activado!");
  }
}

// ===== PROCESAR TODOS LOS MANDOS CONECTADOS =====
void procesarMandos() {
  for (auto mando : miMando) {
    if (mando && mando->isConnected() && mando->hasData()) {
      if (mando->isGamepad()) {
        procesarMando(mando);
      } else {
        Serial.println("Dispositivo no soportado");
      }
    }
  }
}

// ===== CONFIGURACIÓN INICIAL (SE EJECUTA UNA VEZ) =====
void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);
  Serial.printf("Firmware Bluepad32: %s\n", BP32.firmwareVersion());
  
  // Mostrar dirección Bluetooth
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("Dirección BT: %2X:%2X:%2X:%2X:%2X:%2X\n", 
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  
  // ===== CONFIGURAR PINES DE LOS MOTORES =====
  pinMode(motorIzqIN1, OUTPUT);
  pinMode(motorIzqIN2, OUTPUT);
  pinMode(motorDerIN3, OUTPUT);
  pinMode(motorDerIN4, OUTPUT);
  
  // ===== CONFIGURAR PWM PARA CONTROL DE VELOCIDAD (MÉTODO ANTIGUO) =====
  // Este método es compatible con versiones antiguas del ESP32 core
  ledcSetup(canalPWM_Izq, frecuenciaPWM, resolucion);
  ledcSetup(canalPWM_Der, frecuenciaPWM, resolucion);
  
  ledcAttachPin(motorIzqENA, canalPWM_Izq);
  ledcAttachPin(motorDerENB, canalPWM_Der);
  
  // Asegurar que los motores estén detenidos al inicio
  detenerMotores();
  
  // ===== CONFIGURAR BLUEPAD32 =====
  BP32.setup(&mandoConectado, &mandoDesconectado);
  
  // Olvidar mandos emparejados previamente (útil para pruebas)
  BP32.forgetBluetoothKeys();
  
  // Deshabilitar mouse virtual
  BP32.enableVirtualDevice(false);
  
  Serial.println("Sistema inicializado. Esperando conexión del mando...");
}

// ===== BUCLE PRINCIPAL (SE EJECUTA CONTINUAMENTE) =====
void loop() {
  // Actualizar datos de Bluepad32
  bool datosActualizados = BP32.update();
  
  if (datosActualizados) {
    procesarMandos();
  }
  
  // Pequeño delay para estabilidad
  delay(10);
}
