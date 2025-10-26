#include <Bluepad32.h>

// Array para almacenar hasta 4 mandos conectados
ControllerPtr misMandos[BP32_MAX_GAMEPADS];

// ===== CALLBACK: Se ejecuta cuando se conecta un mando =====
void alConectarMando(ControllerPtr ctl) {
  bool espacioLibre = false;
  
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (misMandos[i] == nullptr) {
      Serial.printf("✅ MANDO CONECTADO en slot %d\n", i);
      
      // Obtener información del mando
      ControllerProperties propiedades = ctl->getProperties();
      Serial.printf("📱 Modelo: %s\n", ctl->getModelName().c_str());
      Serial.printf("🔖 VID: 0x%04x, PID: 0x%04x\n", propiedades.vendor_id, propiedades.product_id);
      Serial.println("========================================");
      
      misMandos[i] = ctl;
      espacioLibre = true;
      
      // Cambiar color de la barra de luz a verde (si el mando lo soporta)
      ctl->setColorLED(0, 255, 0);  // RGB: Verde
      
      break;
    }
  }
  
  if (!espacioLibre) {
    Serial.println("⚠️ Mando conectado pero no hay espacio disponible");
  }
}

// ===== CALLBACK: Se ejecuta cuando se desconecta un mando =====
void alDesconectarMando(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (misMandos[i] == ctl) {
      Serial.printf("❌ MANDO DESCONECTADO del slot %d\n", i);
      Serial.println("========================================");
      misMandos[i] = nullptr;
      break;
    }
  }
}

// ===== FUNCIÓN: Mostrar todos los valores del mando en el Monitor Serial =====
void mostrarValoresMando(ControllerPtr ctl) {
  Serial.printf(
    "idx=%d | D-Pad: 0x%02x | Botones: 0x%04x | "
    "JoyIzq X:%4d Y:%4d | JoyDer X:%4d Y:%4d | "
    "Freno:%4d | Acelerador:%4d | Misc: 0x%02x\n",
    ctl->index(),
    ctl->dpad(),
    ctl->buttons(),
    ctl->axisX(),
    ctl->axisY(),
    ctl->axisRX(),
    ctl->axisRY(),
    ctl->brake(),
    ctl->throttle(),
    ctl->miscButtons()
  );
}

