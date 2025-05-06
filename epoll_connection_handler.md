# 🔌 Componente: epoll() + Connection Handler

Este componente representa el **punto de entrada** del servidor web. Su propósito es gestionar múltiples conexiones entrantes de forma eficiente utilizando `epoll()`.

---

## 📊 Diagrama de Flujo

![epoll flow](A_flowchart_diagram_in_the_image_illustrates_the_c.png)

---

## 🔁 Descripción del Flujo

1. **epoll()**  
   El bucle principal del servidor llama a `epoll_wait()` para esperar eventos en múltiples file descriptors.

2. **WAIT FOR EVENTS**  
   El sistema bloquea hasta que uno o más eventos (lectura/escritura) estén listos.

3. **NEW CONNECTION?**  
   Se detecta si el evento es una nueva conexión entrante en un socket de escucha.

   - ✅ **Sí** → Se acepta la conexión (`accept()`) y se añade el nuevo fd al conjunto de epoll.
   - ❌ **No** → Se trata de un fd activo ya existente.

4. **HANDLE ACTIVE CONNECTION**  
   Se lee o escribe en el socket correspondiente al cliente activo, delegando el trabajo al resto de componentes (`Parser HTTP`, `Middleware`, etc.).

---

## ✅ Ventajas de epoll()

- Escalable a miles de conexiones.
- Bajo consumo de CPU.
- Soporte nativo en Linux.