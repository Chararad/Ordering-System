# Flower Shop Ordering System (Python)

## Project Overview

This is a flower shop ordering program written in Python with `tkinter` (a desktop program with windows and buttons). It is the Python version of our C++ Flower Shop Management System and works the same way. A customer can register or log in, browse the flowers, add them to a cart, and check out using **Cash** (money deposited in the account) or **Credit** (buy now, pay later up to a credit limit). Credit purchases become **outstanding debt**, which the customer can pay back in full or in parts.

All data is saved in plain `.txt` files next to `orderSystem.py`, so no database is needed. The data files use the same layout as the C++ version.

## Main Features

- Register and log in (`users&passwords.txt`)
- Product catalog with pictures, and a details page for each flower
- Cart: add items, change quantities (+ / -), see the total
- Checkout with **Cash** or **Credit**
- Deposit money into the account balance
- **Pay Debt** (More > Pay Debt): pay back credit purchases using the account balance
- Receipts for orders, deposits and debt payments
- Transaction history (double-click a receipt to open it)
- Profile page (username, password, balance, purchases, credit limit, debt, remaining credit)
- About Us page, Log Out and Exit

## Code Structure

Everything is in `orderSystem.py`. It has three parts: the payment functions, the `Store` class (data and files, no windows) and the `App` class (windows).

**Account dictionary** – what we keep for each customer (`Store.accounts[username]`):

| Key | Meaning |
|---|---|
| `"password"` | The customer's password |
| `"balance"` | Cash balance (money from deposits) |
| `"purchases"` | Number of purchases made |
| `"credit_limit"` | The most debt this customer may have |
| `"debt"` | Credit purchases that are not yet paid (outstanding debt) |

**Important variables**
- `DEFAULT_CREDIT_LIMIT` – credit limit given to new accounts (`10_000`).
- `Store.accounts` – all loaded customers (username -> account dictionary).
- `Store.current` – the username of the logged-in customer.
- `Store.cart` – the items currently in the cart (each has `id`, `name`, `price`, `qty`).

**Payment functions** (section "Payment logic"; they only work on an account dictionary, no windows or files). Each returns `None` when the payment is approved, or an error message when it is rejected:

| Function | What it does |
|---|---|
| `remaining_credit` | Returns credit limit - outstanding debt (never below 0) |
| `pay_with_cash` | Checks the balance and deducts the total |
| `pay_with_credit` | Checks the remaining credit and adds the total to the debt |
| `pay_debt` | Takes the payment from the balance and lowers the debt (rejects 0, amounts above the debt, and amounts above the balance) |

**`Store` methods**
- `checkout(method)` – full checkout ("Cash" or "Credit"): pay, make the receipt, save files, update history, empty the cart.
- `process_debt_payment(amount_text)` – full debt payment: check the amount, pay, make the receipt, save files, update history.
- `cart_total`, `add_to_cart`, `change_qty` – the cart.
- `deposit` – adds money to the balance.
- `load_account` / `save_account` – read/write `User Informations/<username>.txt`.
- `new_receipt_id`, `save_receipt` – receipts in `Receipts/<username>/` (ids like `ORD-...`, `DEP-...`, `PAY-...`).

**`App` methods (screens)**
- `show_checkout` – the checkout screen
- `show_pay_debt` – the Pay Debt screen
- `show_profile` – the profile page
- `credit_info` – shows credit limit, debt and remaining credit

Each account file looks like this:

```
Username: chad
Password: hhhh
Balance: 1000
Purchases: 1
Credit Limit: 10000
Outstanding Debt: 0
```

## Payment System Explanation

**Cash payment** – uses the account balance (`"balance"`). If the balance is enough, the total is deducted and the new balance is shown on the receipt. If it is not enough, the purchase is rejected with an error message and nothing is deducted.

**Credit payment** – does **not** touch the balance. The total is added to `"debt"`. The purchase is only approved if the total fits in the remaining credit. Otherwise it is rejected and the debt does not change.

**Credit limit** – every customer has their own `"credit_limit"`, saved on the `Credit Limit:` line of their account file. New accounts get `DEFAULT_CREDIT_LIMIT` (10,000). To give one customer a different limit, edit that line in their file. Old account files without this line also get 10,000 when loaded.

**Outstanding debt** – the total of credit purchases that were not paid yet (`"debt"`).

**Debt repayment** – More > Pay Debt. The customer types an amount (full or partial). The amount is deducted from the cash balance and subtracted from the debt, so the same amount of credit becomes available again. The customer cannot pay more than the debt, and cannot pay more than their balance (they must deposit first).

**Information shown** – Credit Limit, Outstanding Debt and Remaining Credit appear on the Checkout screen, the Pay Debt screen and the Profile page (the Pay Debt screen also shows the balance). There are no due dates and no interest.

## Important Code Logic

**Remaining credit** (`remaining_credit`)
```python
return max(0, account["credit_limit"] - account["debt"])   # never below 0
```
Example: limit 10,000 and debt 4,000 -> remaining credit 6,000.

**Cash check** (`pay_with_cash`)
```python
if account["balance"] < total:   -> reject
else:  account["balance"] -= total
```
- Balance 5,000, total 2,100 -> approved, new balance 2,900.
- Balance 1,000, total 2,100 -> rejected, balance stays 1,000.
- Balance exactly 2,100, total 2,100 -> approved, new balance 0. The next purchase is rejected because 0 is less than any total.

**Credit check** (`pay_with_credit`)
```python
if total > remaining_credit(account):   -> reject
else:  account["debt"] += total
```
- Limit 10,000, debt 4,000, total 6,000 -> approved (exactly the remaining credit), debt becomes 10,000.
- Limit 10,000, debt 4,000, total 6,001 -> rejected.

**Debt payment** (`pay_debt`)
```python
if amount <= 0:                  -> reject
if account["debt"] <= 0:         -> reject (no debt)
if amount > account["debt"]:     -> reject (paying too much)
if account["balance"] < amount:  -> reject (not enough balance)
else:
    account["balance"] -= amount
    account["debt"] -= amount
```
- Balance 2,000, debt 4,000, pay 1,500 -> balance 500, debt 2,500, remaining credit 7,500.
- Balance 500, debt 2,500, pay 2,500 -> rejected (balance too low). After depositing 2,500 more, paying 2,500 -> debt 0, remaining credit 10,000.
- Debt 2,000, pay 2,001 -> rejected (more than the debt).

**Safe saving** – `checkout` and `process_debt_payment` keep a copy of the account (`previous = dict(acct)`) before charging. If saving a file fails, the copy is put back (`acct.update(previous)`) so the customer is never charged without a saved receipt.

## How to Run

Needs Python 3.7 or newer (`tkinter` is included with the normal Windows and macOS installers). The pictures work without extra installs; `pip install pillow` is optional (nicer image scaling and the pink default tiles).

1. Open a terminal in the project folder (the one that contains `orderSystem.py`).
2. Run:
   ```
   python orderSystem.py
   ```
3. Try it: log in with `chad` / `hhhh` (or `ches` / `123`, `hello` / `test`), add flowers to the cart, check out with Cash or Credit, and use More > Pay Debt to repay credit purchases (deposit money first if the balance is low).
4. Optional tests: `python -m unittest discover -s tests -p "test_payment.py"` (payment rules) and `python tests/test_gui.py` (clicks through the screens; needs a display).
