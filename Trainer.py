# MIT License

# Copyright (c) 2025 Adithya5434 (https://github.com/Adithya5434)

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import tkinter as tk
from tkinter import messagebox
from pymem import Pymem
from pymem.process import module_from_name
import os
import sys

# check if game is running 
try:
    mem = Pymem("HillClimbRacing.exe")
except Exception as e:
    print(f"Game not running: {e}")
    messagebox.showerror("Error", "Hill Climb Racing is not running.")
    sys.exit()


module = module_from_name(mem.process_handle, "HillClimbRacing.exe").lpBaseOfDll

money_address = module + 0x28CAD4
diamonds_address = module + 0x28CAEC


def set_money():
    try:
        money = int(money_entry.get())
        mem.write_int(money_address, money)
    except Exception as e:
        print(f"error writing money: {e}")

def set_diamonds():
    try:
        diamonds = int(diamonds_entry.get())
        mem.write_int(diamonds_address, diamonds)
    except Exception as e:
        print(f"error writing diamonds: {e}")


# GUI 
root = tk.Tk()
root.title("HCR Trainer")
root.resizable(False, False)
iconfile = os.path.join(os.path.dirname(__file__), 'hcr.ico')
root.iconbitmap(iconfile)

# money
money_label = tk.Label(root, text="Money:", font=("Arial", 10))
money_label.grid(row=0, column=0, padx=10, pady=10, sticky="w")

money_entry = tk.Entry(root, width=15)
money_entry.grid(row=0, column=1, padx=5, pady=5, sticky="w")

money_btn = tk.Button(root, text=" Set ", font=("Arial", 9, "bold"), command=set_money)
money_btn.grid(row=0, column=2, padx=5, pady=5, sticky="w")


# diamonds
diamonds_label = tk.Label(root, text="Diamonds:", font=("Arial", 10))
diamonds_label.grid(row=1, column=0, padx=10, pady=5, sticky="w")

diamonds_entry = tk.Entry(root, width=15)
diamonds_entry.grid(row=1, column=1, padx=5, pady=5, sticky="w")

diamonds_btn = tk.Button(root, text=" Set ", font=("Arial", 9, "bold"), command=set_diamonds)
diamonds_btn.grid(row=1, column=2, padx=5, pady=5, sticky="w")


# Note label
note_label = tk.Label(root, text="Note: value only updates on main menu.")
note_label.grid(row=2, column=0, columnspan=3, padx=10, pady=1, sticky="w")

# Madeby label
madeby_label = tk.Label(root, text="Made by: github.com/Adithya5434",)
madeby_label.grid(row=3, column=0, columnspan=3, padx=10, pady=1, sticky="w")

root.mainloop()
