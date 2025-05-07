### 🗂 Componentes Principales:

```
[ epoll() ]
    |
    v
[ fd activo ] --> [ Connection Handler ]
                      |
                      v
              [ Parser HTTP ]
                      |
                      v
              [ Middleware Stack ]
                  |       |
          [Session]   [Cookie]
                  |
                  v
               [ Router ]
             /     |     \
         CGI   Static   WebSocket
             \     |     /
              [ Response ]
                  |
               [ write() ]
```