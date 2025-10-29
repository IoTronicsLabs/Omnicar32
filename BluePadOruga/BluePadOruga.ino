#include <Bluepad32.h>

// ===== CONFIGURACIÓN DE PINES DEL MOTOR =====
// Motor Izquierdo (Motor A)
const int motorIzqIN1 = 27;
const int motorIzqIN2 = 26;
const int motorIzqENA = 14;

// Motor Derecho (Motor B)
const int motorDerIN3 = 25;
const int motorDerIN4 = 33;
const int motorDerENB = 32;

// ===== CONFIGURACIÓN PWM =====
const int frecuenciaPWM = 1000;
const int canalPWM_Izq = 0;
const int canalPWM_Der = 1;
const int resolucion = 8;

// ===== CONFIGURACIÓN DE CONTROL SUAVE =====
const int zonaMuerta = 80;              // Zona muerta ampliada para mejor control
const int velocidadMinima = 80;         // Velocidad mínima para que el motor se mueva
const int velocidadMaxima = 255;        // Velocidad máxima
const float tasaAceleracion = 0.15;     // Incremento gradual (0.1 = más suave, 0.3 = más rápido)
const float tasaFrenado = 0.35;         // Frenado más rápido que aceleración
const float factorCurva = 0.4;          // Reducción de velocidad en curvas (0.3-0.6 recomendado)
const float filtroSuavizado = 0.25;     // Filtro para entrada del joystick (0.1-0.3)

// ===== VARIABLES DE ESTADO =====
int velocidadIzqActual = 0;
int velocidadDerActual = 0;
int velocidadIzqObjetivo = 0;
int velocidadDerObjetivo = 0;

// Filtro de entrada
float joystickXFiltrado = 0;
float joystickYFiltrado = 0;

// ===== VARIABLES DEL CONTROLADOR =====
ControllerPtr miMando[BP32_MAX_GAMEPADS];

// ===== FUNCIONES DE CALLBACK =====
void mandoConectado(ControllerPtr ctl) {
  bool espacioEncontrado = false;
  
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (miMando[i] == nullptr) {
      Serial.printf("Mando conectado en slot %d\n", i);
      
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
      detenerMotoresInmediato();
      break;
    }
  }
}

// ===== FUNCIONES DE CONTROL DE MOTORES =====
void detenerMotoresInmediato() {
  digitalWrite(motorIzqIN1, LOW);
  digitalWrite(motorIzqIN2, LOW);
  digitalWrite(motorDerIN3, LOW);
  digitalWrite(motorDerIN4, LOW);
  ledcWrite(canalPWM_Izq, 0);
  ledcWrite(canalPWM_Der, 0);
  
  // Resetear velocidades
  velocidadIzqActual = 0;
  velocidadDerActual = 0;
  velocidadIzqObjetivo = 0;
  velocidadDerObjetivo = 0;
}

void establecerDireccionMotor(bool izquierdo, int velocidad) {
  if (izquierdo) {
    // Motor izquierdo
    if (velocidad > 0) {
      digitalWrite(motorIzqIN1, HIGH);
      digitalWrite(motorIzqIN2, LOW);
    } else if (velocidad < 0) {
      digitalWrite(motorIzqIN1, LOW);
      digitalWrite(motorIzqIN2, HIGH);
    } else {
      digitalWrite(motorIzqIN1, LOW);
      digitalWrite(motorIzqIN2, LOW);
    }
  } else {
    // Motor derecho
    if (velocidad > 0) {
      digitalWrite(motorDerIN3, HIGH);
      digitalWrite(motorDerIN4, LOW);
    } else if (velocidad < 0) {
      digitalWrite(motorDerIN3, LOW);
      digitalWrite(motorDerIN4, HIGH);
    } else {
      digitalWrite(motorDerIN3, LOW);
      digitalWrite(motorDerIN4, LOW);
    }
  }
}

