#!/usr/bin/env python3
"""Flower Shop Ordering System - Python/tkinter port of orderSystem.cpp.

Uses the same data layout as the C++ version, so existing data keeps working:
    users&passwords.txt                    username|password
    products.txt                           Product Name|001
    Product Informations/001.png           product image (default.png = fallback)
    Product Informations/Product Details/001.txt   Price / Flower Type / Description
    User Informations/<user>.txt           account details
    User Logs/<user>.txt                   login log
    User Transaction History/<user>.txt    order/deposit history
    Receipts/<user>/<receipt id>.txt       saved receipts
    AboutUs.txt, user.png                  About Us text, profile picture

Optional: `pip install pillow` for better image scaling and the pink default tiles.
"""

import os
import re
import time
import tkinter as tk
from pathlib import Path
from tkinter import messagebox, ttk

try:
    from PIL import Image, ImageDraw, ImageTk
    HAS_PIL = True
except ImportError:
    HAS_PIL = False

BG = "#FFC0CB"
FG = "#501E32"
FONT = "Segoe UI"

USERS_FILE = "users&passwords.txt"
PRODUCTS_FILE = "products.txt"
ABOUT_FILE = "AboutUs.txt"
PROFILE_IMAGE = "user.png"
INFO_DIR = Path("User Informations")
LOGS_DIR = Path("User Logs")
HISTORY_DIR = Path("User Transaction History")
RECEIPTS_DIR = Path("Receipts")
PRODUCT_DIR = Path("Product Informations")
DETAILS_DIR = PRODUCT_DIR / "Product Details"

MAX_DEPOSIT = 1_000_000
USERNAME_RE = re.compile(r"[A-Za-z0-9_.\- ]+")


# --------------------------------------------------------------------------
# Helpers
# --------------------------------------------------------------------------

def normalize_id(product_id):
    digits = "".join(c for c in product_id if c.isdigit())
    return digits or "001"


def money(amount):
    return f"P{amount:,}"


def timestamp():
    return time.ctime()


def read_lines(path):
    try:
        with open(path, encoding="utf-8-sig", errors="replace") as f:
            return [line.rstrip("\r\n") for line in f]
    except OSError:
        return []


def image_path(product_id):
    for candidate in (PRODUCT_DIR / f"{product_id}.png", PRODUCT_DIR / "default.png"):
        if candidate.exists():
            return candidate
    return None


