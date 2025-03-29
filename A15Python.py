import serial
import threading
import time

# Configuración del puerto serial (ajusta 'COM3' a tu puerto)
arduino = serial.Serial('COM3', 115200, timeout=0.1)

# Bandera para controlar la ejecución
running = True

def leer_respuestas():
    while running:
        try:
            while arduino.in_waiting > 0:
                linea = arduino.readline().decode().strip()
                if linea:
                    print(f"Arduino: {linea}")
        except:
            pass
        time.sleep(0.01)

def enviar_comandos():
    global running
    print("Sistema listo. Comandos disponibles:")
    print(" ltr - Animación izquierda-derecha\n rtl - Animación derecha-izquierda\n off - Apagar LEDs\n exit - Salir")
    
    while running:
        comando = input("Ingrese comando: ").strip().lower()
        
        if comando == "exit":
            running = False
            break
            
        if comando in ["ltr", "rtl", "off"]:
            arduino.write(f"{comando}\n".encode())
        else:
            print("Comando inválido. Enviando señal de error...")
            arduino.write(b"comando_invalido\n")
        
        time.sleep(0.1)  # Pequeña pausa para evitar sobrecarga

# Configurar y ejecutar hilos
lector = threading.Thread(target=leer_respuestas)
lector.daemon = True
lector.start()

enviar_comandos()

# Limpieza final
arduino.close()
print("Programa terminado.")