void actualizarVelocidadesGradual() {
  // Determinar tasa de cambio (frenado más rápido que aceleración)
  float tasaIzq = (abs(velocidadIzqObjetivo) < abs(velocidadIzqActual)) ? tasaFrenado : tasaAceleracion;
  float tasaDer = (abs(velocidadDerObjetivo) < abs(velocidadDerActual)) ? tasaFrenado : tasaAceleracion;
  
  // Motor izquierdo
  if (velocidadIzqActual < velocidadIzqObjetivo) {
    velocidadIzqActual += max(1, (int)((velocidadIzqObjetivo - velocidadIzqActual) * tasaIzq));
    if (velocidadIzqActual > velocidadIzqObjetivo) velocidadIzqActual = velocidadIzqObjetivo;
  } else if (velocidadIzqActual > velocidadIzqObjetivo) {
    velocidadIzqActual -= max(1, (int)((velocidadIzqActual - velocidadIzqObjetivo) * tasaIzq));
    if (velocidadIzqActual < velocidadIzqObjetivo) velocidadIzqActual = velocidadIzqObjetivo;
  }
  
  // Motor derecho
  if (velocidadDerActual < velocidadDerObjetivo) {
    velocidadDerActual += max(1, (int)((velocidadDerObjetivo - velocidadDerActual) * tasaDer));
    if (velocidadDerActual > velocidadDerObjetivo) velocidadDerActual = velocidadDerObjetivo;
  } else if (velocidadDerActual > velocidadDerObjetivo) {
    velocidadDerActual -= max(1, (int)((velocidadDerActual - velocidadDerObjetivo) * tasaDer));
    if (velocidadDerActual < velocidadDerObjetivo) velocidadDerActual = velocidadDerObjetivo;
  }
  
  // Aplicar velocidades a los motores
  establecerDireccionMotor(true, velocidadIzqActual);
  establecerDireccionMotor(false, velocidadDerActual);
  
  ledcWrite(canalPWM_Izq, abs(velocidadIzqActual));
  ledcWrite(canalPWM_Der, abs(velocidadDerActual));
}

// ===== PROCESAR COMANDOS DEL MANDO =====
void procesarMando(ControllerPtr ctl) {
  // Obtener valores del joystick
  int joystickX = ctl->axisX();  // -511 a 512 (izquierda/derecha)
  int joystickY = ctl->axisY();  // -511 a 512 (arriba/abajo)
  
  // Aplicar filtro de suavizado exponencial (solo cuando hay movimiento)
  if (abs(joystickX) > zonaMuerta || abs(joystickY) > zonaMuerta) {
    joystickXFiltrado = joystickXFiltrado * (1 - filtroSuavizado) + joystickX * filtroSuavizado;
    joystickYFiltrado = joystickYFiltrado * (1 - filtroSuavizado) + joystickY * filtroSuavizado;
  } else {
    // Resetear filtros cuando el joystick está en reposo
    joystickXFiltrado = 0;
    joystickYFiltrado = 0;
  }
  
  // Normalizar valores del joystick a rango -1.0 a 1.0
  float ejeY = -joystickYFiltrado / 511.0;  // Negativo para que arriba sea positivo
  float ejeX = joystickXFiltrado / 511.0;
  
  // Aplicar zona muerta
  if (abs(joystickXFiltrado) < zonaMuerta) ejeX = 0;
  if (abs(joystickYFiltrado) < zonaMuerta) ejeY = 0;
  
  // Si no hay movimiento, detener gradualmente
  if (ejeX == 0 && ejeY == 0) {
    velocidadIzqObjetivo = 0;
    velocidadDerObjetivo = 0;
    return;
  }
  
  // ===== CONTROL TIPO TANQUE/ORUGA =====
  // Calcular velocidades base para cada motor
  float velocidadBase = ejeY * velocidadMaxima;
  
  // Aplicar giro diferencial
  float velocidadIzq, velocidadDer;
  
  if (ejeX < 0) {
    // Giro a la izquierda: reducir velocidad del motor izquierdo
    velocidadIzq = velocidadBase * (1.0 + ejeX);
    velocidadDer = velocidadBase;
  } else if (ejeX > 0) {
    // Giro a la derecha: reducir velocidad del motor derecho
    velocidadIzq = velocidadBase;
    velocidadDer = velocidadBase * (1.0 - ejeX);
  } else {
    // Sin giro: ambos motores igual
    velocidadIzq = velocidadBase;
    velocidadDer = velocidadBase;
  }
  
  // Si solo hay giro sin avance/retroceso (giro en su lugar)
  if (abs(ejeY) < 0.1 && abs(ejeX) > 0.1) {
    float velocidadGiro = abs(ejeX) * velocidadMaxima * factorCurva;
    if (ejeX < 0) {
      // Girar izquierda: izq atrás, der adelante
      velocidadIzq = -velocidadGiro;
      velocidadDer = velocidadGiro;
    } else {
      // Girar derecha: izq adelante, der atrás
      velocidadIzq = velocidadGiro;
      velocidadDer = -velocidadGiro;
    }
  }
  
  // Aplicar velocidad mínima si hay movimiento
  if (velocidadIzq != 0) {
    float signo = velocidadIzq > 0 ? 1 : -1;
    velocidadIzq = signo * max((float)velocidadMinima, abs(velocidadIzq));
  }
  if (velocidadDer != 0) {
    float signo = velocidadDer > 0 ? 1 : -1;
    velocidadDer = signo * max((float)velocidadMinima, abs(velocidadDer));
  }
  
  // Limitar velocidades al rango permitido
  velocidadIzq = constrain(velocidadIzq, -velocidadMaxima, velocidadMaxima);
  velocidadDer = constrain(velocidadDer, -velocidadMaxima, velocidadMaxima);
  
  // Establecer velocidades objetivo
  velocidadIzqObjetivo = (int)velocidadIzq;
  velocidadDerObjetivo = (int)velocidadDer;
  
  // Debug (comentar si no se necesita)
  static unsigned long ultimoDebug = 0;
  if (millis() - ultimoDebug > 200) {
    Serial.printf("Joy(%.2f,%.2f) → Vel Izq:%d→%d Der:%d→%d\n", 
                  ejeX, ejeY, 
                  velocidadIzqActual, velocidadIzqObjetivo,
                  velocidadDerActual, velocidadDerObjetivo);
    ultimoDebug = millis();
  }
}