// ===== FUNCIÓN: Procesar acciones específicas del mando =====
void procesarAccionesMando(ControllerPtr ctl) {
  
  // ===== LECTURA DE JOYSTICKS =====
  int joyIzqX = ctl->axisX();
  int joyIzqY = ctl->axisY();
  int joyDerX = ctl->axisRX();
  int joyDerY = ctl->axisRY();
  
  // Zona muerta para evitar lecturas espurias
  const int zonaMuerta = 50;
  
  // Mostrar valores de joysticks solo si se están moviendo
  if (abs(joyIzqX) > zonaMuerta || abs(joyIzqY) > zonaMuerta) {
    Serial.printf("🕹️  JOYSTICK IZQUIERDO -> X: %4d, Y: %4d\n", joyIzqX, joyIzqY);
  }
  
  if (abs(joyDerX) > zonaMuerta || abs(joyDerY) > zonaMuerta) {
    Serial.printf("🕹️  JOYSTICK DERECHO   -> X: %4d, Y: %4d\n", joyDerX, joyDerY);
  }
  
  // ===== LECTURA DE GATILLOS (L2/R2 o LT/RT) =====
  int gatilloL2 = ctl->brake();
  int gatilloR2 = ctl->throttle();
  
  if (gatilloL2 > 50) {
    Serial.printf("🎮 GATILLO L2 presionado: %d\n", gatilloL2);
  }
  
  if (gatilloR2 > 50) {
    Serial.printf("🎮 GATILLO R2 presionado: %d\n", gatilloR2);
  }
  
  // ===== LECTURA DE D-PAD (CRUCETA) =====
  uint8_t dpad = ctl->dpad();
  
  if (dpad & 0x01) {
    Serial.println("⬆️  D-PAD: ARRIBA");
  }
  if (dpad & 0x02) {
    Serial.println("⬇️  D-PAD: ABAJO");
  }
  if (dpad & 0x04) {
    Serial.println("⬅️  D-PAD: DERECHA");
  }
  if (dpad & 0x08) {
    Serial.println("➡️  D-PAD: IZQUIERDA");
  }
  
  // ===== LECTURA DE BOTONES PRINCIPALES =====
  uint16_t botones = ctl->buttons();
  
  if (botones & 0x0001) {
    Serial.println("🔴 BOTÓN X presionado");
    ctl->setColorLED(255, 0, 0);
  }
  
  if (botones & 0x0002) {
    Serial.println("🔵 BOTÓN CIRCLE/B presionado");
    ctl->setColorLED(0, 0, 255);
  }
  
  if (botones & 0x0004) {
    Serial.println("🟡 BOTÓN SQUARE/Y presionado");
    ctl->setColorLED(255, 255, 0);
  }
  
  if (botones & 0x0008) {
    Serial.println("🟢 BOTÓN TRIANGLE/A presionado");
    ctl->setColorLED(0, 255, 0);
  }
  
  if (botones & 0x0010) {
    Serial.println("🎮 BOTÓN L1 presionado");
  }
  
  if (botones & 0x0020) {
    Serial.println("🎮 BOTÓN R1 presionado");
  }
  
  if (botones & 0x0040) {
    Serial.println("🕹️  BOTÓN L3 (Joy Izq) presionado");
  }
  
  if (botones & 0x0080) {
    Serial.println("🕹️  BOTÓN R3 (Joy Der) presionado");
  }
  
  if (botones & 0x0100) {
    Serial.println("⏸️  BOTÓN START/OPTIONS presionado");
  }
  
  if (botones & 0x0200) {
    Serial.println("📤 BOTÓN SELECT/SHARE presionado");
  }
  
  if (botones & 0x1000) {
    Serial.println("🏠 BOTÓN HOME presionado");
    ctl->setRumble(128, 128);
  }
  
  // ===== LECTURA DE BOTONES MISCELÁNEOS =====
  uint8_t botonesExtra = ctl->miscButtons();
  
  if (botonesExtra != 0) {
    Serial.printf("🔘 BOTONES EXTRA: 0x%02x\n", botonesExtra);
  }
  
  // Separador visual
  if (botones != 0 || dpad != 0 || abs(joyIzqX) > zonaMuerta || 
      abs(joyIzqY) > zonaMuerta || abs(joyDerX) > zonaMuerta || 
      abs(joyDerY) > zonaMuerta || gatilloL2 > 50 || gatilloR2 > 50) {
    Serial.println("----------------------------------------");
  }
}

// ===== FUNCIÓN: Procesar todos los mandos conectados =====
void procesarMandos() {
  for (auto mando : misMandos) {
    if (mando && mando->isConnected() && mando->hasData()) {
      if (mando->isGamepad()) {
        // Descomentar para ver valores raw
        // mostrarValoresMando(mando);
        
        // Procesar acciones específicas
        procesarAccionesMando(mando);
      } else {
        Serial.println("⚠️ Dispositivo conectado pero no es un gamepad");
      }
    }
  }
}

// ===== CONFIGURACIÓN INICIAL =====
void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n========================================");
  Serial.println("🎮 TEST DE MANDO BLUETOOTH CON ESP32");
  Serial.println("========================================");
  
  Serial.printf("📦 Firmware Bluepad32: %s\n", BP32.firmwareVersion());
  
  const uint8_t* direccionBT = BP32.localBdAddress();
  Serial.printf("📡 Dirección Bluetooth: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                direccionBT[0], direccionBT[1], direccionBT[2], 
                direccionBT[3], direccionBT[4], direccionBT[5]);
  
  Serial.println("========================================");
  
  BP32.setup(&alConectarMando, &alDesconectarMando);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
  
  Serial.println("\n⏳ Esperando conexión del mando...");
  Serial.println("💡 Pon tu mando en modo emparejamiento:");
  Serial.println("   - VSG Tarvos: Mantén presionado el botón de encendido");
  Serial.println("   - PS4: Presiona PS + Share");
  Serial.println("   - Xbox: Presiona el botón de emparejamiento");
  Serial.println("   - Switch Pro: Presiona el botón SYNC");
  Serial.println("========================================\n");
}

// ===== BUCLE PRINCIPAL =====
void loop() {
  bool datosActualizados = BP32.update();
  
  if (datosActualizados) {
    procesarMandos();
  }
  
  delay(10);
}
