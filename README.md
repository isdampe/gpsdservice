# Compiling

```bash
gcc -o ../build/gps gps.c -lm -lgps -lcurl
```
## Requirements

* libgps-dev
* curl curl-dev
* gpsd
* libcurl4-openssl-dev

### Configuring
* Read only / mount
* /etc/hosts add "remote" IP