// ===== PROCESAR TODOS LOS MANDOS =====
void procesarMandos() {
  for (auto mando : miMando) {
    if (mando && mando->isConnected() && mando->hasData()) {
      if (mando->isGamepad()) {
        procesarMando(mando);
      }
    }
  }
}

// ===== CONFIGURACIÓN INICIAL =====
void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware Bluepad32: %s\n", BP32.firmwareVersion());
  
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("Dirección BT: %2X:%2X:%2X:%2X:%2X:%2X\n", 
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  
  // Configurar pines
  pinMode(motorIzqIN1, OUTPUT);
  pinMode(motorIzqIN2, OUTPUT);
  pinMode(motorDerIN3, OUTPUT);
  pinMode(motorDerIN4, OUTPUT);
  
  // Configurar PWM
  ledcSetup(canalPWM_Izq, frecuenciaPWM, resolucion);
  ledcSetup(canalPWM_Der, frecuenciaPWM, resolucion);
  ledcAttachPin(motorIzqENA, canalPWM_Izq);
  ledcAttachPin(motorDerENB, canalPWM_Der);
  
  detenerMotoresInmediato();
  
  // Configurar Bluepad32
  BP32.setup(&mandoConectado, &mandoDesconectado);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
  
  Serial.println("\n=== SISTEMA INICIADO - CONTROL SUAVE ACTIVADO ===");
  Serial.println("Configuración:");
  Serial.printf("- Aceleración: %.2f | Frenado: %.2f\n", tasaAceleracion, tasaFrenado);
  Serial.printf("- Factor curva: %.2f\n", factorCurva);
  Serial.printf("- Zona muerta: %d\n", zonaMuerta);
  Serial.printf("- Vel. mín/máx: %d/%d\n", velocidadMinima, velocidadMaxima);
  Serial.println("Esperando conexión del mando...\n");
}

// ===== BUCLE PRINCIPAL =====
void loop() {
  bool datosActualizados = BP32.update();
  
  if (datosActualizados) {
    procesarMandos();
  }
  
  // Actualizar velocidades gradualmente en cada ciclo
  actualizarVelocidadesGradual();
  
  delay(10);  // 100Hz de actualización
}
