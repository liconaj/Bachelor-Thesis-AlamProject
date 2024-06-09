## Documentación para Códigos G y M para RodRobot

### Comandos G

| Código  | Descripción                                                                                                                                      | Ejemplo                           | Implementado |
| ------- | ------------------------------------------------------------------------------------------------------------------------------------------------ | --------------------------------- | ------------ |
| `G4`    | Pausa o dwell. Detiene la máquina por el tiempo especificado.                                                                                    | `G4 P1000`                        | Sí           |
| `G90`   | Establece el modo de posicionamiento absoluto. Todos los movimientos siguientes son interpretados como posiciones absolutas.                     | `G90` | Sí           |
| `G91`   | Establece el modo de posicionamiento relativo. Todos los movimientos siguientes son interpretados como desplazamientos desde la posición actual. | `G91`                             | Sí           |
| `G1`    | Mover uno o más actuadores a las posiciones especificadas a una velocidad determinada.                                                           | `G1 A100 B150 C200 F300`          | Sí           |
| `G92`   | Establecer posición de origen                                                                                                                    | `G92 A0 B0 C0`                    | Sí           |
| `G92.1` | Establecer posición predeterminada 2                                                                                                             | `G92.1 A0 B0 C0`                  | Sí           |
| `G28`   | Mover actuadores a posición de origen                                                                                                            | `G28`                             | Sí           |
| `G28.1` | Mover actuadores a posición predeterminada 2                                                                                                     | `G28.1`                           | Sí           |
| `G20`   | Establece las unidades de medida en pulgadas para todos los movimientos subsiguientes.                                                           | `G20`                             | Sí           |
| `G21`   | Establece las unidades de medida en milímetros para todos los movimientos subsiguientes.                                                         | `G21`                             | Sí           |

### Códigos M

| Código | Descripción                                                                                           | Ejemplo                 | Implementado |
| ------ | ----------------------------------------------------------------------------------------------------- | ----------------------- | ------------ |
| `M00`  | Detener                                                                                               | `M00`                   | Sí           |
| `M30`  | Fin del programa. También resetea el programa para iniciar desde el principio si se ejecuta de nuevo. | `M30`                   |              |
| `M85`  | Tiempo máximo de espera de comando (valor predeterminado, 5 segundos)                                 | `M85 P5000`             | Sí           |
| `M114` | Reportar posición actual de los actuadores                                                            | `M114`                  | Sí           |
| `M301` | Establece los parámetros PID.                                                                         | `M301 P1.0 I0.05 D0.01` | Sí           |
| `M303` | Inicia un proceso de autotuning para los parámetros del PID.                                          | `M303`                  | Sí           |
| `M355` | Reportar estado actual parámetros del robot                                                           | `M400`                  |              |
| `M500` | Guardar parámetros en memoria EEPROM                                                                  | `M500`                  |              |
| `M501` | Leer parámetros de memoria EEPROM                                                                     | `M501`                  |              |
| `M502` | Restablecer parámetros por defecto guardados en memoria EEPROM                                        | `M502`                  |              |
| `M503` | Mostrar configuración guardada en la memoria EEPROM                                                   | `M503`                  |              |

### Parámetros

| Letra                                      | Descripción                                                                                    | Ejemplo                 |
| ------------------------------------------ | ---------------------------------------------------------------------------------------------- | ----------------------- |
| `F`                                        | Velocidad de avance en unidades por minuto. Aplicable a movimientos lineales de actuadores.    | `G1 A100 F200`          |
| `A`, `B`, `C`, `I`,`J`, `K`, `U`, `V`, `W` | Identificadores de los actuadores en el sistema. Cada uno corresponde a un actuador diferente. | `G1 U10 V20 W30`        |
| `P`                                        | Puede ser usado para definir un parámetro o valor de pausa en milisegundos.                    | `G4 P500`               |
| `P`, `I`, `D`                              | Parámetros para definir constantes del PID                                                     | `M301 P1.0 I0.05 D0.01` |

### Ejemplos Comunes

- **Mover Actuadores a Posición Absoluta:**
  
  ```gcode
  G90       ; Establecer modo absoluto
  G1 A50 B75 C100 F150 ; Mueve los actuadores A, B y C a las posiciones 50, 75 y 100 respectivamente a una velocidad de 150.
  ```

- **Establecer Unidades y Mover en Relativo:**
  
  ```gcode
  G21       ; Establecer unidades en milímetros
  G91       ; Establecer modo relativo
  G1 I20 J15 K10 F120 ; Mueve los actuadores I, J y K en 20mm, 15mm y 10mm respectivamente a una velocidad de 120.
  ```

- **Retorno a Origen y Reset de Posición:**
  
  ```gcode
  G28 U V W ; Mueve los actuadores U, V y W a sus posiciones de origen
  G92 U0 V0 W0 ; Establece la posición actual de U, V y W como nueva posición de origen
  ```
