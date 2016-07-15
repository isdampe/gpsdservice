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

Before doing the below, install the dependencies.

1. git clone to /root/gps
2. Compile to build/gps
3. Install systemd/gpstracker.service on boot
4. Add "remote" hostname entry to /etc/hosts
5. Manually test
6. Make read only

