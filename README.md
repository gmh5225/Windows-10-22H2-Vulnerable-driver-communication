# Windows-10-22H2-Vulnerable-driver-communication.
Allocate/free memory in the kernel & r/w control registers with a vulnerable driver.

This driver is not blacklisted, & is able to run on both windows 11 and 10. You could use it to map your driver by messing around with r/w control register on Win10, but mitigations introduced/ enforced on Windows 11 like hyperguard will bugcheck once you do.

This driver's IOCTL communication is easy to reverse, so I've included it in this repo if you wanna mess around more. It exposes other stuff you could use but r/w control register & allocate memory are the more useful ones imo![time](https://user-images.githubusercontent.com/120271650/236528738-66239da6-44cc-4ae5-a32c-d53bd6582cd6.png)
