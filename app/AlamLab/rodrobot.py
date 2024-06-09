import serial
import sys
import argparse
import time

EOC = "!EOC"
EOL = "\r\n"

parser = argparse.ArgumentParser()
parser.add_argument("-f", "--file", help="open G-CODE file")
parser.add_argument("-p", "--port", help="set COM port")
args = parser.parse_args()

if args.port:
    port = args.port
else:
    port = "COM3"

try:
    mcu = serial.Serial(port=port, baudrate=250000, timeout=.1)
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
        print("\nRodRobot Command Sender\n")
        try:
            while True:
                cmd = input("G-CODE> ").upper()
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
