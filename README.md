# Omnicar32
# 🎮 Carro RC con Control Bluetooth - Segunda Vida Electrónica

## 📋 Descripción del Proyecto

Este proyecto consiste en **reutilizar un carro a control remoto antiguo** que estaba destinado a la basura, dándole una segunda vida mediante la integración de un **ESP32** y un **mando Bluetooth VSG Tarvos**. 

En lugar de desechar el juguete completo, aprovechamos toda la electrónica funcional (motores, chasis, ruedas y baterías) y le damos **nuevas capacidades de control inalámbrico moderno**.

## 🌱 Impacto Ambiental

### ¿Por qué es importante este proyecto?

- **♻️ Reduce la basura electrónica**: Los juguetes RC antiguos contienen componentes perfectamente funcionales que terminan en vertederos.
- **🌍 Contribuye al medio ambiente**: Reutilizar electrónica existente reduce la demanda de fabricación de nuevos componentes.
- **💡 Promueve la economía circular**: Demostramos que la tecnología "obsoleta" puede tener nuevos usos.
- **🔋 Aprovecha recursos**: Motores, baterías y estructura mecánica tienen años de vida útil restante.

## 🛠️ Componentes Necesarios

### Componentes reutilizados del carro antiguo:
- Chasis y estructura del carro RC
- 2 motores DC (funcionando)
- Ruedas y sistema de transmisión
- Batería recargable (si está en buen estado)

### Componentes nuevos (inversión mínima):
- 1x ESP32 DevKit (~$5 USD)
- 1x Mando Bluetooth VSG Tarvos (~$15 USD)
- 1x Driver de motores L298N (~$2 USD)
- Cables jumper y conectores básicos (~$2 USD)

**💰 Costo total aproximado: $24 USD** (vs. $50-80 USD de un carro RC nuevo)

## 🚀 Características del Proyecto

- ✅ Control inalámbrico mediante Bluetooth
- ✅ Control de velocidad variable usando joysticks analógicos
- ✅ Giros suaves y precisos
- ✅ Rango de hasta 10 metros
- ✅ Compatible con múltiples tipos de mandos (PS4, Xbox, Switch, Android)
- ✅ Código abierto y fácil de modificar

## 📦 Instalación

### Paso 1: Preparar Arduino IDE

1. Instala Arduino IDE (versión 1.8.19 o superior)
2. Agrega las URLs de gestores de placas en **Archivo → Preferencias**:

# Librerias 
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json


3. Instala las placas ESP32 y Bluepad32 desde **Herramientas → Gestor de tarjetas**

### Paso 2: Conexiones del Hardware

**ESP32 → L298N:**
- GPIO 27 → IN1 (Motor Izquierdo Dirección)
- GPIO 26 → IN2 (Motor Izquierdo Dirección)
- GPIO 14 → ENA (Motor Izquierdo Velocidad)
- GPIO 25 → IN3 (Motor Derecho Dirección)
- GPIO 33 → IN4 (Motor Derecho Dirección)
- GPIO 32 → ENB (Motor Derecho Velocidad)
- GND → GND

**L298N → Motores del carro antiguo:**
- OUT1 y OUT2 → Motor izquierdo
- OUT3 y OUT4 → Motor derecho

**Alimentación:**
- Conecta la batería del carro antiguo al L298N (6-12V)

### Paso 3: Cargar el código

1. Abre el archivo `BluePadV1.ino` en Arduino IDE
2. Selecciona la placa correcta: **Herramientas → Placa → esp32_bluepad32**
3. Selecciona tu puerto COM
4. Presiona **Subir**

### Paso 4: Emparejar el mando

1. Abre el **Monitor Serial** (115200 baudios)
2. Presiona el botón **EN** del ESP32
3. Pon el mando VSG Tarvos en **modo emparejamiento**
4. Espera el mensaje "✅ MANDO CONECTADO"
5. ¡Listo para conducir! 🎉

## 🎮 Controles

- **Joystick Izquierdo Vertical**: Avanzar / Retroceder
- **Joystick Izquierdo Horizontal**: Girar en su lugar
- **Joystick con ángulos**: Giros mientras avanza

## 📁 Archivos del Proyecto

├── BluePadV1.ino # Código principal de control
├── BluePadTest.ino # Código de prueba del mando
└── README.md # Este archivo


## 🌟 Beneficios de Reutilizar

| Aspecto | Carro Nuevo | Carro Reutilizado |
|---------|-------------|-------------------|
| **Costo** | $50-80 USD | $24 USD |
| **Impacto ambiental** | Alta huella de carbono | Huella reducida |
| **Basura electrónica** | Genera nueva | Reduce existente |
| **Aprendizaje** | Uso básico | Programación + Electrónica |
| **Personalización** | Limitada | Total |

## 📚 Aprende Más

Este proyecto es ideal para:
- 🎓 Aprender programación con Arduino
- 🔧 Entender electrónica básica
- 🌍 Practicar sostenibilidad tecnológica
- 💡 Desarrollar habilidades de reutilización creativa

## 🤝 Contribuciones

¡Este proyecto es de código abierto! Si mejoras el código o agregas nuevas funcionalidades, comparte tus cambios.

## 📝 Licencia

Este proyecto es de uso libre para fines educativos y personales.

---

**💚 Recuerda**: Cada dispositivo electrónico que reutilizamos es un paso hacia un futuro más sostenible. ¡Dale una segunda oportunidad a la tecnología "vieja"!
