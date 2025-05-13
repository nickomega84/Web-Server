[ epoll() ]
     |
     v
[ evento recibido ]
     |
     v
[ fd activo ] --> [ Connection Handler ]
                      |
                      v
             [ aceptar conexión? ]
                |         |
               Sí        No
                |         |
    [ aceptar() + registrar en epoll ] 
                          |
                          v
                 [ Procesar conexión ]