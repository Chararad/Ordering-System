"""Tests for the payment system of orderSystem.py (cash, credit, credit limit, debt payments).

Run from anywhere:   python -m unittest test_payment -v
Each test works inside a temporary folder, so your real data files are never touched.
"""
import importlib.util
import os
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

# Load orderSystem.py (it sits one folder above this "tests" folder)
SOURCE = Path(__file__).resolve().parent.parent / "orderSystem.py"
spec = importlib.util.spec_from_file_location("orderSystem", SOURCE)
orderSystem = importlib.util.module_from_spec(spec)
sys.modules["orderSystem"] = orderSystem
spec.loader.exec_module(orderSystem)


class PaymentTests(unittest.TestCase):
    def setUp(self):
        self.old_cwd = os.getcwd()
        self.tmp = tempfile.mkdtemp()
        os.chdir(self.tmp)                      # the program reads/writes files in the current folder
        self.store = orderSystem.Store()

    def tearDown(self):
        os.chdir(self.old_cwd)
        shutil.rmtree(self.tmp, ignore_errors=True)

    # ---- helpers ---------------------------------------------------------
    def make_user(self, name, balance, limit, debt):
        """Create an account file with the given numbers and load it."""
        Path("User Informations").mkdir(exist_ok=True)
        Path("User Informations", f"{name}.txt").write_text(
            f"Username: {name}\nPassword: pw\nBalance: {balance}\nPurchases: 0\n"
            f"Credit Limit: {limit}\nOutstanding Debt: {debt}\n", encoding="utf-8")
        self.store.current = name
        self.store.load_account()
        self.store.cart.clear()

    def fill_cart(self, unit_price, qty=1):
        self.store.cart.clear()
        self.store.add_to_cart("001", "Test Bouquet", unit_price, qty)

    def acct(self):
        return self.store.account

    def saved(self):
        """What is really saved on disk (reload the account file)."""
        self.store.load_account()
        return self.store.account

    def receipts(self, prefix):
        folder = Path("Receipts") / self.store.current
        return [p for p in folder.glob(f"{prefix}-*.txt")] if folder.exists() else []

    def history(self):
        p = Path("User Transaction History") / f"{self.store.current}.txt"
        return p.read_text(encoding="utf-8") if p.exists() else ""

    # ======================= CASH PAYMENT =======================
    def test_cash_sufficient_balance(self):
        self.make_user("cash1", 5000, 10000, 0)
        self.fill_cart(2100)
        ok, receipt = self.store.checkout("Cash")
        self.assertTrue(ok)
        self.assertEqual(self.acct()["balance"], 2900)
        self.assertEqual(self.saved()["balance"], 2900)            # saved to file
        self.assertIn("Updated Balance: P2,900", receipt)          # shown on the receipt
        self.assertIn("Paid Immediately", receipt)
        self.assertEqual(self.acct()["debt"], 0)
        self.assertEqual(self.acct()["purchases"], "1")
        self.assertEqual(self.store.cart, [])
        self.assertEqual(len(self.receipts("ORD")), 1)
        self.assertIn("Cash - P2,100", self.history())

    def test_cash_insufficient_balance(self):
        self.make_user("cash2", 1000, 10000, 0)
        self.fill_cart(2100)
        ok, message = self.store.checkout("Cash")
        self.assertFalse(ok)
        self.assertIn("Insufficient balance", message)
        self.assertEqual(self.acct()["balance"], 1000)             # nothing deducted
        self.assertEqual(self.saved()["balance"], 1000)
        self.assertEqual(self.acct()["purchases"], "0")
        self.assertEqual(self.receipts("ORD"), [])                 # no receipt
        self.assertEqual(self.history(), "")                       # no history line
        self.assertEqual(len(self.store.cart), 1)                  # cart kept

    def test_cash_exact_balance(self):
        self.make_user("cash3", 2100, 10000, 0)
        self.fill_cart(2100)
        ok, _ = self.store.checkout("Cash")
        self.assertTrue(ok)
        self.assertEqual(self.acct()["balance"], 0)
        self.assertEqual(self.saved()["balance"], 0)

    def test_cash_purchase_after_balance_reaches_zero(self):
        self.make_user("cash4", 2100, 10000, 0)
        self.fill_cart(2100)
        self.assertTrue(self.store.checkout("Cash")[0])
        self.fill_cart(100)
        ok, message = self.store.checkout("Cash")
        self.assertFalse(ok)
        self.assertIn("Insufficient balance", message)
        self.assertEqual(self.acct()["balance"], 0)                # not negative
        self.assertEqual(self.saved()["balance"], 0)

    # ======================= CREDIT PAYMENT =======================
    def test_credit_within_limit(self):
        self.make_user("cred1", 500, 10000, 0)
        self.fill_cart(2000, 2)                                    # total 4,000
        ok, receipt = self.store.checkout("Credit")
        self.assertTrue(ok)
        self.assertEqual(self.acct()["debt"], 4000)
        self.assertEqual(self.acct()["balance"], 500)              # cash balance NOT deducted
        self.assertEqual(orderSystem.remaining_credit(self.acct()), 6000)
        self.assertEqual(self.acct()["credit_limit"], 10000)
        saved = self.saved()
        self.assertEqual((saved["debt"], saved["balance"]), (4000, 500))
        self.assertIn("Outstanding Debt: P4,000", receipt)
        self.assertIn("Remaining Credit: P6,000", receipt)
        self.assertIn("Charged to Credit", receipt)
        self.assertIn("Credit - P4,000", self.history())

    def test_credit_exceeding_remaining_credit(self):
        self.make_user("cred2", 500, 10000, 4000)                  # remaining credit 6,000
        self.fill_cart(6001)
        ok, message = self.store.checkout("Credit")
        self.assertFalse(ok)
        self.assertIn("Credit limit exceeded", message)
        self.assertEqual(self.acct()["debt"], 4000)
        self.assertEqual(self.saved()["debt"], 4000)
        self.assertEqual(len(self.store.cart), 1)                  # cart kept
        self.fill_cart(6000)                                       # exactly the remaining credit
        self.assertTrue(self.store.checkout("Credit")[0])
        self.assertEqual(self.acct()["debt"], 10000)
        self.assertEqual(orderSystem.remaining_credit(self.acct()), 0)
        self.fill_cart(1)
        self.assertFalse(self.store.checkout("Credit")[0])         # even P1 more is rejected

    def test_credit_works_with_zero_balance(self):
        self.make_user("cred3", 0, 10000, 0)
        self.fill_cart(1500)
        self.assertTrue(self.store.checkout("Credit")[0])
        self.assertEqual(self.acct()["balance"], 0)
        self.assertEqual(self.acct()["debt"], 1500)

    # ======================= DEBT PAYMENT =======================
    def test_partial_debt_payment(self):
        self.make_user("pay1", 2000, 10000, 4000)
        ok, receipt = self.store.process_debt_payment("1500")
        self.assertTrue(ok)
        self.assertEqual(self.acct()["debt"], 2500)
        self.assertEqual(orderSystem.remaining_credit(self.acct()), 7500)   # credit restored
        self.assertEqual(self.acct()["balance"], 500)                       # taken from the balance
        saved = self.saved()
        self.assertEqual((saved["debt"], saved["balance"]), (2500, 500))
        self.assertIn("Amount Paid: P1,500", receipt)
        self.assertIn("Updated Balance: P500", receipt)
        self.assertIn("Outstanding Debt: P2,500", receipt)
        self.assertIn("Remaining Credit: P7,500", receipt)
        self.assertEqual(len(self.receipts("PAY")), 1)
        self.assertIn("Debt Payment PAY-", self.history())

    def test_debt_payment_with_too_little_balance_is_rejected(self):
        self.make_user("pay2", 500, 10000, 2500)
        ok, message = self.store.process_debt_payment("2500")
        self.assertFalse(ok)
        self.assertIn("Insufficient balance", message)
        self.assertEqual((self.acct()["debt"], self.acct()["balance"]), (2500, 500))
        self.assertEqual((self.saved()["debt"], self.saved()["balance"]), (2500, 500))
        self.assertEqual(self.receipts("PAY"), [])
        self.assertEqual(self.history(), "")

    def test_complete_payoff(self):
        self.make_user("pay3", 3000, 10000, 2500)
        self.assertTrue(self.store.process_debt_payment("2500")[0])
        self.assertEqual(self.acct()["debt"], 0)
        self.assertEqual(self.acct()["balance"], 500)
        self.assertEqual(orderSystem.remaining_credit(self.acct()), 10000)  # full limit again
        self.assertEqual(self.saved()["debt"], 0)

    def test_purchase_again_after_partial_repayment(self):
        self.make_user("pay4", 1000, 10000, 4000)
        self.assertTrue(self.store.process_debt_payment("1000")[0])         # debt 3,000
        self.assertEqual(self.acct()["debt"], 3000)
        self.fill_cart(5000)
        self.assertTrue(self.store.checkout("Credit")[0])
        self.assertEqual(self.acct()["debt"], 8000)
        self.assertEqual(orderSystem.remaining_credit(self.acct()), 2000)
        self.fill_cart(2001)
        self.assertFalse(self.store.checkout("Credit")[0])                  # only 2,000 left

    def test_cannot_pay_more_than_the_debt(self):
        self.make_user("pay5", 9999, 10000, 2000)
        ok, message = self.store.process_debt_payment("2001")
        self.assertFalse(ok)
        self.assertIn("cannot pay more", message)
        self.assertEqual((self.acct()["debt"], self.acct()["balance"]), (2000, 9999))
        self.assertEqual((self.saved()["debt"], self.saved()["balance"]), (2000, 9999))
        self.assertEqual(self.receipts("PAY"), [])
        self.assertEqual(self.history(), "")

    def test_invalid_debt_payments(self):
        self.make_user("pay6", 100, 10000, 2000)
        for bad in ("", "abc", "-5", "1.5", "12 3"):
            ok, message = self.store.process_debt_payment(bad)
            self.assertFalse(ok, bad)
            self.assertIn("whole-number", message)
        ok, message = self.store.process_debt_payment("0")
        self.assertFalse(ok)
        self.assertIn("greater than zero", message)
        self.make_user("pay7", 100, 10000, 0)
        ok, message = self.store.process_debt_payment("50")
        self.assertFalse(ok)
        self.assertIn("no outstanding debt", message)

    # ======================= EXTRA CHECKS =======================
    def test_each_customer_has_an_individual_limit(self):
        self.make_user("small", 0, 3000, 0)
        self.fill_cart(3001)
        self.assertFalse(self.store.checkout("Credit")[0])
        self.make_user("big", 0, 50000, 0)
        self.fill_cart(40000)
        self.assertTrue(self.store.checkout("Credit")[0])

    def test_account_files(self):
        # old file without the credit lines -> default limit, no debt
        Path("User Informations").mkdir(exist_ok=True)
        Path("User Informations", "old.txt").write_text(
            "Username: old\nPassword: pw\nBalance: 700\nPurchases: 2\n", encoding="utf-8")
        self.store.current = "old"
        self.store.load_account()
        a = self.acct()
        self.assertEqual((a["credit_limit"], a["debt"], a["balance"]),
                         (orderSystem.DEFAULT_CREDIT_LIMIT, 0, 700))
        # brand new account
        self.assertIsNone(self.store.register("brandnew", "pw"))
        a = self.acct()
        self.assertEqual((a["credit_limit"], a["debt"], a["balance"]),
                         (orderSystem.DEFAULT_CREDIT_LIMIT, 0, 0))
        file_text = Path("User Informations", "brandnew.txt").read_text(encoding="utf-8")
        self.assertIn("Credit Limit: 10000", file_text)
        self.assertIn("Outstanding Debt: 0", file_text)
        # negative numbers in a file are treated as zero
        Path("User Informations", "brandnew.txt").write_text(
            "Balance: 5\nCredit Limit: -50\nOutstanding Debt: -9\n", encoding="utf-8")
        self.store.load_account()
        self.assertEqual((self.acct()["credit_limit"], self.acct()["debt"]), (0, 0))
        # debt above the limit -> remaining credit shows 0, never negative
        self.assertEqual(orderSystem.remaining_credit({"credit_limit": 1000, "debt": 1500}), 0)

    def test_rollback_when_account_cannot_be_saved(self):
        self.make_user("fail1", 5000, 10000, 0)
        self.fill_cart(2100)
        account_file = Path("User Informations", "fail1.txt")
        account_file.unlink()
        account_file.mkdir()                                        # makes saving impossible
        ok, _ = self.store.checkout("Cash")
        self.assertFalse(ok)
        self.assertEqual(self.acct()["balance"], 5000)              # undone
        self.assertEqual(self.acct()["purchases"], "0")
        self.assertEqual(self.receipts("ORD"), [])                  # receipt removed
        self.assertEqual(len(self.store.cart), 1)                   # cart kept
        self.assertEqual(self.history(), "")

        self.make_user("fail2", 3000, 10000, 2000)
        account_file = Path("User Informations", "fail2.txt")
        account_file.unlink()
        account_file.mkdir()
        ok, _ = self.store.process_debt_payment("500")
        self.assertFalse(ok)
        self.assertEqual((self.acct()["debt"], self.acct()["balance"]), (2000, 3000))
        self.assertEqual(self.receipts("PAY"), [])

    def test_deposit_still_works(self):
        self.make_user("dep", 100, 10000, 0)
        ok, receipt = self.store.deposit("900")
        self.assertTrue(ok)
        self.assertEqual(self.acct()["balance"], 1000)
        self.assertIn("New Balance: P1,000", receipt)
        self.assertEqual(len(self.receipts("DEP")), 1)


if __name__ == "__main__":
    unittest.main(verbosity=2)
