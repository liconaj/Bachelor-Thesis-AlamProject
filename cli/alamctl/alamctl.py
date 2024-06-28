#!/usr/bin/python3

import serial
import sys
import argparse
import time
import serial.tools.list_ports

EOC = "!EOC"
EOL = "\r\n"

def get_ports():
    ports = []
    patterns = ["COM", "ttyACM", "ttyUSB"]
    for pattern in patterns:
        ports += serial.tools.list_ports.grep(pattern)
    return ports

def list_ports():
    ports = get_ports()
    for port in ports:
        print("{0:<20} \t {1:<30}".format(port.device, port.manufacturer))

parser = argparse.ArgumentParser()
parser.add_argument("-p", "--port", help="set device port")
parser.add_argument("-f", "--file", help="open G-CODE file")
parser.add_argument("-l", "--list", help="list available devices", action="store_true")
args = parser.parse_args()

if args.list:
    list_ports()
    sys.exit()

if args.port:
    port = args.port
else:
    ports = get_ports()
    print(f"{len(ports)} device(s) found")
    if not ports:
        print("No serial ports available")
        sys.exit()
    else:
        port = ports[0].device
    print(f"Connecting to {port}...")

try:
    mcu = serial.Serial(port=port, baudrate=250000, timeout=.1)
    time.sleep(1)
    reponse = mcu.readline().decode()
    if not reponse.startswith("this is alam"):
        print("Incompatible robot")
        sys.exit()
except serial.SerialException:
    print("Serial port unavailable")
    sys.exit()

def send_command(value: str):
    mcu.write(bytes(value, "utf-8"))

def parse_command(cmd: str) -> str:
    cmd = cmd.strip().split(";")[0]
    cmd += EOL
    # print(f"Parsing command {repr(cmd)}")
    send_command(cmd)
    output = ""
    value = ""
    while not EOC in value:
        value = mcu.readline().decode()
        if not EOC in value:
            output += value
    return output

if __name__ == "__main__":
    if args.file is None:
        print("\nAlam Control Center")
        print("By Josue Licona (C) 2024\n")

        print("G to show G-CODE commands")
        print("M to show M-CODE commands")
        print("Ctrl+C or ;EXIT to quit\n")

        try:
            while True:
                cmd = input("alam> ").upper()
                if ";EXIT" in cmd:
                    break
                elif not cmd or cmd.startswith(";"):
                    continue
                print(parse_command(cmd))
        except KeyboardInterrupt:
            print("\n\nPROGRAM ENDED BY USER\n")

    else:
        time.sleep(1)
        try:
            with open(args.file, "r", encoding="utf-8") as fh:
                lines = fh.readlines()
                fh.close()
        except FileNotFoundError:
            print(f"File `{args.file}` not found")
            sys.exit()

        for cmd in lines:
            cmd = cmd.strip().upper()
            if not cmd or cmd.startswith(";"):
                continue
            print(f"> {cmd.strip()}")
            print(parse_command(cmd))