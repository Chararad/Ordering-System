"""GUI test: opens the real tkinter app and clicks its buttons (needs a display).

Run from the project folder:   python tests/test_gui.py
(On a machine without a screen, run it with:  xvfb-run -a python tests/test_gui.py)
The test works on a temporary copy of the project, so your real data files are not changed.
"""
import importlib.util
import os
import shutil
import sys
import tempfile
import tkinter as tk
from pathlib import Path

PROJECT = Path(__file__).resolve().parent.parent
tmp = Path(tempfile.mkdtemp())
shutil.copytree(PROJECT, tmp / "app", ignore=shutil.ignore_patterns(".git", ".idea", "tests", "__pycache__"))
os.chdir(tmp / "app")

# a test customer: balance 2,000 / credit limit 10,000 / debt 4,000
Path("users&passwords.txt").write_text("guitest|pw\n", encoding="utf-8")
Path("User Informations/guitest.txt").write_text(
    "Username: guitest\nPassword: pw\nBalance: 2000\nPurchases: 0\n"
    "Credit Limit: 10000\nOutstanding Debt: 4000\n", encoding="utf-8")

spec = importlib.util.spec_from_file_location("orderSystem", tmp / "app" / "orderSystem.py")
orderSystem = importlib.util.module_from_spec(spec)
sys.modules["orderSystem"] = orderSystem
spec.loader.exec_module(orderSystem)

popups = []                                            # every message box the app tries to show
for kind in ("showinfo", "showwarning", "showerror"):
    setattr(orderSystem.messagebox, kind, lambda title, message, _k=kind, **kw: popups.append(message))

passed = failed = 0
def check(condition, label):
    global passed, failed
    if condition:
        passed += 1
        print("  PASS ", label)
    else:
        failed += 1
        print("  FAIL ", label)

def walk(widget):
    for child in widget.winfo_children():
        yield child
        yield from walk(child)

def button(text):
    for w in walk(app.frame):
        if isinstance(w, tk.Button) and w.cget("text") == text:
            return w
    raise AssertionError(f"no button '{text}' on this screen")

def click(text):
    button(text).invoke()
    app.update()

def label_texts():
    return [w.cget("text") for w in walk(app.frame) if isinstance(w, tk.Label)]

def has_label(text):
    return text in label_texts()

def entries():
    return [w for w in walk(app.frame) if isinstance(w, tk.Entry)]

def type_into(entry, text):
    entry.delete(0, "end")
    entry.insert(0, text)

def last_popup():
    return popups[-1] if popups else ""

app = orderSystem.App()
app.update()

print("=== Log in ===")
click("PROCEED"); click("Log In")
user, pw = entries()
type_into(user, "guitest"); type_into(pw, "pw")
click("Submit")
check(app.title() == "Main Menu", "logged in, main menu shown")
check(has_label("BALANCE: P2,000"), "main menu shows the balance")

print("=== More menu -> Pay Debt screen ===")
click("More")
check(all(button(t) for t in ("About Us", "Profile", "Transaction History", "Back")), "old More buttons still there")
click("Pay Debt")
check(app.title() == "Pay Debt", "Pay Debt screen opened")
check(has_label("Credit Limit: P10,000") and has_label("Outstanding Debt: P4,000")
      and has_label("Remaining Credit: P6,000") and has_label("Your Balance: P2,000"),
      "shows credit limit, debt, remaining credit and balance")

print("=== Pay Debt: bad amounts ===")
amount = entries()[0]
type_into(amount, ""); click("Pay Debt")
check("whole-number" in last_popup(), "empty amount -> " + last_popup())
type_into(amount, "0"); click("Pay Debt")
check("greater than zero" in last_popup(), "0 -> " + last_popup())
type_into(amount, "4001"); click("Pay Debt")
check("cannot pay more than your outstanding debt of P4,000" in last_popup(), "4001 -> " + last_popup())
type_into(amount, "2500"); click("Pay Debt")
check("Insufficient balance" in last_popup(), "2,500 with balance 2,000 -> " + last_popup().splitlines()[0])
check(app.store.account["debt"] == 4000 and app.store.account["balance"] == 2000, "debt and balance unchanged")

print("=== Pay Debt: partial payment of 1,500 ===")
type_into(amount, "1500"); click("Pay Debt")
check(app.title() == "Receipt", "receipt screen shown")
receipt_text = [w for w in walk(app.frame) if isinstance(w, tk.Text)][0].get("1.0", "end")
check("Amount Paid: P1,500" in receipt_text and "Updated Balance: P500" in receipt_text, "receipt shows payment + new balance")
click("Continue")
check(app.title() == "Pay Debt", "Continue returns to the Pay Debt screen")
check(has_label("Outstanding Debt: P2,500") and has_label("Remaining Credit: P7,500") and has_label("Your Balance: P500"),
      "screen shows the new numbers")
click("Back")
check(app.title() == "More", "Back returns to the More menu")
click("Back")

print("=== Checkout screen ===")
app.store.add_to_cart("001", "Blue Lagoon", 2100, 4)          # total 8,400
click("Cart"); click("Check Out")
check(app.title() == "Check Out", "checkout opened")
check(has_label("Total: P8,400") and has_label("Your Balance: P500") and has_label("Credit Limit: P10,000")
      and has_label("Outstanding Debt: P2,500") and has_label("Remaining Credit: P7,500"),
      "shows total, balance and credit info")
combo = [w for w in walk(app.frame) if w.winfo_class() == "TCombobox"][0]
check(tuple(combo.cget("values")) == ("Cash", "Credit"), "payment methods still Cash + Credit")
combo.set("Cash"); click("Confirm Order")
check("Insufficient balance" in last_popup(), "Cash with too little balance -> " + last_popup().splitlines()[0])
combo.set("Credit"); click("Confirm Order")
check("Credit limit exceeded" in last_popup(), "Credit above remaining credit -> " + last_popup().splitlines()[0])
check(app.store.account["debt"] == 2500 and app.store.account["balance"] == 500 and len(app.store.cart) == 1,
      "nothing charged, cart kept")

print("=== Credit purchase within the limit ===")
click("Back")                                                  # back to the cart
click("-"); click("-")                                         # quantity 4 -> 2  (total 4,200)
click("Check Out")
combo = [w for w in walk(app.frame) if w.winfo_class() == "TCombobox"][0]
combo.set("Credit"); click("Confirm Order")
check(app.title() == "Receipt", "order approved, receipt shown")
receipt_text = [w for w in walk(app.frame) if isinstance(w, tk.Text)][0].get("1.0", "end")
check("Charged to Credit" in receipt_text and "Outstanding Debt: P6,700" in receipt_text and "Remaining Credit: P3,300" in receipt_text,
      "credit receipt text")
check(app.store.account["balance"] == 500 and app.store.account["debt"] == 6700, "balance 500 (untouched), debt 6,700")
click("Continue")
check(app.title() == "Main Menu" and has_label("BALANCE: P500"), "back at main menu, balance still P500")

print("=== Profile ===")
click("More"); click("Profile")
check(has_label("Credit Limit: P10,000") and has_label("Outstanding Debt: P6,700") and has_label("Remaining Credit: P3,300"),
      "profile shows the credit lines")

app.destroy()
shutil.rmtree(tmp, ignore_errors=True)
print(f"\nGUI RESULT: {passed} passed, {failed} failed")
sys.exit(0 if failed == 0 else 1)
