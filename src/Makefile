MCU=atmega32
DUDECPUTYPE=m32
F_CPU=12000000
OBJS=main.o HTTPServer.o ServicesServer.o DataServer.o AuthServer.o IRServer.o generator.o tcpip/enc28j60.o tcpip/ip_arp_udp_tcp.o 
OUT=lanpilot

CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size

OPT=-mmcu=$(MCU) -O2 -DF_CPU=$(F_CPU)
DUDEOPT=-p $(DUDECPUTYPE) -c usbasp -y 

$(OBJS): %.o: %.c
	$(CC) $(OPT) -c $< -o $@


$(OUT): $(OBJS)
	$(CC) $(OPT) $(OBJS) -o $(OUT)
	$(SIZE) $(OUT)

$(OUT).hex: $(OUT)
	$(OBJCOPY) -R .eeprom -O ihex $< $@

all: $(OUT)

up: clean upload

upload: $(OUT).hex
	avrdude $(DUDEOPT) -e -U flash:w:$<


rebuild: clean all

clean:
	rm -f $(OUT)
	rm -f *.hex
	rm -f *.o
	rm -f *~
	rm -f tcpip/*.o
	rm -f tcpip/*~