def load_photo(path, w, h):
    """Load an image scaled to fit w x h (centered on white). None on failure."""
    if path is None or not Path(path).exists():
        return None
    try:
        if HAS_PIL:
            img = Image.open(path).convert("RGBA")
            img.thumbnail((w, h), Image.LANCZOS)
            canvas = Image.new("RGB", (w, h), "white")
            canvas.paste(img, ((w - img.width) // 2, (h - img.height) // 2), img)
            return ImageTk.PhotoImage(canvas)
        img = tk.PhotoImage(file=str(path))
        factor = max(1, -(-img.width() // w), -(-img.height() // h))
        return img.subsample(factor) if factor > 1 else img
    except Exception:
        return None


def default_tile(name, w, h):
    """Pink placeholder tile with the product name (needs Pillow)."""
    if not HAS_PIL:
        return None
    img = Image.new("RGB", (w, h), "#A8556E")
    draw = ImageDraw.Draw(img)
    draw.rectangle([2, 2, w - 3, h - 3], fill="#FFE0E6")
    draw.rectangle([5, 5, w - 6, h - 6], fill="#FFC0CB")
    draw.text((16, h // 2 - 8), name[:18], fill="#64283C")
    return ImageTk.PhotoImage(img)


# --------------------------------------------------------------------------
# Data layer (no UI code in here)
# --------------------------------------------------------------------------

class Store:
    def __init__(self):
        for folder in (INFO_DIR, LOGS_DIR, HISTORY_DIR, RECEIPTS_DIR):
            folder.mkdir(parents=True, exist_ok=True)
        Path(USERS_FILE).touch(exist_ok=True)

        self.users = {}       # username -> password
        self.accounts = {}    # username -> {"password", "balance", "purchases"}
        self.products = []    # [(name, id)]
        self.details = {}     # id -> {"name", "price", "flower_type", "description"}
        self.cart = []        # [{"id", "name", "price", "qty"}]
        self.current = ""

        self.load_users()
        self.load_products()
        self.load_product_details()

    # ---- loading ---------------------------------------------------------

    def load_users(self):
        self.users.clear()
        for line in read_lines(USERS_FILE):
            if "|" in line:
                username, password = line.split("|", 1)
                if username:
                    self.users[username] = password

    def load_products(self):
        self.products.clear()
        for line in read_lines(PRODUCTS_FILE):
            if not line.strip():
                continue
            name, _, pid = line.partition("|")
            if name:
                self.products.append((name, normalize_id(pid) if pid else "001"))

    def load_product_details(self):
        self.details.clear()
        for name, pid in self.products:
            info = {"name": name, "price": 0,
                    "flower_type": "Not available.",
                    "description": "No additional details available."}
            for line in read_lines(DETAILS_DIR / f"{pid}.txt"):
                if ":" not in line:
                    continue
                field, value = line.split(":", 1)
                field, value = field.strip(), value.strip()
                if field in ("Price", "Cost"):
                    digits = "".join(c for c in value if c.isdigit())
                    info["price"] = int(digits) if digits else 0
                elif field == "Flower Type":
                    info["flower_type"] = value
                elif field == "Description":
                    info["description"] = value
            self.details[pid] = info

    # ---- accounts --------------------------------------------------------

    @property
    def account(self):
        return self.accounts[self.current]

    def load_account(self):
        acct = {"password": self.users.get(self.current, ""), "balance": 0, "purchases": "0"}
        for line in read_lines(INFO_DIR / f"{self.current}.txt"):
            if ":" not in line:
                continue
            field, value = line.split(":", 1)
            field, value = field.strip(), value.strip()
            if field == "Password":
                acct["password"] = value
            elif field == "Balance":
                try:
                    acct["balance"] = int(value)
                except ValueError:
                    acct["balance"] = 0
            elif field in ("Purchases", "Transactions", "Transations"):
                acct["purchases"] = value
        self.accounts[self.current] = acct

    def save_account(self):
        acct = self.accounts.get(self.current)
        if acct is None:
            return False
        try:
            with open(INFO_DIR / f"{self.current}.txt", "w", encoding="utf-8") as f:
                f.write(f"Username: {self.current}\n"
                        f"Password: {acct['password']}\n"
                        f"Balance: {acct['balance']}\n"
                        f"Purchases: {acct['purchases']}\n")
            return True
        except OSError:
            return False

    def log(self, message):
        try:
            with open(LOGS_DIR / f"{self.current}.txt", "a", encoding="utf-8") as f:
                f.write(f"{timestamp()} - {message}\n")
        except OSError:
            pass

    def add_history(self, line):
        try:
            with open(HISTORY_DIR / f"{self.current}.txt", "a", encoding="utf-8") as f:
                f.write(line + "\n")
        except OSError:
            pass

    def register(self, username, password):
        """Returns an error message, or None on success (and logs the user in)."""
        if not USERNAME_RE.fullmatch(username) or username.startswith("."):
            return "Username may only contain letters, numbers, spaces, '_', '-' and '.'."
        if username in self.users:
            return "Username already exists."
        try:
            with open(USERS_FILE, "a", encoding="utf-8") as f:
                f.write(f"{username}|{password}\n")
        except OSError:
            return "Error saving user data."
        self.users[username] = password
        self.current = username
        self.accounts[username] = {"password": password, "balance": 0, "purchases": "0"}
        self.save_account()
        (LOGS_DIR / f"{username}.txt").write_text("", encoding="utf-8")
        self.log("Account Created")
        (HISTORY_DIR / f"{username}.txt").write_text("", encoding="utf-8")
        (RECEIPTS_DIR / username).mkdir(parents=True, exist_ok=True)
        self.load_account()
        return None

    def login(self, username, password):
        if self.users.get(username) != password:
            return False
        self.current = username
        self.load_account()
        self.log("Account Logged In")
        (RECEIPTS_DIR / username).mkdir(parents=True, exist_ok=True)
        return True

    # ---- receipts --------------------------------------------------------

    def new_receipt_id(self, prefix):
        folder = RECEIPTS_DIR / self.current
        folder.mkdir(parents=True, exist_ok=True)
        base = f"{prefix}-{int(time.time() * 1000)}"
        rid, n = base, 1
        while (folder / f"{rid}.txt").exists():
            rid = f"{base}-{n}"
            n += 1
        return rid

    def save_receipt(self, rid, text):
        try:
            folder = RECEIPTS_DIR / self.current
            folder.mkdir(parents=True, exist_ok=True)
            (folder / f"{rid}.txt").write_text(text, encoding="utf-8")
            return True
        except OSError:
            return False

    def delete_receipt(self, rid):
        try:
            (RECEIPTS_DIR / self.current / f"{rid}.txt").unlink()
        except OSError:
            pass

    def list_receipts(self):
        folder = RECEIPTS_DIR / self.current
        if not folder.is_dir():
            return []
        return sorted(p for p in folder.iterdir() if p.is_file() and p.suffix == ".txt")

    # ---- cart ------------------------------------------------------------

    def add_to_cart(self, pid, name, price, qty):
        for item in self.cart:
            if item["id"] == pid:
                item["qty"] += qty
                return
        self.cart.append({"id": pid, "name": name, "price": price, "qty": qty})

    def change_qty(self, index, delta):
        """+1 / -1 on a cart row (removes the row at qty 1). Returns row to select."""
        item = self.cart[index]
        if delta > 0:
            item["qty"] += 1
        elif item["qty"] > 1:
            item["qty"] -= 1
        else:
            del self.cart[index]
            return min(index, len(self.cart) - 1) if self.cart else -1
        return index

    def cart_total(self):
        return sum(i["price"] * i["qty"] for i in self.cart)

    # ---- money -----------------------------------------------------------

    def deposit(self, amount_text):
        """Returns (ok, error message or receipt text)."""
        text = amount_text.strip()
        if not text or not (text.isascii() and text.isdigit()):
            return False, "Enter a whole-number deposit amount."
        amount = int(text)
        if amount <= 0:
            return False, "The deposit must be greater than zero."
        if amount > MAX_DEPOSIT:
            return False, f"The maximum deposit is {money(MAX_DEPOSIT)}."

        acct = self.account
        rid = self.new_receipt_id("DEP")
        ts = timestamp()
        new_balance = acct["balance"] + amount
        receipt = ("FLOWER SHOP DEPOSIT RECEIPT\n\n"
                   f"Receipt ID: {rid}\n"
                   f"Date: {ts}\n"
                   f"Customer: {self.current}\n"
                   "Transaction: Deposit\n"
                   f"Amount Received: {money(amount)}\n"
                   f"New Balance: {money(new_balance)}\n"
                   "Status: Successful\n")
        if not self.save_receipt(rid, receipt):
            return False, "Could not save the deposit receipt."
        previous = acct["balance"]
        acct["balance"] = new_balance
        if not self.save_account():
            acct["balance"] = previous
            self.delete_receipt(rid)
            return False, "Could not save the updated account balance."
        self.add_history(f"{ts} - Deposit {rid}: {money(amount)}")
        return True, receipt

    def checkout(self, method):
        """method is 'Cash' or 'Credit'. Returns (ok, error message or receipt text)."""
        if not self.cart:
            return False, "Your cart is empty."
        total = self.cart_total()
        acct = self.account
        if method == "Credit" and acct["balance"] < total:
            return False, "Insufficient system balance for credit payment."

        ts = timestamp()
        oid = self.new_receipt_id("ORD")
        status = "Paid Immediately" if method == "Cash" else "Balance Deducted"
        lines = ["FLOWER SHOP ORDER RECEIPT", "",
                 f"Receipt ID: {oid}",
                 f"Customer: {self.current}",
                 f"Date: {ts}",
                 f"Payment Method: {method}",
                 f"Payment Status: {status}", "",
                 "Items:"]
        for it in self.cart:
            lines.append(f"- {it['name']} x{it['qty']} @ {money(it['price'])} "
                         f"= {money(it['price'] * it['qty'])}")
        lines += ["", f"Total: {money(total)}", "Status: Successful", ""]
        receipt = "\n".join(lines)

        previous = dict(acct)
        if method == "Credit":
            acct["balance"] -= total
        try:
            acct["purchases"] = str(int(acct["purchases"]) + 1)
        except ValueError:
            acct["purchases"] = "1"

        if not self.save_receipt(oid, receipt):
            acct.update(previous)
            return False, "Could not save the receipt."
        if not self.save_account():
            acct.update(previous)
            self.delete_receipt(oid)
            return False, "Could not save the updated account."
        self.add_history(f"{ts} - Order {oid} - {method} - {money(total)}")
        self.cart.clear()
        return True, receipt


# --------------------------------------------------------------------------
# UI
# --------------------------------------------------------------------------

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.store = Store()
        self.configure(bg=BG)
        self.option_add("*Font", f"{{{FONT}}} 10")

        w, h = 800, 600
        x = (self.winfo_screenwidth() - w) // 2
        y = (self.winfo_screenheight() - h) // 2
        self.geometry(f"{w}x{h}+{x}+{y}")
        self.minsize(720, 520)

        self.frame = None
        self.scroll_canvas = None
        self.menu_scroll = 0.0
        self.thumbs = {}

        self.bind_all("<MouseWheel>", self._on_wheel)
        self.bind_all("<Button-4>", self._on_wheel)
        self.bind_all("<Button-5>", self._on_wheel)

        self.show_landing()

    # ---- small widget helpers -------------------------------------------

    def screen(self, title):
        if self.scroll_canvas is not None and self.scroll_canvas.winfo_exists():
            self.menu_scroll = self.scroll_canvas.yview()[0]
        self.scroll_canvas = None
        if self.frame is not None:
            self.frame.destroy()
        self.title(title)
        self.frame = tk.Frame(self, bg=BG)
        self.frame.pack(fill="both", expand=True)
        return self.frame

    def lbl(self, parent, text, size=11, bold=False, anchor="w", **kw):
        return tk.Label(parent, text=text, bg=BG, fg=FG, anchor=anchor, justify="left",
                        font=(FONT, size, "bold" if bold else "normal"), **kw)

    def btn(self, parent, text, command):
        return tk.Button(parent, text=text, command=command)

    def back_btn(self, parent, command):
        self.btn(parent, "Back", command).place(x=10, y=10, width=90, height=30)

    @staticmethod
    def place_c(widget, x, y, w, h):
        """Place using original 800px-wide coordinates, but centered if resized."""
        widget.place(relx=0.5, x=x - 400, y=y, width=w, height=h)

    def _on_wheel(self, event):
        canvas = self.scroll_canvas
        if canvas is None or not canvas.winfo_exists():
            return
        if event.num == 4:
            step = -1
        elif event.num == 5:
            step = 1
        else:
            step = -1 if event.delta > 0 else 1
        canvas.yview_scroll(step * 3, "units")

    # ---- landing / login / register -------------------------------------

    def show_landing(self):
        f = self.screen("FLOWER SHOP ORDERING SYSTEM")
        self.place_c(self.lbl(f, "FLOWER SHOP ORDERING SYSTEM", 22, True, "center"), 100, 100, 600, 40)
        self.place_c(self.btn(f, "PROCEED", self.show_login_register), 340, 200, 120, 40)

    def show_login_register(self):
        f = self.screen("Login and Registration")
        self.place_c(self.btn(f, "Log In", self.show_login), 340, 250, 120, 40)
        self.place_c(self.btn(f, "Register", self.show_register), 340, 300, 120, 40)

    def _credentials_form(self, title, on_back, on_submit):
        f = self.screen(title)
        self.back_btn(f, on_back)
        self.place_c(self.lbl(f, "Username:"), 200, 150, 100, 30)
        self.place_c(self.lbl(f, "Password:"), 200, 200, 100, 30)
        user = tk.Entry(f)
        pw = tk.Entry(f, show="*")
        self.place_c(user, 300, 150, 200, 30)
        self.place_c(pw, 300, 200, 200, 30)

        def submit():
            on_submit(user.get().strip(), pw.get())

        self.place_c(self.btn(f, "Submit", submit), 340, 250, 120, 40)
        user.bind("<Return>", lambda e: pw.focus_set())
        pw.bind("<Return>", lambda e: submit())
        user.focus_set()

    def show_login(self):
        def submit(username, password):
            if not username or not password:
                messagebox.showinfo("Login", "Please enter username and password.")
            elif self.store.login(username, password):
                self.menu_scroll = 0.0
                self.show_main_menu()
            else:
                messagebox.showinfo("Login", "Invalid username or password.")

        self._credentials_form("Log In", self.show_login_register, submit)

    def show_register(self):
        def submit(username, password):
            if not username or not password:
                messagebox.showinfo("Register", "Please enter username and password.")
                return
            error = self.store.register(username, password)
            if error:
                messagebox.showinfo("Register", error)
            else:
                self.menu_scroll = 0.0
                self.show_main_menu()

        self._credentials_form("Register", self.show_login_register, submit)

    def logout(self):
        self.menu_scroll = 0.0
        self.show_login_register()

    # ---- main menu -------------------------------------------------------

    def show_main_menu(self):
        s = self.store
        f = self.screen("Main Menu")
        self.btn(f, "Exit", self.destroy).place(x=10, y=10, width=90, height=30)
        self.btn(f, "Log Out", self.logout).place(x=10, y=50, width=90, height=30)
        self.btn(f, "More", self.show_more).place(x=10, rely=1, y=-45, width=90, height=30)

        self.lbl(f, f"Welcome Back, {s.current}!", 14, True, "center").place(
            relx=0.5, x=-200, y=10, width=400, height=30)
        self.lbl(f, "FLOWER SHOP CATALOG", 22, True, "center").place(
            relx=0.5, x=-220, y=40, width=440, height=40)

        self.btn(f, "Cart", self.show_cart).place(relx=1, x=-170, y=10, width=90, height=30)
        self.lbl(f, f"BALANCE: {money(s.account['balance'])}", 12, True).place(
            relx=1, x=-210, y=50, width=200, height=30)
        self.btn(f, "Deposit", self.open_deposit).place(relx=1, x=-170, y=80, width=90, height=25)

        view = tk.Frame(f, bg=BG)
        view.place(relx=0.5, x=-335, y=110, width=670, relheight=1, height=-130)
        canvas = tk.Canvas(view, bg=BG, highlightthickness=0)
        scrollbar = ttk.Scrollbar(view, orient="vertical", command=canvas.yview)
        canvas.configure(yscrollcommand=scrollbar.set)
        scrollbar.pack(side="right", fill="y")
        canvas.pack(side="left", fill="both", expand=True)
        inner = tk.Frame(canvas, bg=BG)
        canvas.create_window((0, 0), window=inner, anchor="nw")
        inner.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))
        self.scroll_canvas = canvas

        buttons = []
        for i, (name, pid) in enumerate(s.products):
            tile = tk.Frame(inner, width=140, height=140, bg=BG)
            tile.grid(row=i // 4, column=i % 4, padx=10, pady=10)
            tile.pack_propagate(False)
            b = tk.Button(tile, text=name, wraplength=120, highlightthickness=0,
                          command=lambda idx=i: self.show_product(idx))
            b.pack(fill="both", expand=True)
            buttons.append(b)
        self._load_thumbs(buttons)
        self.after(50, lambda: canvas.winfo_exists() and canvas.yview_moveto(self.menu_scroll))

    def _load_thumbs(self, buttons, i=0):
        """Fill in product images one at a time so the menu opens instantly."""
        if i >= len(buttons):
            return
        b = buttons[i]
        if b.winfo_exists():
            name, pid = self.store.products[i]
            img = self.thumbs.get(pid)
            if img is None:
                img = load_photo(image_path(pid), 140, 140) or default_tile(name, 140, 140)
                if img is not None:
                    self.thumbs[pid] = img
            if img is not None:
                b.configure(image=img, text="")
            self.after(1, lambda: self._load_thumbs(buttons, i + 1))

    # ---- deposit dialog --------------------------------------------------

    def open_deposit(self):
        dlg = tk.Toplevel(self)
        dlg.title("Deposit Funds")
        dlg.configure(bg=BG)
        dlg.resizable(False, False)
        w, h = 420, 260
        x = self.winfo_x() + (self.winfo_width() - w) // 2
        y = self.winfo_y() + (self.winfo_height() - h) // 2
        dlg.geometry(f"{w}x{h}+{x}+{y}")
        dlg.transient(self)
        dlg.grab_set()

        self.lbl(dlg, "Amount (P):").place(x=55, y=75, width=90, height=28)
        digits_only = (self.register(lambda v: v == "" or (v.isascii() and v.isdigit())), "%P")
        amount = tk.Entry(dlg, validate="key", validatecommand=digits_only)
        amount.place(x=150, y=70, width=200, height=30)
        amount.focus_set()

        def submit():
            ok, result = self.store.deposit(amount.get())
            if not ok:
                messagebox.showwarning("Deposit", result, parent=dlg)
                return
            dlg.destroy()
            self.show_receipt(result, self.show_main_menu)

        self.btn(dlg, "Deposit", submit).place(x=145, y=130, width=100, height=35)
        self.btn(dlg, "Back", dlg.destroy).place(x=255, y=130, width=90, height=35)
        amount.bind("<Return>", lambda e: submit())

    # ---- product details -------------------------------------------------

    def show_product(self, index):
        s = self.store
        name, pid = s.products[index]
        det = s.details.get(pid)
        f = self.screen("ProductDetails")
        self.back_btn(f, self.show_main_menu)

        photo = load_photo(image_path(pid), 300, 300) or default_tile(name, 300, 300)
        if photo is not None:
            pic = tk.Label(f, image=photo, bg=BG)
            pic.image = photo
        else:
            pic = tk.Label(f, text=name, bg="#FFE0E6", fg=FG, relief="ridge", bd=2,
                           wraplength=260, font=(FONT, 14, "bold"))
        self.place_c(pic, 40, 90, 300, 300)

        self.place_c(self.lbl(f, name, 16, True), 390, 100, 340, 45)
        if det:
            text = (f"Price: {money(det['price'])}\n\n"
                    f"Flower Type: {det['flower_type']}\n\n"
                    f"Description: {det['description']}")
        else:
            text = "No additional details available."
        self.place_c(self.lbl(f, text, 11, anchor="nw", wraplength=340), 390, 165, 340, 250)

        self.place_c(self.lbl(f, "Quantity:"), 390, 430, 70, 25)
        digits_only = (self.register(lambda v: v == "" or (v.isascii() and v.isdigit())), "%P")
        qty = tk.Entry(f, validate="key", validatecommand=digits_only)
        qty.insert(0, "1")
        self.place_c(qty, 465, 425, 80, 28)

        def add():
            try:
                quantity = int(qty.get())
            except ValueError:
                quantity = 0
            if quantity <= 0:
                messagebox.showinfo("Add to Cart", "Please enter a valid quantity.")
                return
            if det is None:
                messagebox.showinfo("Add to Cart", "Could not load this product's price.")
                return
            s.add_to_cart(pid, name, det["price"], quantity)
            messagebox.showinfo("Add to Cart", "Added to cart successfully!")

        self.place_c(self.btn(f, "Add to Cart", add), 390, 465, 120, 40)

    # ---- cart / checkout / receipt --------------------------------------

    def show_cart(self):
        s = self.store
        f = self.screen("Cart")
        self.back_btn(f, self.show_main_menu)

        box = tk.Frame(f)
        box.place(x=35, y=60, relwidth=1, width=-70, relheight=1, height=-185)
        box.grid_rowconfigure(0, weight=1)
        box.grid_columnconfigure(0, weight=1)
        lb = tk.Listbox(box, exportselection=False, activestyle="none")
        vs = ttk.Scrollbar(box, orient="vertical", command=lb.yview)
        hs = ttk.Scrollbar(box, orient="horizontal", command=lb.xview)
        lb.configure(yscrollcommand=vs.set, xscrollcommand=hs.set)
        lb.grid(row=0, column=0, sticky="nsew")
        vs.grid(row=0, column=1, sticky="ns")
        hs.grid(row=1, column=0, sticky="ew")

        total_var = tk.StringVar()

        def refresh(selected=-1):
            lb.delete(0, "end")
            for it in s.cart:
                lb.insert("end", f"{it['name']} | ID: {it['id']} | Qty: {it['qty']} | "
                                 f"Unit: {money(it['price'])} | "
                                 f"Total: {money(it['price'] * it['qty'])}")
            if 0 <= selected < len(s.cart):
                lb.selection_set(selected)
            elif s.cart:
                lb.selection_set(0)
            total_var.set(f"Total: {money(s.cart_total())}")

        def change(delta):
            sel = lb.curselection()
            if sel and sel[0] < len(s.cart):
                refresh(s.change_qty(sel[0], delta))

        def checkout():
            if not s.cart:
                messagebox.showwarning("Check Out", "Your cart is empty.")
            else:
                self.show_checkout()

        self.btn(f, "-", lambda: change(-1)).place(x=35, rely=1, y=-165, width=45, height=35)
        self.btn(f, "+", lambda: change(1)).place(x=90, rely=1, y=-165, width=45, height=35)
        tk.Label(f, textvariable=total_var, bg=BG, fg=FG, anchor="w",
                 font=(FONT, 11)).place(relx=1, x=-255, rely=1, y=-160, width=210, height=30)
        self.btn(f, "Check Out", checkout).place(relx=1, x=-145, rely=1, y=-115, width=130, height=40)
        refresh()

    def show_checkout(self):
        s = self.store
        f = self.screen("Check Out")
        self.place_c(self.lbl(f, "ORDER CONFIRMATION", 16, True, "center"), 220, 20, 360, 35)
        self.place_c(self.lbl(f, f"Total: {money(s.cart_total())}", 12), 80, 90, 260, 30)
        self.place_c(self.lbl(f, "Payment Method:", 12), 80, 150, 160, 30)
        combo = ttk.Combobox(f, values=("Cash", "Credit"), state="readonly")
        combo.current(0)
        self.place_c(combo, 240, 148, 180, 28)
        self.place_c(self.lbl(f, f"Items in cart: {len(s.cart)}", 12), 80, 200, 300, 30)

        def confirm():
            ok, result = s.checkout(combo.get())
            if not ok:
                messagebox.showwarning("Checkout", result)
                return
            self.show_receipt(result, self.show_main_menu)

        self.btn(f, "Back", self.show_cart).place(x=80, rely=1, y=-100, width=120, height=40)
        self.btn(f, "Confirm Order", confirm).place(relx=1, x=-200, rely=1, y=-100, width=120, height=40)

    def show_receipt(self, text, on_continue):
        f = self.screen("Receipt")
        self.place_c(self.lbl(f, "FLOWER SHOP RECEIPT", 20, True, "center"), 150, 20, 500, 40)
        box = tk.Frame(f)
        box.place(x=80, y=80, relwidth=1, width=-160, relheight=1, height=-190)
        box.grid_rowconfigure(0, weight=1)
        box.grid_columnconfigure(0, weight=1)
        body = tk.Text(box, wrap="word", font=(FONT, 12), fg=FG, bg=BG)
        vs = ttk.Scrollbar(box, orient="vertical", command=body.yview)
        body.configure(yscrollcommand=vs.set)
        body.grid(row=0, column=0, sticky="nsew")
        vs.grid(row=0, column=1, sticky="ns")
        body.tag_configure("center", justify="center")
        body.insert("1.0", text, "center")
        body.configure(state="disabled")
        self.btn(f, "Continue", on_continue).place(relx=1, x=-180, rely=1, y=-90, width=130, height=40)

    # ---- more / profile / about / history -------------------------------

    def show_more(self):
        f = self.screen("More")
        self.back_btn(f, self.show_main_menu)
        self.place_c(self.btn(f, "About Us", self.show_about), 330, 100, 140, 30)
        self.place_c(self.btn(f, "Profile", self.show_profile), 330, 150, 140, 30)
        self.place_c(self.btn(f, "Transaction History", self.show_history), 290, 200, 220, 40)

    def show_about(self):
        f = self.screen("AboutUs")
        self.back_btn(f, self.show_more)
        lines = read_lines(ABOUT_FILE)
        text = "\n".join(lines) if lines else "No information available."
        self.lbl(f, text, 11, anchor="nw", wraplength=700).place(
            x=40, y=60, relwidth=1, width=-80, relheight=1, height=-80)

    def show_profile(self):
        s = self.store
        acct = s.account
        f = self.screen("Profile")
        self.back_btn(f, self.show_more)

        photo = load_photo(PROFILE_IMAGE, 128, 128)
        if photo is not None:
            pic = tk.Label(f, image=photo, bg=BG)
            pic.image = photo
            self.place_c(pic, 336, 145, 128, 128)

        self.place_c(self.lbl(f, f"Username: {s.current}"), 300, 300, 300, 30)
        self.place_c(self.lbl(f, f"Password: {acct['password']}"), 300, 340, 300, 30)
        self.place_c(self.lbl(f, f"Balance: {money(acct['balance'])}"), 300, 380, 300, 30)
        self.place_c(self.lbl(f, f"Purchases: {acct['purchases']}"), 300, 420, 400, 30)

    def show_history(self):
        f = self.screen("Transaction History")
        self.back_btn(f, self.show_more)
        receipts = self.store.list_receipts()

        box = tk.Frame(f)
        box.place(x=35, y=60, relwidth=1, width=-70, relheight=1, height=-90)
        box.grid_rowconfigure(0, weight=1)
        box.grid_columnconfigure(0, weight=1)
        lb = tk.Listbox(box, exportselection=False, activestyle="none")
        vs = ttk.Scrollbar(box, orient="vertical", command=lb.yview)
        lb.configure(yscrollcommand=vs.set)
        lb.grid(row=0, column=0, sticky="nsew")
        vs.grid(row=0, column=1, sticky="ns")

        if not receipts:
            lb.insert("end", "No receipts available.")
        for path in receipts:
            lb.insert("end", path.stem)

        def open_selected(event=None):
            sel = lb.curselection()
            if not sel or sel[0] >= len(receipts):
                return
            try:
                text = receipts[sel[0]].read_text(encoding="utf-8", errors="replace").strip()
            except OSError:
                text = ""
            self.show_receipt(text or "No receipt details available.", self.show_history)

        lb.bind("<Double-Button-1>", open_selected)
        lb.bind("<Return>", open_selected)


if __name__ == "__main__":
    os.chdir(Path(__file__).resolve().parent)  # data files live next to the script
    App().mainloop()
