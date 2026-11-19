# Flower Shop Management System (C++)

## Project Overview

This is a flower shop ordering program written in C++ (a Windows desktop program built with the Win32 API, so it opens windows with buttons). A customer can register or log in, browse the flowers, add them to a cart, and check out using **Cash** (money deposited in the account) or **Credit** (buy now, pay later up to a credit limit). Credit purchases become **outstanding debt**, which the customer can pay back in full or in parts.

All data is saved in plain `.txt` files inside the `Main` folder, so no database is needed.

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

Everything is in `Main/orderSystem.cpp`.

**Structure `userInformation`** – what we store for each customer:

| Variable | Meaning |
|---|---|
| `curUserPassword` | The customer's password |
| `curUserBalance` | Cash balance (money from deposits) |
| `curUserPurchases` | Number of purchases made |
| `curUserCreditLimit` | The most debt this customer may have |
| `curUserOutstandingDebt` | Credit purchases that are not yet paid |

**Other structures:** `productInformation` (one flower), `cartItem` (one line in the cart), `orderDetail`.

**Important variables**
- `DEFAULT_CREDIT_LIMIT` – credit limit given to new accounts (`10000`).
- `userInformationMap` – all loaded customers (username -> `userInformation`).
- `currentUser` – the username of the logged-in customer.
- `cartItemVector` – the items currently in the cart.

**Payment functions** (section `PAYMENT LOGIC`; they only work with numbers and files, not windows):

| Function | What it does |
|---|---|
| `GetRemainingCredit` | Returns credit limit - outstanding debt (never below 0) |
| `GetCartTotal` | Adds up the cart items |
| `PayWithCash` | Checks the balance and deducts the total |
| `PayWithCredit` | Checks the remaining credit and adds the total to the debt |
| `PayDebt` | Takes the payment from the balance and lowers the debt (rejects 0, amounts above the debt, and amounts above the balance) |
| `ProcessCheckout` | Full checkout: pay, make the receipt, save files, update history, empty the cart |
| `ProcessDebtPayment` | Full debt payment: pay, make the receipt, save files, update history |

**Window functions** (each screen has a `...WindowProc` that handles button clicks and a `Create...Form` that builds the buttons and texts):
- `CheckOutWindowProc` / `CreateCheckOutForm` – the checkout screen
- `PayDebtWindowProc` / `CreatePayDebtForm` – the Pay Debt screen
- `DepositWindowProc` – deposits
- `CreateCreditInfoLabels` / `UpdateCreditInfoLabels` – show credit limit, debt and remaining credit

**File functions:** `LoadCurrentUserDetails` and `SaveCurrentUserDetails` (read/write `User Informations/<username>.txt`), `SaveReceiptTextFile` (saves into `Receipts/<username>/`), `CreateReceiptId` (makes ids like `DEP-...` and `PAY-...`; order ids `ORD-...` are made in `ProcessCheckout`).

Each account file looks like this:

```
Username: Ches
Password: 12345
Balance: 4200
Purchases: 6
Credit Limit: 10000
Outstanding Debt: 0
```

## Payment System Explanation

**Cash payment** – uses the account balance (`curUserBalance`). If the balance is enough, the total is deducted and the new balance is shown on the receipt. If it is not enough, the purchase is rejected with an error message and nothing is deducted.

**Credit payment** – does **not** touch the balance. The total is added to `curUserOutstandingDebt`. The purchase is only approved if the total fits in the remaining credit. Otherwise it is rejected and the debt does not change.

**Credit limit** – every customer has their own `curUserCreditLimit`, saved on the `Credit Limit:` line of their account file. New accounts get `DEFAULT_CREDIT_LIMIT` (10,000). To give one customer a different limit, edit that line in their file. Old account files without this line also get 10,000 when loaded.

**Outstanding debt** – the total of credit purchases that were not paid yet (`curUserOutstandingDebt`).

**Debt repayment** – More > Pay Debt. The customer types an amount (full or partial). The amount is deducted from the cash balance (`curUserBalance`) and subtracted from the debt, so the same amount of credit becomes available again. The customer cannot pay more than the debt, and cannot pay more than their balance (they must deposit first).

**Information shown** – Credit Limit, Outstanding Debt and Remaining Credit appear on the Checkout screen, the Pay Debt screen and the Profile page (the Pay Debt screen also shows the balance). There are no due dates and no interest.

## Important Code Logic

**Remaining credit** (`GetRemainingCredit`)
```cpp
remainingCredit = curUserCreditLimit - curUserOutstandingDebt;   // never below 0
```
Example: limit 10,000 and debt 4,000 -> remaining credit 6,000.

**Cash check** (`PayWithCash`)
```cpp
if (user.curUserBalance < totalAmount)  -> reject
else  user.curUserBalance -= totalAmount;
```
- Balance 5,000, total 2,100 -> approved, new balance 2,900.
- Balance 1,000, total 2,100 -> rejected, balance stays 1,000.
- Balance exactly 2,100, total 2,100 -> approved, new balance 0. The next purchase is rejected because 0 is less than any total.

**Credit check** (`PayWithCredit`)
```cpp
if (totalAmount > GetRemainingCredit(user))  -> reject
else  user.curUserOutstandingDebt += totalAmount;
```
- Limit 10,000, debt 4,000, total 6,000 -> approved (exactly the remaining credit), debt becomes 10,000.
- Limit 10,000, debt 4,000, total 6,001 -> rejected.

**Debt payment** (`PayDebt`)
```cpp
if (paymentAmount <= 0)                              -> reject
if (user.curUserOutstandingDebt <= 0)                -> reject (no debt)
if (paymentAmount > user.curUserOutstandingDebt)     -> reject (paying too much)
if (user.curUserBalance < paymentAmount)             -> reject (not enough balance)
else {
    user.curUserBalance -= paymentAmount;
    user.curUserOutstandingDebt -= paymentAmount;
}
```
- Balance 2,000, debt 4,000, pay 1,500 -> balance 500, debt 2,500, remaining credit 7,500.
- Balance 500, debt 2,500, pay 2,500 -> rejected (balance too low). After depositing 2,500 more, paying 2,500 -> debt 0, remaining credit 10,000.
- Debt 2,000, pay 2,001 -> rejected (more than the debt).

**Safe saving** – `ProcessCheckout` and `ProcessDebtPayment` keep a copy of the account (`backupUser`) before charging. If saving a file fails, the copy is restored so the customer is never charged without a saved receipt.

## How to Run

Needs a Windows PC with MinGW g++ (for example MSYS2 UCRT64).

1. Open a terminal in the `Main` folder (the program reads its `.txt` files and pictures from the folder it is run from).
2. Compile:
   ```
   g++ orderSystem.cpp -o orderSystem.exe -mwindows -lgdi32 -luser32 -lgdiplus
   ```
   (In VS Code you can also press `Ctrl+Shift+B`, which uses `.vscode/tasks.json`.)
3. Run:
   ```
   orderSystem.exe
   ```
4. Try it: log in with `Ches` / `12345` (or `User` / `123`), add flowers to the cart, check out with Cash or Credit, and use More > Pay Debt to repay credit purchases (deposit money first if the balance is low).
