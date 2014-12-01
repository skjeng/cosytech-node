# UART Interface
# Set up UART commiunication from Arduino to PC.
# TODO: Choose lib for displaying images and sound management
# Libraries: pyserial and PIL (tkinter) 

import serial
# from PIL import Image

def uart_init():
  # UART config. Locate USB port in /dev dir and replace "ttyAMA0" with correct port
  return serial.Serial("/dev/ttyAMA0", baudrate = 115200) # Port, timeout, baudrate, parity bit etc

def uart_rcv_line(conn):
  return conn.readline() # read cmd from Arduino. Must be \n terminated

def uart_send(conn, msg):
  conn.write(bytes(msg))

def play_sound():
  # open sound file with linux audio
  print "playing sound"

# def show_img():
#   img = Image.open("vga_driver.PNG")
#   img.show()

def slide_show():

  # Set up UART connection
  conn = uart_init()

  # Dummy message for testing
  uart_send(conn, "pic1\n")

  # Endless loop
  while(1):

    # Wait for command from Arduino
    cmd = uart_rcv_line(conn)

    # Display different images based on cmd form Arduino
    if "pic1" in cmd:
      # Display picture with Tkinter
      print "pic1"
    if "pic2" in cmd:
      # Display picture with Tkinter
      print "pic2"
    if "pic3" in cmd:
      # Display picture with Tkinter
      print "pic3"
    if "sound1" in cmd:
      # Playing sound1
      print "sound1"

# Run slide show
slide_show()
