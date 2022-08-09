import serial
from tkinter import * 
import threading

com1 = serial.Serial("COM2", 9600, bytesize=8, stopbits=serial.STOPBITS_ONE)

root = Tk()
root.geometry("400x200")
root.title("Fan Speed Control App")
root.configure(bg="#203000")

label = Label(root, text = "Fan Speed Control App", width = 31, height=3, bg="#407020", fg="black", font = ("Verdana", 15)).place(x=0, y=0)
label_speed = Label(root, text = "Speed", width = 15, height=2, bg="#80b060", fg="black", font = ("Verdana", 15)).place(x=0, y=82)
label_temp = Label(root, text = "Temperature", width = 15, height=2, bg="#80b060", fg="black", font = ("Verdana", 15)).place(x=0, y=140)

text_speed = Text(root, width = 20, height = 3, bg="#a0d080")
text_speed.place(x=210, y=84)
text_speed.tag_configure("font", font = ("Verdana", 15, "bold"), justify = "center")

text_temp = Text(root, width = 20, height = 3, bg="#a0d080")
text_temp.place(x=210, y=142)
text_temp.tag_configure("font", font = ("Verdana", 15, "bold"), justify = "center")

def citanje_com_porta():
    
    while True:
        
        poruka = com1.readline()
        poruka = str(poruka)

        if poruka.startswith("b'Speed = "):
            
            poruka = poruka[10:13:1]
            text_speed.delete(1.0, END) 
            text_speed.insert(INSERT, poruka + "%", "font") 

        elif poruka.startswith("b'Temperature = "):

            poruka = poruka[16:22:1]
            text_temp.delete(1.0, END) 
            text_temp.insert(INSERT, poruka, "font")
 
thread_citanje = threading.Thread(target = citanje_com_porta)
thread_citanje.daemon = True
thread_citanje.start()

root.mainloop()
